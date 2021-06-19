#pragma once

#include <string>

namespace constParams
{
	/* 8 bytes (in HEX) */
	const size_t sizeOfCRCHash = 8;
	/* 1 MB */
	const size_t defaultBlockSize = 1024 * 1024; 
};

class Params
{
public:
	Params(int argc, char* argv[]);

	std::string GetPathToReadFile() const;
	std::string GetPathToWriteFile() const;
	size_t GetSizeBlock() const;

private:
	/* argv[1] - path to file whose checksum must be calculated */
	std::string pathToReadFile;
	/* argv[2] - path to file where hash of readed blocks gonna be */
	std::string pathToWriteFile;
	/* argv[3] - size of block file in MB to perform checksum */
	size_t sizeOfBlock;
};
