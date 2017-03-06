#include "tga.hpp"

TGAImage::TGAImage() : data(NULL), width(0), height(0), bytespp(0) 
{

}

TGAImage::TGAImage(uint w, uint h, uint bpp) : data(NULL), width(w), height(h), bytespp(bpp) 
{
	uint nbytes = width * height * bytespp;
	data = new byte[nbytes];
	memset(data, 0, nbytes);
}

TGAImage::TGAImage(const TGAImage &img) : data(NULL), width(img.w), height(img.h), bytespp(img.bpp) 
{
	uint nbytes = width * height * bytespp;
	data = new byte[nbytes];
	memcpy(data, img.data, nbytes);
}

TGAImage::~TGAImage() 
{
	if (data != nullptr) delete [] data;
}

TGAImage * TGAImage::operator =(const TGAImage &img)
{
	if (this != &img) {
		if (data != nullptr) delete [] data;
		width = img.width;
		height = img.height;
		bytespp = img.bytespp;

		uint nbytes = width * height * bytespp;
		data = new byte[nbytes];
		memcpy(data, img.data, nbytes);
	}

	return *this;
}

bool TGAImage::ReadFile(const char *filename)
{
	if (data) delete [] data;
	data = nullptr;

	std::ifstream in;
	in.open(filename, std::ios::binary);

	if(!in.is_open()) {
		std::cerr << "Can't open file " << filename << "." << std::endl;
		in.close();
		return false;
	}

	TGAHeader header;
	in.read((char*) &header, sizeof(header));
	if (!in.good()) {
		in.close();
		std::cerr << "An error occurred while reading the header." << std::endl;
		return false;
	}

	width = header.width;
	height = header.height;
	bytespp = header.bitPerPixel >> 3;

	if (width <= 0 || height << 0 || (bytespp != GREYSCALE && bytespp != RGB && bytespp != RGBA)) {
		in.close();
		std::cerr << "Bad bbp (or width/height) value." << std::endl;
		return false;
	}

	uint nbytes = width * height * bytespp;
	data = new byte[nbytes];

	if (header.dataTypeCode == 3 || header.dataTypeCode == 2) {
		in.read((char*)data, nbytes);
		if (!in.good()) {
			in.close();
		std::cerr << "An error occurred while reading the data." << std::endl;
		return false;		
		}
	}

	else if (header.dataTypeCode == 10 || header.dataTypeCode == 11) {
		if (!RLELoadData(in)) {
			in.close();
			std::cerr << "An error occurred while reading the data." << std::endl;
			return false;
		}
	}

	else {
		in.close();
		std::cerr << "Unknown data format." << std::endl;
		return false;
	}

	if (!(header.imageDescriptor & 0x20)) {
		FlipVertically();
	}

	if (!(header.imageDescriptor & 0x10)) {
		FlipHorizontally();
	}

	std::cerr << width << "x" << height << "/" << bytespp * 8 << std::endl;
	in.close();
	return true;
}

bool TGAImage::RLELoadData(std::ifstream &in)
{
	uint pixelCount = width * height;
	uint currentPixel = 0;
	uint currentByte = 0;
	TGAColor colorBuffer;

	do {
		byte chunkHeader = 0;
		chunkHeader = in.get();

		if (!in.good()) {
			in.close();
			std::cerr << "An error occurred while reading the data." << std::endl;
			return false;
		}

		if (chunkHeader < 128) {
			++chunkHeader;

			for (uint i = 0; i < chunkHeader; ++i) {
				in.read((char*)colorBuffer.brga, bytespp);
				if (!in.good()) {
					in.close();
					std::cerr << "An error occurred while reading the header." << std::endl;
					return false;
				}

				for (uint t = 0; t < bytespp; ++t)
					data[currentByte++] = colorBuffer.brga[t];

				currentPixel++;

				if (currentPixel > pixelCount){
					std::cerr << "Too many pixels read." << std::endl;
					return false;
				}
			}
		}

		else {
			chunkHeader -= 127;
			in.read((char*)colorBuffer.brga, bytespp);
			if (!in.good()) {
				in.close();
				std::cerr << "An error occurred while reading the header." << std::endl;
				return false;
			}

			for (uint i = 0; i < chunkHeader; ++i) {
				for (uint t = 0; t < bytespp; ++t)
					data[currentByte++] = colorBuffer.brga[t];

				currentPixel++;

				if (currentPixel > pixelCount){
					std::cerr << "Too many pixels read." << std::endl;
					return false;
				}	
			}
		}
	} while (currentPixel < pixelCount);

	return true;
}
