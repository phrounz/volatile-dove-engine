/******************************************************************
une police de caracteres
Done by Francois Braud
******************************************************************/
	
#include "../include/Utils.h"
#include "../include/Image.h"
#include "../include/FileUtil.h"

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
	#include "opengl/OpenGLDraw.h"
#endif

#include "Font.h"

//---------------------------------------------------------------------

Font* Font::defaultFont = NULL;

//---------------------------------------------------------------------
/**
	constructor
*/

Font::Font(const std::string& replacementTextureMappedFontFile, 
	float relativeScale, int widthSpace, int widthCharReduction)
	:m_relativeScale(relativeScale), m_widthSpace(widthSpace), m_widthCharReduction(widthCharReduction)
{
	const bool verbose = false;
	m_image = new Image(replacementTextureMappedFontFile.c_str());
	m_bitmap = new Bitmap(*m_image, false);
	m_bitmapSmooth = new Bitmap(*m_image, true);

    //Assert(image->getIsAlpha());
	if (verbose) outputln("Loading font size=(" << m_bitmap->size() << ")");

    for (int i=0;i<256;i++)
    {
        tabPos[i] = -1;
        tabWidth[i] = -1;
    }
    int x = 0;
    
    //this->fillTabPosIndex(49,57,x, verbose);//1-9
    //this->fillTabPosIndex(48,48,x, verbose);//0
    //this->fillTabPosIndex(97,122,x, verbose);//a-z
    //this->fillTabPosIndex(65,90,x, verbose);//A-Z
    
    this->fillTabPosIndex(33,126,x, verbose);

    //only the texture remains useful
    //m_bitmap->freeUselessMemory();

	if (verbose) outputln("... done.");
}

//---------------------------------------------------------------------
/**
	Affiche du texte a l'emplacement donne. Renvoie la position atteinte a droite en coordonnee pixel x.
	\param posX position x en pixels
	\param posY position y en pixels
	\param text le texte a afficher
	\param fontScale l'echelle du texte par rapport a la font initiale
	\param nbCharsToRead nombre de caracteres a prendre en compte (-1 = toute la chaine text)
*/

Int2 Font::drawText(const Color& color, Int2 pos, const char *text, Float2 fontScale, int nbCharsToRead, bool writeBold, bool smooth) const
{
	fontScale = fontScale * m_relativeScale;
    Assert(text != NULL);
    int len = nbCharsToRead==-1?(int)strlen(text):nbCharsToRead;
    const int initialPosX = pos.x();

	Bitmap* bitmap = smooth ? m_bitmapSmooth : m_bitmap;
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
	const float opacity = 1.f;
	OpenGLDraw::setColor(color);
#else
	if (color.r() != 255 || color.g() != 255 || color.b() != 255)
	{
		std::vector<ColorPlusBitmap>::const_iterator itFound = m_tintedBitmaps.end();
		iterateVectorConst(m_tintedBitmaps, ColorPlusBitmap)
		{
			if ((*it).first == color) itFound = it;
		}
		if (itFound == m_tintedBitmaps.end())
		{
			Image tmpImage(*m_image, Float4(color.r() / 255.f, color.g() / 255.f, color.b() / 255.f, 1.f));
			bitmap = new Bitmap(tmpImage, smooth);
			m_tintedBitmaps.push_back(ColorPlusBitmap(color, bitmap));
		}
		else
		{
			bitmap = (*itFound).second;
		}
	}
	const float opacity = color.a() / 255.f;
#endif

    //pour chaque caractere de la chaine
    for (int i=0;i<len;i++)
    {
        if (!isSupported(text[i])) //caractere inconnu
            pos.data[0] += 0;
        else if (text[i]==' ') //espace
            pos.data[0] += (int)((float)m_widthSpace*fontScale.x());
        else if (text[i]=='\n') //retour a la ligne
        {
            pos.data[0] = initialPosX;
            pos.data[1] += getHeight(fontScale.y());
        }
        else //autre caractere
        {
            int posXChar = tabPos[(int)text[i]];
            int widthChar = tabWidth[(int)text[i]];

            //on affiche le fragment de texture correspondant
			bitmap->drawFragment(
                pos,
                (float)posXChar / (float)m_image->size().width(),0.0f,
                (float)(posXChar+widthChar) / (float)m_image->size().width(),1.0f,
                (int)((float)widthChar*fontScale.x()),(int)((float)m_image->size().height()*fontScale.y()),
				opacity);
            if (writeBold)
            {
				bitmap->drawFragment(
                    pos,
                    (float)posXChar / (float)m_image->size().width(),0.0f,
                    (float)(posXChar+widthChar+1) / (float)m_image->size().width(),1.0f,
                    (int)((float)widthChar*fontScale.x()),(int)((float)m_image->size().height()*fontScale.y()),
					opacity);
            }

            pos.data[0] += (int)((float)(widthChar-m_widthCharReduction)*fontScale.x());
        }
    }

#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
	OpenGLDraw::resetColor();
#endif
    return pos;
}

//---------------------------------------------------------------------
/**
    Affiche le texte avec un retour a la ligne sans couper les mots.
    Renvoie la position Y atteinte.
    \param lineReturn Nombre de pixels en x avant le retour a la ligne
*/

