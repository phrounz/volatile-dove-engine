
#include "lodepng.h"

#include "../include/Utils.h"
#include "../include/FileUtil.h"

#include "../include/Image.h"

//#define DEBUG_NO_IMAGE

//-------------------------------------------------------------------------
// constructors
//-------------------------------------------------------------------------

namespace
{

	bool getContainsTransparency(int width, int height, unsigned char* buffer)
	{
		int countTransparentPixels = 0;
		for (int i = 0; i < width; ++i)
			for (int j = 0; j < height; ++j)
				if (buffer[(width*j + i)*4 + 3] < 250)
					countTransparentPixels++;
		return (float)countTransparentPixels / (float)(width*height) > 0.01f;
	}

}

//-------------------------------------------------------------------------
// constructors
//-------------------------------------------------------------------------

Image::Image(const char* imageFile, bool isInAppData):m_containsTransparency(false)
{
	size_t size;
	outputln("Loading image: " << imageFile);
	unsigned char* buf = FileUtil::readFile(isInAppData?FileUtil::APPLICATION_DATA_FOLDER:FileUtil::APPLICATION_FOLDER, imageFile, &size);
	this->init(buf, size);
	delete [] buf;
}

//-------------------------------------------------------------------------

Image::Image(unsigned char* fileBuffer, size_t fileSize):m_containsTransparency(false)
{
	outputln("Loading image from buffer (" << fileSize << " bytes)");
	this->init(fileBuffer, fileSize);
}

//-------------------------------------------------------------------------

Image::Image(int width, int height):m_containsTransparency(false)
{
	outputln("Loading image with size (" << width << "x" << height << ")");
	m_size = Int2(width, height);
	m_buffer = new unsigned char[width*height*4];
}

//-------------------------------------------------------------------------

Image::Image(const Int2& size) :m_containsTransparency(false)
{
	outputln("Loading image with size (" << size.width() << "x" << size.height() << ")");
	m_size = size;
	m_buffer = new unsigned char[size.width()*size.height() * 4];
}

//-------------------------------------------------------------------------

Image::Image(const Int2& size, const Color& color) :m_containsTransparency(color.a() < 255)
{
	outputln("Loading image with color (" << color << ")");
	m_size = size;
	m_buffer = new unsigned char[size.width()*size.height() * 4];
	for (int i = 0; i < m_size.width(); ++i)
		for (int j = 0; j < m_size.height(); ++j)
			this->setPixel(Int2(i, j), color);
	//outputln("color: " << (int)color.r() << "," << (int)color.g() << "," << (int)color.b() << "," << (int)color.a() << ";");
	//for (int i=0;i<size.width()*size.height();++i) {this->getDataPixels()[i*4]=(int)color.r();this->getDataPixels()[i*4+1]=(int)color.g();this->getDataPixels()[i*4+2]=(int)color.b();this->getDataPixels()[i*4+3]=(int)color.a();}
}

//-------------------------------------------------------------------------

Image::Image(const Image& original, int division)
{
	outputln("Loading image with division " << division);
	int width = original.getWidth() / division;
	int height = original.getHeight() / division;
	m_size = Int2(width, height);
	m_buffer = new unsigned char[width*height * 4];
	this->copyFromImageWithScaleReduce(original, division);
}

//-------------------------------------------------------------------------

Image::Image(const Image& original, const Float4& colorFactor)
{
	outputln("Loading image with color factor (" << colorFactor << ")");
	m_size = original.m_size;
	m_buffer = new unsigned char[m_size.width()*m_size.height() * 4];
	for (int i = 0; i < m_size.width(); ++i)
	{
		for (int j = 0; j < m_size.height(); ++j)
		{
			Color px = original.getPixel(i, j);
			Color newpx = Color(
				(u8)(px.r()*colorFactor.r()), u8(px.g()*colorFactor.g()), (u8)(px.b()*colorFactor.b()), (u8)(px.a()*colorFactor.a()));
			this->setPixel(Int2(i, j), newpx);
		}
	}
	Assert(this->getBytesPerPixel()==4 && this->getIsAlpha());
	m_containsTransparency = getContainsTransparency(m_size.width(), m_size.height(), m_buffer);
}

