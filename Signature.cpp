#include "Signature.h"

#include <sstream>

#include <boost/crc.hpp>
#include <boost/bind.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

Signature::Signature(const Params& param)
{
	m_readFile.open(param.GetPathToReadFile());

	m_sizeToRead = m_readFile.size();
	m_sizeOfBlock = param.GetSizeBlock();
	m_countHashTask = ceil(static_cast<double>(m_sizeToRead) / m_sizeOfBlock);

	m_paramsWriteFile.path = param.GetPathToWriteFile();
	m_paramsWriteFile.new_file_size = constParams::sizeOfCRCHash * m_countHashTask;
	m_paramsWriteFile.flags = boost::iostreams::mapped_file::mapmode::readwrite;

	m_writeFile.open(m_paramsWriteFile);
}

void Signature::StartProcessing()
{
	if (m_readFile.is_open() && m_writeFile.is_open())
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

		for (size_t i = 0; i < m_countHashTask; i++)
		{
			size_t offsetToRead = m_sizeOfBlock;

			/* For the last block read the remaining bytes */
			if (m_sizeOfBlock > m_sizeToRead)
				offsetToRead = m_sizeToRead;

			boost::asio::post(thPool, boost::bind(&Signature::GetCRC32Hash, this,
				m_readFile.data() + i * offsetToRead, offsetToRead, m_writeFile.data() + i * constParams::sizeOfCRCHash));

			m_sizeToRead -= m_sizeOfBlock;
		}

		thPool.join();

		m_readFile.close();
		m_writeFile.close();

		if (m_exPtr)
			std::rethrow_exception(m_exPtr);
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
		m_exPtr = std::current_exception();
	}
}

void Signature::WriteHashToFile(const std::uint32_t& resultCRC, char* strToWrite)
{
	std::stringstream ss;
	ss << std::hex << resultCRC;

	/* 
		It is worth mentioning that an padded null will be skipped
		For example: checksum '0eaf3c32' will be printed in file as 'eaf3c32'
	*/

	auto hash = ss.str();
	std::copy(hash.begin(), hash.end(), strToWrite);
}