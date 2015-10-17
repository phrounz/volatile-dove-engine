#ifndef Image_h_INCLUDED
#define Image_h_INCLUDED

#include "CoreUtils.h"

//---------------------------------------------------------------------
/**
* \brief A 32 bits RGBA image
*/

class Image
{
public:
	Image(const char* imageFile, bool isInAppData = false);			///< create an image from a PNG file path
	Image(unsigned char* fileBuffer, size_t fileSize);				///< create an image from a PNG file data
	Image(int width, int height);									///< create an image with an unitialized buffer
	Image(const Int2& size);										///< create an image with an unitialized buffer
	Image(const Image& original, int division);						///< create a rescaled copy
	Image(const Image& original, int x1, int y1, int x2, int y2);	///< create a copy which contains only a part of the original picture
	Image(const Image& original, const Float4& colorFactor);		///< create a copy with modified color values
	Image(const Int2& size, const Color& color);					///< create an image filled with a given color

	inline const Int2& size()			const {return m_size;}
	inline int getWidth()				const {return size().width();}
	inline int getHeight()				const {return size().height();}
	inline int getBytesPerPixel()		const {return 4;}
	inline bool getIsAlpha()			const {return true;}					///< does image contains alpha channel
	inline bool containsTransparency()	const {return m_containsTransparency;}	///< does image contains alpha channel AND this alpha channel is sometimes less than 255

	unsigned char* getDataPixels() {return m_buffer;}
	const unsigned char* getDataPixels() const {return m_buffer;}
	Color getPixel(const Int2& pos) const;
	inline Color getPixel(int x, int y) const
	{
		int offset = (m_size.width()*y + x)*getBytesPerPixel();
		Assert(x < m_size.width() && y < m_size.height() && offset >= 0 
			&& offset <= m_size.width()*m_size.height()*getBytesPerPixel() - getBytesPerPixel());
		return Color(m_buffer[offset], m_buffer[offset+1], m_buffer[offset+2], getIsAlpha()?m_buffer[offset+3]:255);
	}
	void setPixel(const Int2& pos, const Color& color);

	inline int getPixelOffsetInBuffer(const Int2& pos) const
	{
		return (m_size.width()*pos.y() + pos.x())*getBytesPerPixel();
	}

	void copyFromImageWithScaleReduce(const Image& original, int division);

	~Image();

private:
	void init(unsigned char* fileBuffer, size_t fileSize);
	Int2 m_size;
	unsigned char* m_buffer;
	bool m_containsTransparency;
};

//---------------------------------------------------------------------

#endif //Image_h_INCLUDED
