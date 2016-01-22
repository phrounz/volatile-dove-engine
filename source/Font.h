/******************************************************************
une police de caracteres, cree a partir d'un fichier image,
contenant les caracteres les uns apres les autres
Done by Francois Braud
******************************************************************/
#ifndef Font_h_INCLUDED
#define Font_h_INCLUDED

#include "../include/global_defines.h"
#include "../include/CoreUtils.h"
#include "../include/Bitmap.h"
#include "../include/Engine.h"

class Image;

//---------------------------------------------------------------------

class Font
{
	friend class AppSetup;
	//friend class TextElement;
public:
	Font(
		const std::string& replacementTextureMappedFontFile, // path of replacement texture containing all the characters
		float relativeScale = 1.f, // relative scale of replacement texture, when comparing to fontName
		int widthSpace = 6, // size of Space (' ') character (relative to text size and relativeScale)
		int widthCharReduction = 2);// reduction between characters (relative to text size and relativeScale)

    //! Destructor.
    ~Font();

	//! Get default font. Return NULL is not set.
    static const Font* getDefaultFont() {return defaultFont;}

	//! Display the text. Return the position reached at right.
    //! (writeInItalic not yet implemented)
    Int2 drawText(const Color& color, Int2 pos, const char *text, Float2 fontScale, int nbCharsToRead, bool writeBold, bool smooth) const;
		//const Color& color, Int2 pos, const char *text, Float2 fontScale = Float2(1.f, 1.f), int nbCharsToRead = -1, bool writeBold = false, bool smooth = false) const;

    //! Display the text with line return.
    Int2 drawTextWithLineReturn(const Color& color, const Int2& pos, const char* text, unsigned int lineReturn, const Float2& fontScale, bool writeBold, int nbCharsToRead, bool smooth) const;

    //! Return the width of the string in pixels as if it was displayed.
    int getWidth(const char *text,int nbCharsToRead = -1,float fontScale = 1.0f) const;

    //! Return the font height in pixels
	int getHeight(float fontScale = 1.0f) const;
	int getHeight(const char *text, float fontScale) const;
	
    //! Is this character supported by the font.
    bool isSupported(char c) const;

private:

	//! Set default font. Can be NULL.
    static void setDefaultFont(Font* parFont);
	static void deleteDefaultFont();

    static Font* defaultFont;

	Image* m_image;
	Bitmap* m_bitmap;
	Bitmap* m_bitmapSmooth;

	int tabPos[256];
    int tabWidth[256];
	int m_widthSpace;

	float m_relativeScale;
	int m_widthCharReduction;
	void fillTabPosIndex(int beginningAscii, int endAscii, int& x, bool verbose);

	typedef std::pair<Color, Bitmap*> ColorPlusBitmap;
	mutable std::vector<ColorPlusBitmap> m_tintedBitmaps;
};

//---------------------------------------------------------------------

#endif //Font_h_INCLUDED
