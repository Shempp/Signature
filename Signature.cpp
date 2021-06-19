#include "Signature.h"

#include <sstream>

#include <boost/crc.hpp>
#include <boost/bind.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

Signature::Signature(const Params& param)
{
	readFile.open(param.GetPathToReadFile());

	sizeToRead = readFile.size();
	sizeOfBlock = param.GetSizeBlock();
	countHashTask = ceil(static_cast<double>(sizeToRead) / sizeOfBlock);

	paramsWriteFile.path = param.GetPathToWriteFile();
	paramsWriteFile.new_file_size = constParams::sizeOfCRCHash * countHashTask;
	paramsWriteFile.flags = boost::iostreams::mapped_file::mapmode::readwrite;

	writeFile.open(paramsWriteFile);
}

void Signature::StartProcessing()
{
	if (readFile.is_open() && writeFile.is_open())
	{
		/* TODO: in some OS std::thread::hardware_concurrency cannot be performed. Need to check it */
		const size_t countCPU = std::thread::hardware_concurrency(); 
		boost::asio::thread_pool thPool(countCPU);

		/* TODO: Try to optimize calculating CRC involving all system cores
		For example, we have 10 GB file and the third argument of the program is 10 GB 
		In that situation the program will calculate the hash of this file in one thread
		The idea is to read the hash of the file blocks in different threads
		and then combine the results into one. How to combine CRC32? We can try use crc32_combine()
		from this lib: https://github.com/madler/zlib/blob/master */
		/* if (countHashTask < countCPU)
		{
			// Getting number of threads for perform one task 
			size_t numOfThreadsForOneTask = ceil(countCPU / countHashTask);
			// Getting number of remaining threads 
			size_t numOfLeftThreads = countCPU - countHashTask * numOfThreadsForOneTask;
			// Need to distribute the remaining threads 
		}
		*/

		for (size_t i = 0; i < countHashTask; i++)
		{
			size_t offsetToRead = sizeOfBlock;

			/* For the last block read the remaining bytes */
			if (sizeOfBlock > sizeToRead)
				offsetToRead = sizeToRead;

			boost::asio::post(thPool, boost::bind(&Signature::GetCRC32Hash, this,
				readFile.data() + i * offsetToRead, offsetToRead, writeFile.data() + i * constParams::sizeOfCRCHash));

			sizeToRead -= sizeOfBlock;
		}

		thPool.join();

		readFile.close();
		writeFile.close();

		if (exPtr)
			std::rethrow_exception(exPtr);
	}
	else
		throw std::runtime_error("Can't open input/output file.");
}

void Signature::GetCRC32Hash(const char* strToRead, const size_t lengthRead, char* strToWrite)
{
	try
	{
		boost::crc_32_type resultCRC;
		resultCRC.process_bytes(strToRead, lengthRead);

		WriteHashToFile(resultCRC.checksum(), strToWrite);
	}
	catch (const std::exception&)
	{
		exPtr = std::current_exception();
	}
}

void Signature::WriteHashToFile(const std::uint32_t& resultCRC, char* strToWrite)
{
	std::stringstream ss;
	ss << std::hex << resultCRC;

	/* It is worth mentioning that an padded null will be skipped
	For example: checksum 0eaf3c32 will be printed in file as eaf3c32 */

	std::string hash = ss.str();
	std::copy(hash.begin(), hash.end(), strToWrite);
}