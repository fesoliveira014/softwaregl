#ifndef TGA_H
#define TGA_H

#include "common.hpp"

#pragma pack(push, 1)
struct TGAHeader 
{
	byte idLength;
	byte colorMapType;
	byte imageType;
	unsigned short colorMapOrigin;
	unsigned short colorMapLength;
	byte colorMapDepth;
	unsigned short xOrigin;
	unsigned short yOrigin;
	unsigned short width;
	unsigned short height;
	byte bitsPerPixel;
	byte imageDescriptor;
};
#pragma pack(pop)

struct TGAColor 
{
	byte brga[4];
	byte bytespp;

	TGAColor() : brga(), bytespp(1) 
	{
		for (uint i = 0; i < 4; ++i) brga[i] = 0;
	}

	TGAColor(byte b, byte g, byte r, byte a)
	{
		brga[0] = b;
		brga[1] = g;
		brga[2] = r;
		brga[3] = a;
	}

	TGAColor(byte v) : brga(), bytespp(1)
	{
		for (uint i = 0; i < 4; ++i) brga[i] = 0;	
		brga[0] = v;
	}

	TGAColor(byte* p, byte bpp) : brga(), bytespp(bpp)
	{
		for (uint i = 0; i < 4; ++i) 
			brga[i] = p[i];

		for (uint i = bpp; i < 4; ++i) 
			brga[i] = 0;
	}

	byte& operator[](const uint i) {return brga[i];}

	TGAColor operator *(float intensity) const 
	{
		TGAColor res = *this;
		intensity = (intensity > 1.f ? 1.f : (intensity < 0 ? : 0.f, intensity));
		
		for (uint i = 0; i < 4; ++i)
			res.brga[i] = brga[i] * intensity;

		return res;
	}
};

struct TGAImage
{
protected:
	byte * data;
	uint width;
	uint height;
	uint bytespp;

	bool   RLELoadData(std::ifstream &in);
	bool RLEUnloadData(std::ofstream &out);

public:
	enum Format {
		GRAYSCALE=1, RGB=3, RGBA=4
	};

	TGAImage() {};
	TGAImage(uint w, uint h, int bpp);
	~TGAColor();

	bool ReadFile(const char *filename);
	bool WriteFile(const char *filename, bool rle=true);

	bool FlipHorizontally();
	bool FlipVertically();
	bool Scale(uint w, uint h);

	TGAColor Get(uint x, uint y);
	bool Set(uint x, uint y, TGAColor &c);
	bool Set(uint x, uint y, const TGAColor &c);
	
	TGAImage & operator =(const TGAImage &img);

	uint GetWidth();
	uint GetHeight();
	uint GetBytesPP();
	byte *GetBuffer();
	void Clear();
};

#endif // TGA_H