# Signature of the file
Console program on C++ to generate the signature of the specified file. The signature is generated as follows: the original file is divided into blocks of equal (fixed) length (if the file size is not a multiple of the block size, the last fragment can be smaller or supplemented with zeros to the size of the full block). For each block, the hash value of the function is calculated and added to the output signature file.
## Before started
To build the program you need a [boost](https://www.boost.org/) library.

The project was created in the Visual Studio IDE and necessary paths for the boost library are already registered in settings of the project. However, your paths to library can differ.
* In Configuration Properties > C/C++ > General > Additional Include Directories, enter the path to the Boost root directory. For example:
```
C:\Program Files\boost\boost_1_73_0
```
* In Configuration Properties > Linker > Additional Library Directories, enter the path to the Boost binaries. For example:
```
C:\Program Files\boost\boost_1_73_0\stage\lib\
```
## Usage
Template:
```
Signature.exe <path_to_in_file> <path_to_output_file> <size_of_block>
```
For example:
```
Signature.exe C:\Users\TestUser\FileIn.txt C:\Users\TestUser\FileOut.txt 1
```
