#include "Params.h"

#include <boost/lexical_cast.hpp>

Params::Params(int argc, char* argv[])
{
	if (argc < 3)
		throw std::invalid_argument("Wrong arguments. Usage: <path_to_in_file> "
			"<path_to_output_file> <size_of_block> (optional, MB)");

	/* boost::lexical_cast<size_t> will return signed value for negative numbers. Need to check it */
	if (argc > 3)
	{
		std::string checkerNegativeNum = argv[3];
		if (!checkerNegativeNum.empty())
		{
			if (checkerNegativeNum.find('-') != std::string::npos)
				throw std::invalid_argument("Wrong third argument. <size_of_block> cannot be negative");
		}
	}

	try
	{
		m_sizeOfBlock = argc > 3 ?
			(boost::lexical_cast<size_t>(argv[3]) * constParams::defaultBlockSize) : constParams::defaultBlockSize;

		if (m_sizeOfBlock == 0)
			throw std::invalid_argument("Wrong third argument. <size_of_block> cannot be null");

		m_pathToReadFile = argv[1];
		m_pathToWriteFile = argv[2];
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

std::string Params::GetPathToReadFile() const
{
	return m_pathToReadFile;
}

std::string Params::GetPathToWriteFile() const
{
	return m_pathToWriteFile;
}

size_t Params::GetSizeBlock() const
{
	return m_sizeOfBlock;
}