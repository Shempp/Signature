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
	size_t m_sizeToRead;
	size_t m_sizeOfBlock;
	/* Count of checksum tasks */
	size_t m_countHashTask;

	boost::iostreams::mapped_file_source m_readFile;
	boost::iostreams::mapped_file_params m_paramsWriteFile;
	boost::iostreams::mapped_file_sink m_writeFile;

	std::exception_ptr m_exPtr;
};