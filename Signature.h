#pragma once

#include "Params.h"

#include <boost/iostreams/device/mapped_file.hpp>

class Signature
{
public:
	Signature(const Params& param);

	void StartProcessing();

private:
	void GetCRC32Hash(const char* strToRead, const size_t lengthRead, char* strToWrite);
	void WriteHashToFile(const std::uint32_t& resultCRC, char* strToWrite);

	/* Changes at runtime. Contains information how much more to read */
	size_t sizeToRead;
	size_t sizeOfBlock;
	/* Count of checksum tasks */
	size_t countHashTask;

	boost::iostreams::mapped_file_source readFile;
	boost::iostreams::mapped_file_params paramsWriteFile;
	boost::iostreams::mapped_file_sink writeFile;

	std::exception_ptr exPtr;
};