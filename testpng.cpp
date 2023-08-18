#include <iostream> 
#include <fstream> 




namespace system
{
	auto getVectorOfBytes(const char* filename, unsigned long n, std::ios::openmode mode = std::ios::binar)
	{
		//return a vector of bytes with the N first bytes, if n = 0 return them all;

		auto file = std::ifstream(filename, mode);
		if (!file.is_open())
		{
			std::cout << "Error opening file " << filename << std::endl;
			return std::vector<unsigned char>();
		}
		else
		{
			auto bytes = std::vector<unsigned char>();
			if (n == 0)
			{
				file.seekg(0, std::ios::end);
				n = file.tellg();
				file.seekg(0, std::ios::beg);
			}
			bytes.resize(n);
			file.read((char*)bytes.data(), n);
			file.close();
			return bytes;
		}
	}
}

namespace png
{
	
	struct chunk
	{
		const unsigned long length;
		const unsigned char type[4];
		std::vector<unsigned char> data{std::vector<unsigned char>(length)};
		unsigned long crc;
		chunk(unsigned long length, const unsigned char type[4], std::vector<unsigned char> data, unsigned long crc) : length(length), type{ type[0], type[1], type[2], type[3] }, data(data), crc(crc) {}
	
		static inline chunk createChunk(std::vector<unsigned char> fileData)
		{
			auto length = (fileData[0] << 24) + (fileData[1] << 16) + (fileData[2] << 8) + fileData[3];
			auto type = std::vector<unsigned char>(fileData.begin() + 4, fileData.begin() + 8);
			auto data = std::vector<unsigned char>(fileData.begin() + 8, fileData.begin() + 8 + length);
			auto crc = (fileData[8 + length] << 24) + (fileData[8 + length + 1] << 16) + (fileData[8 + length + 2] << 8) + fileData[8 + length + 3];
			return chunk(length, type.data(), data, crc);
		}

	};
	
	struct ihdr : public chunk
	{
		const unsigned long width;
		const unsigned long height;
		const unsigned char bitDepth;
		const unsigned char colorType;
		const unsigned char compressionMethod;
		const unsigned char filterMethod;
		const unsigned char interlaceMethod;
		ihdr(unsigned long length, const unsigned char type[4], std::vector<unsigned char> data, unsigned long crc) : chunk(length, type, data, crc)
		{}
		
		static inline ihdr createIHDR(std::vector<unsigned char> fileData)
		{
			auto width = (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
			auto height = (data[4] << 24) + (data[5] << 16) + (data[6] << 8) + data[7];
			auto bitDepth = data[8];
			auto colorType = data[9];
			auto compressionMethod = data[10];
			auto filterMethod = data[11];
			auto interlaceMethod = data[12];
			return ihdr(length, type.data(), data, crc, width, height, bitDepth, colorType, compressionMethod, filterMethod, interlaceMethod);
		}

	};
	}

	bool testPng(std::vector<unsigned char> bytes)
	{
		//check if the file is a png file
		if (bytes.size() < 8)
		{
			std::cout << "File is too small to be a png file" << std::endl;
			return false;
		}
		else
		{
			//declare a vector in hex format of the first 8 bytes of a png file
			std::vector<unsigned char> pngHeader = { 0x89, 0x50, 0x4E, 0x47, 0x0D ,0x0A ,0x1A ,0x0A };
			for (int i = 0; i < 8; i++)
			{
				if (bytes[i] != pngHeader[i])
				{
					std::cout << "File is not a png file" << std::endl;
					return false;
				}
			}
			return true;
		}
	}
}

