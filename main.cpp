#include "Params.h"
#include "Signature.h"

#include <iostream>
#include <chrono>

int main(int argc, char* argv[])
{
	auto startTime = std::chrono::steady_clock::now();

	try
	{
		Params param(argc, argv);
		Signature sign(param);

		sign.StartProcessing();
 	}
 	catch (const std::exception& ex)
 	{
		std::cout << "Program terminated." << std::endl << ex.what() << std::endl;
 		return 1;
 	}

	auto endTime = std::chrono::steady_clock::now();
	std::cout << "Execution time = " << 
		std::chrono::duration <double, std::milli>(endTime - startTime).count() << " ms" << std::endl;

	return 0;
}