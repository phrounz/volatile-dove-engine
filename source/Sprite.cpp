
#include "../include/Bitmap.h"

#include "../include/Sprite.h"

//-------------------------------------------------------------------------

std::map<std::string, Bitmap*> Sprite::m_database;
std::map<Bitmap*, int> Sprite::m_counter;

//-------------------------------------------------------------------------

Sprite::Sprite(const char* filename):m_pos(0, 0), m_scale(1.f, 1.f), m_bitmap(NULL), m_angle(0.f), m_hotSpotPoint(0, 0), m_mirrorX(false), m_mirrorY(false), m_inDatabase(true)
{	
	std::string strFilename = filename;
	std::map<std::string,Bitmap*>::const_iterator it = m_database.find(strFilename);
	if (it == m_database.end())
	{
		m_bitmap = new Bitmap(filename);
		m_database[strFilename] = m_bitmap;
		m_counter[m_bitmap] = 1;
	}
	else
	{
		m_bitmap = (*it).second;
		m_counter[m_bitmap]++;
	}
}

//-------------------------------------------------------------------------

Sprite::Sprite(const Image& image):m_pos(0, 0), m_scale(1.f, 1.f), m_bitmap(new Bitmap(image, false)), m_hotSpotPoint(0, 0), m_angle(0.f), m_mirrorX(false), m_mirrorY(false), m_inDatabase(false)
{
}

//-------------------------------------------------------------------------

Int2 Sprite::getScaledSize() const
{
	//if (m_bitmap == NULL) return Int2(100,100) * m_scale;
	return Int2((int)(m_bitmap->size().width() * m_scale.x()), (int)(m_bitmap->size().height() * m_scale.y()));
}

//-------------------------------------------------------------------------

Int2 Sprite::getOriginalSize() const
{
	//if (m_bitmap == NULL) return Int2(100,100);
	return m_bitmap->size();
}

//-------------------------------------------------------------------------

void Sprite::draw() const
{
	//if (m_bitmap != NULL)
	if (m_angle == 0.f && !m_mirrorX && !m_mirrorY)
		m_bitmap->draw(m_pos - m_hotSpotPoint, 1.f, m_scale);
	else
		m_bitmap->drawRotated(m_pos, this->getScaledSize(), m_angle, m_hotSpotPoint, m_mirrorX, m_mirrorY, 1.f);
}

//-------------------------------------------------------------------------

void Sprite::draw(float opacity) const
{
	//if (m_bitmap != NULL)
	if (m_angle == 0.f && !m_mirrorX && !m_mirrorY)
		m_bitmap->draw(m_pos - m_hotSpotPoint, opacity, m_scale);
	else
		m_bitmap->drawRotated(m_pos, this->getScaledSize(), m_angle, m_hotSpotPoint, m_mirrorX, m_mirrorY, opacity);
}

//-------------------------------------------------------------------------

void Sprite::drawFragment(const Int2& topLeftInSprite, const Int2& bottomRightInSprite, const Int2& sizeOnTarget, float opacity) const
{
	m_bitmap->drawFragment(this->getPosition(), 
		(float)topLeftInSprite.x() / (float)this->getOriginalSize().width(), 
		(float)topLeftInSprite.y() / (float)this->getOriginalSize().height(), 
		(float)bottomRightInSprite.x() / (float)this->getOriginalSize().width(), 
		(float)bottomRightInSprite.y() / (float)this->getOriginalSize().height(), 
		sizeOnTarget.width(), 
		sizeOnTarget.height(), 
		opacity);
}

//-------------------------------------------------------------------------

Sprite::~Sprite()
{
	if (m_inDatabase)
	{
		if (m_bitmap != NULL)
		{
			m_counter[m_bitmap]--;
			if (m_counter[m_bitmap] == 0)
			{
				m_counter.erase(m_bitmap);
				iterateMapConst(m_database, std::string, Bitmap*)
				{
					if ((*it).second == m_bitmap)
					{
						//outputln("Unloading sprite (shared)");
						delete m_bitmap;
						m_database.erase((*it).first);
						break;
					}
				}
			}
		}
	}
	else
	{
		if (m_bitmap != NULL)
			delete m_bitmap;
	}
}

//---------------------------------------------------------------------
/**
    Change l'echelle du sprite pour qu'il ait la taille donnee
    Nb : la largeur ou la hauteur diminuera encore pour que l'image ne soit pas deformee
    \param rectSize Nouvelle largeur et hauteur de l'image
*/

void Sprite::setSizeDelimiters(const Int2& rectSize)
{
	float rescaleWidth = (float)rectSize.width()/(float)m_bitmap->size().width();
	float rescaleHeight = (float)rectSize.height()/(float)m_bitmap->size().height();

	if (rescaleWidth>rescaleHeight)
		m_scale = Float2(rescaleHeight, rescaleHeight);
	else
		m_scale = Float2(rescaleWidth, rescaleWidth);
}

//-------------------------------------------------------------------------

bool Sprite::isColliding(const Sprite& otherSprite) const
{
	Int2 thismin = m_pos-m_hotSpotPoint;
	Int2 thismax = m_pos-m_hotSpotPoint+this->getScaledSize();

	Int2 othermin = otherSprite.m_pos-otherSprite.m_hotSpotPoint;
	Int2 othermax = otherSprite.m_pos-otherSprite.m_hotSpotPoint+otherSprite.getScaledSize();

	return (othermin.y() < thismax.y() && othermax.y() > thismin.y() && othermin.x() < thismax.x() && othermax.x() > thismin.x());
}

//-------------------------------------------------------------------------