//-------------------------------------------------------------------------

Image::Image(const Image& original, int x1, int y1, int x2, int y2)
{
	AssertRelease(x1 < x2 && y1 < y2);
	int width = x2 - x1;
	int height = y2 - y1;
	outputln("Loading image from fragment of another image (size " << width << "x" << height << ")");
	m_size = Int2(width, height);
	m_buffer = new unsigned char[width*height * 4];
	m_containsTransparency = original.m_containsTransparency;
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			Color px = original.getPixel(i + x1, j + x1);
			this->setPixel(Int2(i, j), px);
		}
	}
}

//-------------------------------------------------------------------------
// misc
//-------------------------------------------------------------------------

void Image::init(unsigned char* fileBuffer, size_t fileSize)
{
	unsigned int width = -1;
	unsigned int height = -1;
#ifdef DEBUG_NO_IMAGE
	width = 64;
	height = 64;
	m_buffer = new unsigned char[width * height * this->getBytesPerPixel()];
	for (int i = 0; i < width*height*this->getBytesPerPixel(); ++i) m_buffer[i] = (unsigned char)255;//'\0';
	int result = 0;
#else
	unsigned int result = LodePNG_decode32(&m_buffer, &width, &height, fileBuffer, static_cast<size_t>(fileSize));
	//unsigned int result = 0;m_buffer = new unsigned char[9*4];width = 3;height = 3;
#endif
	Assert(result == 0);

	Assert(this->getBytesPerPixel()==4 && this->getIsAlpha());
	m_containsTransparency = getContainsTransparency(width, height, m_buffer);

	m_size = Int2(width, height);
}

//-------------------------------------------------------------------------

void Image::copyFromImageWithScaleReduce(const Image& original, int division)
{
	Assert(m_size.width() == original.getWidth() / division);
	Assert(m_size.height() == original.getHeight() / division);
	m_containsTransparency = original.m_containsTransparency;
	int divisionSquare = division*division;
	for (int i = 0; i < m_size.width(); ++i)
	{
		for (int j = 0; j < m_size.height(); ++j)
		{
			int totalR = 0;
			int totalG = 0;
			int totalB = 0;
			int totalA = 0;
			for (int i2 = 0; i2 < division; ++i2)
			{
				for (int j2 = 0; j2 < division; ++j2)
				{
					Color px = original.getPixel(i*division+i2, j*division+j2);
					totalR += px.r();
					totalG += px.g();
					totalB += px.b();
					totalA += px.a();
				}
			}
			Color average(totalR/divisionSquare, totalG/divisionSquare, totalB/divisionSquare, totalA/divisionSquare);
			/*// for visualisation of mip maps
			if (division == 2) average.data[0] = 255;
			else if (division == 4) average.data[1] = 255;
			else if (division == 8) average.data[2] = 255;*/
			this->setPixel(Int2(i, j), average);
		}
	}
}

//---------------------------------------------------------------------

Color Image::getPixel(const Int2& pos) const
{
	int offset = getPixelOffsetInBuffer(pos);
	return Color(m_buffer[offset], m_buffer[offset+1], m_buffer[offset+2], getIsAlpha()?m_buffer[offset+3]:255);
}

//---------------------------------------------------------------------

void Image::setPixel(const Int2& pos, const Color& color)
{
	int offset = getPixelOffsetInBuffer(pos);
	m_buffer[offset+0] = color.r();
	m_buffer[offset+1] = color.g();
	m_buffer[offset+2] = color.b();
	m_buffer[offset+3] = color.a();
}

//-------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------

Image::~Image()
{
	outputln("Unloading image");
	delete[] m_buffer;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
