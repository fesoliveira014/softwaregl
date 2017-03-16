#include "tga.hpp"

TGAImage::TGAImage() : data(nullptr), width(0), height(0), bytespp(0) 
{

}

TGAImage::TGAImage(uint w, uint h, uint bpp) : data(nullptr), width(w), height(h), bytespp(bpp)
{
	uint nbytes = width * height * bytespp;
	data = new byte[nbytes];
	memset(data, 0, nbytes);
}

TGAImage::TGAImage(const TGAImage &img) : data(nullptr), width(img.width), height(img.height), bytespp(img.bytespp)
{
	uint nbytes = width * height * bytespp;
	data = new byte[nbytes];
	memcpy(data, img.data, nbytes);
}

TGAImage::~TGAImage() 
{
	if (data != nullptr) delete [] data;
}

TGAImage & TGAImage::operator =(const TGAImage &img)
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
	bytespp = header.bitsPerPixel >> 3;

	if (width <= 0 || height << 0 || (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA)) {
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

bool TGAImage::WriteFile(const char* filename, bool rle)
{
    byte devAreaRef[4] = { 0,0,0,0 };
    byte extensionAreaRef[4] = { 0,0,0,0 };
    byte footer[18] = { 'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0' };
    std::ofstream out;

    out.open(filename, std::ios::binary);
    out.imbue(std::locale::classic());
    if (!out.is_open()) {
        std::cerr << "Can't open file " << filename << "." << std::endl;
        out.close();
        return false;
    }

    TGAHeader header;
    memset((void*)&header, 0, sizeof(header));

    header.bitsPerPixel = (byte)(bytespp << 3);
    header.width = (byte)width;
    header.height = (byte)height;
    header.dataTypeCode = (byte)(bytespp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
    header.imageDescriptor = 0x20; // top-left origin

    std::cerr << "original data" << std::endl;
    std::cerr << "  bpp         : " << std::hex << bytespp << std::endl;
    std::cerr << "  width       : " << std::hex << width << std::endl;
    std::cerr << "  height      : " << std::hex << height << std::endl;
    std::cerr << "  dataTypeCode: " << std::hex << (bytespp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2))  << std::endl;
    std::cerr << "  imgDesc     : " << std::hex << 0x20 << std::endl;

    std::cerr << std::endl;

    std::cerr << "header" << std::endl;
    std::cerr << "  bpp         : " << std::hex << (uint)header.bitsPerPixel << std::endl;
    std::cerr << "  width       : " << std::hex << header.width << std::endl;
    std::cerr << "  height      : " << std::hex << header.height << std::endl;
    std::cerr << "  dataTypeCode: " << std::hex << (uint)header.dataTypeCode << std::endl;
    std::cerr << "  imgDesc     : " << std::hex << (uint)header.imageDescriptor << std::endl;

    out.write((char*)&header, sizeof(header));
    if (!out.is_open()) {
        std::cerr << "Can't open file " << filename << "." << std::endl;
        out.close();
        return false;
    }

    if (!rle) {
        out.write((char*)data, width * height * bytespp);
        if (!out.is_open()) {
            std::cerr << "Can't open file " << filename << "." << std::endl;
            out.close();
            return false;
        }
    }

    out.write((char*)devAreaRef, sizeof(devAreaRef));
    if (!out.is_open()) {
        std::cerr << "Can't open file " << filename << "." << std::endl;
        out.close();
        return false;
    }

    out.write((char*)extensionAreaRef, sizeof(extensionAreaRef));
    if (!out.is_open()) {
        std::cerr << "Can't open file " << filename << "." << std::endl;
        out.close();
        return false;
    }

    out.write((char*)footer, sizeof(footer));
    if (!out.is_open()) {
        std::cerr << "Can't open file " << filename << "." << std::endl;
        out.close();
        return false;
    }

    out.close();
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

bool TGAImage::RLEUnloadData(std::ofstream &out)
{
    const byte maxChunkLength = 128;
    uint npixels = width * height;
    byte currentPixel = 0;

    while (currentPixel < npixels) {
        uint chunkStart = currentPixel * bytespp;
        uint currentByte = currentPixel * bytespp;
        byte runLength = 1;
        bool raw = true;

        while (currentPixel + runLength < npixels && runLength < maxChunkLength) {
            bool nextEqual = true;
            
            for (uint t = 0; nextEqual && t < bytespp; ++t)
                nextEqual = (data[currentByte + t] == data[currentByte + t + bytespp]);

            currentByte += bytespp;
            
            if (runLength == 1)
                runLength = !nextEqual;

            if (raw && nextEqual) {
                runLength--;
                break;
            }

            if (!raw && !nextEqual)
                break;

            runLength++;
        }
        currentPixel += runLength;
        
        out.put(raw ? runLength - 1 : runLength + 127);
        if (!out.good()) {
            std::cerr << "Could not dump TGA file." << std::endl;
            return false;
        }

        out.write((char *)(data + chunkStart), (raw ? runLength * bytespp : bytespp));
        if (!out.good()) {
            std::cerr << "Could not dump TGA file." << std::endl;
            return false;
        }
    }

    return true;
}

TGAColor TGAImage::Get(uint x, uint y)
{
    if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
        return false;

    return TGAColor(data + (x + y * width) * bytespp, byte(bytespp));
}

bool TGAImage::Set(uint x, uint y, TGAColor &c)
{
    if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
        return false;

    memcpy(data + (x + y * width) * bytespp, c.brga, bytespp);
    return true;
}

bool TGAImage::Set(uint x, uint y, const TGAColor &c)
{
    if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height)
        return false;

    memcpy(data + (x + y * width) * bytespp, c.brga, bytespp);
    return true;
}

uint TGAImage::GetBytesPP()
{
    return bytespp;
}

uint TGAImage::GetWidth()
{
    return width;
}

uint TGAImage::GetHeight()
{
    return height;
}

bool TGAImage::FlipHorizontally()
{
    if (data == nullptr) return false;

    uint half = width >> 1;

    for (uint i = 0; i < half; ++i) {
        for (uint j = 0; j < height; ++j) {
            TGAColor c1 = Get(i, j);
            TGAColor c2 = Get(width - 1 - i, j);
            Set(i, j, c2);
            Set(width - 1 - i, j, c1);
        }
    }
    return true;
}

bool TGAImage::FlipVertically()
{
    if (data == nullptr) return false;

    uint bytesPerLine = width * bytespp;
    byte * line = new byte[bytesPerLine];
    uint half = height >> 1;

    for (uint j = 0; j < half; ++j) {
        uint l1 = j * bytesPerLine;
        uint l2 = (height - 1 - j) * bytesPerLine;
        memmove((void *)line,        (void *)(data + l1), bytesPerLine);
        memmove((void *)(data + l1), (void *)(data + l2), bytesPerLine);
        memmove((void *)(data + l2), (void *)line,        bytesPerLine);
    }

    delete[] line;
    return true;
}

byte * TGAImage::GetBuffer()
{
    return data;
}

void TGAImage::Clear()
{
    memset((void *)data, 0, width * height * bytespp);
}

bool TGAImage::Scale(uint w, uint h)
{
    if (w <= 0 || h <= 0 || data == nullptr) return false;

    byte * tdata = new byte[w*h*bytespp];
    int newScanLine = 0, oldScanLine = 0;
    int erry = 0;
    uint newLinesBytes = w * bytespp; 
    uint oldLinesBytes = width * bytespp;

    for (uint j = 0; j < height; ++j) {
        int errx = width - w;
        int nx = -(int)(bytespp);
        int ox = -(int)(bytespp);

        for (uint i = 0; i < height; ++i) {
            ox += bytespp;
            errx += w;
            
            while (errx >= (int)width) {
                errx -= width;
                nx += bytespp;
                memcpy(tdata + newScanLine + nx, data + oldScanLine + ox, bytespp);
            }
        }

        erry += h;
        oldScanLine += oldLinesBytes;
        
        while (erry >= (int)height) {
            if (erry >= (int)height << 1)
                memcpy(tdata + newScanLine + newLinesBytes, tdata + newScanLine, newLinesBytes);
            erry -= height;
            newScanLine += newLinesBytes;
        }
    }

    delete[] data;
    data = tdata;
    width = w;
    height = h;

    return true;
}