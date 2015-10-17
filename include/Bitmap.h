#ifndef Bitmap_h_INCLUDED
#define Bitmap_h_INCLUDED

#include <map>

#include "CoreUtils.h"
#include "BoundingBoxes.h"

class Image;
struct ID2D1Bitmap;

//---------------------------------------------------------------------
/**
* \brief A 32 bits RGBA drawable pixmap
*/

class Bitmap
{
public:
	Bitmap(const char* fileName);									///< create a bitmap from a PNG file path
	Bitmap(unsigned char* fileBuffer, unsigned int fileSize);		///< create a bitmap from a PNG file data
	Bitmap(const Image& image, bool useMipmap);						///< create a bitmap from the @param image

	Int2 size() const;												///< get the width and height of the bitmap
	bool getIsAlpha() const {return m_isAlpha;}						///< return true if the image contains alpha channel (= currently returns always true)

	/// Draw the image on the position @param pos, with an opacity @param opacity (set 1.f for opaque), and a scale @param scale (set Float2(1.f,1.f) for original size)
	void draw(const Int2& pos, float opacity, const Float2& scale) const;
	// Draw only a fragment (x1,y1)-(x2,y2) of the original image
	void drawFragment(const Int2& pos, float x1, float y1, float x2, float y2, int width, int height, float opacity) const;
	void drawRotated(const Int2& pixelPos, const Int2& size, float angleDegree, const Int2& rotationCenterPos, bool mirrorAxisX, bool mirrorAxisY, float opacity) const;
	void drawRotated(const BoundingBoxes::AdvancedBox& boundingBox, bool mirrorAxisX, bool mirrorAxisY, float opacity) const;

	/// destructor
	~Bitmap();

private:	
	ID2D1Bitmap* m_bitmap;
	bool m_isAlpha;
	Int2 m_bitmapSize;
	std::string m_fileName;

	void initFromImage(const Image& image, bool useMipmap);
	void copyFromBuffer(const Int2& posInBitmap, const Int2& size, const unsigned char* src);
};

//---------------------------------------------------------------------

#endif //Bitmap_h_INCLUDED