Int2 Font::drawTextWithLineReturn(const Color& color, const Int2& pos, const char* text, unsigned int lineReturn, const Float2& fontScale, bool writeBold, int nbCharsToRead, bool smooth) const
{
    const int totalWidth = lineReturn;
    int len = nbCharsToRead==-1?(int)strlen(text):nbCharsToRead;
    int heightFont = getHeight(fontScale.y());

    //pour chaque caractere de la chaine
    int j = 0;
    int currentX = 0;
    int currentY = 0;
	Int2 tmpPos;
    for(int i = 0; i < len; ++i)
    {
        if (text[i] == ' ' || i == len-1 || text[i] == '\n')
        {
            int nbCharsWord = i + 1 - j;
            int width = getWidth(&text[j], nbCharsWord, fontScale.x());
            if (currentX + width >= totalWidth)
            {
                currentY += heightFont;
                currentX = 0;
            }
			tmpPos.data[0] = pos.x() + currentX;
			tmpPos.data[1] = pos.y() + currentY;
            this->drawText(color, tmpPos, &text[j], fontScale, nbCharsWord, writeBold, smooth);
            currentX += width;
            if (text[i] == '\n')
            {
                currentY += heightFont;
                currentX = 0;
            }
            j = i + 1;
        }
    }
    return Int2(pos.x() + currentX, pos.y() + currentY);
}

//---------------------------------------------------------------------
/**
	renvoie la largeur que la chaine fait si elle est affichee
	(ne gere pas les retours a la ligne)
	\param nbChars nombre de caracteres max. Si -1, alors toute la chaine est prise.
*/

int Font::getWidth(const char *text,int nbCharsToRead,float fontScale) const
{
    int len = nbCharsToRead!=-1?nbCharsToRead:(int)strlen(text);
	int maxTotalWidth = 0;
    int totalWidth = 0;
    //int tmp;
    for (int i=0;i<len;i++)
    {
        if (isSupported(text[i]))
        {
            if (text[i]==' ')
                totalWidth += (int)((float)m_widthSpace*fontScale);
            else if (text[i]=='\n')
			{
				if (totalWidth > maxTotalWidth) maxTotalWidth = totalWidth;
                totalWidth = 0;
			}
            else
                totalWidth += (int)((float)tabWidth[(int)text[i]]*fontScale)-1;
        }
    }
	return totalWidth > maxTotalWidth ? totalWidth : maxTotalWidth;
}


//---------------------------------------------------------------------
/**
	est-ce que le caractere est supporte par la font
*/

bool Font::isSupported(char c) const
{
    return c==' ' || c=='\n' || tabPos[(int)c]!=-1;
}

//---------------------------------------------------------------------
/**
	destructor
*/

Font::~Font()
{
	iterateVectorConst(m_tintedBitmaps, ColorPlusBitmap)
	{
		delete (*it).second;
	}

    delete m_bitmap;
	delete m_bitmapSmooth;
	delete m_image;
}


//---------------------------------------------------------------------
/**
	remplit tabPos et tabWidth
*/

void Font::fillTabPosIndex(int beginningAscii, int endAscii,int &x,bool verbose)
{
    bool foundText = false;
    int widthImage = m_image->size().width();
    int heightImage = m_image->size().height();
    unsigned char* dataImage = m_image->getDataPixels();

    //cherche le caractere suivant
    while(!foundText)
    {
        //pour chaque pixel de la colonne courante
        for (int j=0;j<heightImage;j++)
        {
            //verifie le canal alpha
            if (dataImage[(j*widthImage+x)*4+3] != 0)
                foundText = true;
        }
        x++;
    }

    for (int i=beginningAscii;i<=endAscii;i++)
    {
        tabPos[i] = x-2;
        if (verbose)
            printf("   x: %c %d\n",(char)i,x);

        //cherche l'espace suivant
		while (foundText && x < widthImage)
        {
			Assert(x < widthImage);
            foundText = false;
            //pour chaque pixel de la colonne courante
            for (int j=0;j<heightImage;j++)
            {
                //verifie le canal alpha
				//if (m_image->getPixel(Int2(x, j)).a() != 0)
				if (dataImage[(j*widthImage + x) * 4 + 3] != 0)
                    foundText = true;
            }

            x++;
        }

		if (x == widthImage) break;

        tabWidth[i] = x+2 - tabPos[i];

        //cherche le caractere suivant
        while(!foundText)
        {
            //pour chaque pixel de la colonne courante
            for (int j=0;j<heightImage;j++)
            {
                //verifie le canal alpha
                if (dataImage[(j*widthImage+x)*4+3] != 0)
                    foundText = true;
            }
            x++;
        }

    }
}


//---------------------------------------------------------------------
/**
	get font height in pixel units (as if displayed with scale fontScale)
*/

int Font::getHeight(float fontScale) const
{
	return (int)(m_image->size().height()*fontScale);
}

int Font::getHeight(const char *text, float fontScale) const
{
	int nbLinesReturn = 0;
	for (size_t i = 0; i < strlen(text); ++i)
	{
		if (text[i] == '\n') ++nbLinesReturn;
	}
	return (int)(m_image->size().height()*fontScale*(1+nbLinesReturn));
}

//---------------------------------------------------------------------

void Font::setDefaultFont(Font* parFont) {defaultFont = parFont;}
	
void Font::deleteDefaultFont() { Assert(defaultFont != NULL);delete defaultFont; }

//---------------------------------------------------------------------
