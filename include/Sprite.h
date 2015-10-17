#ifndef Sprite_h_INCLUDED
#define Sprite_h_INCLUDED

#include <map>
#include <string>

#include "CoreUtils.h"

class Image;
class Bitmap;

//---------------------------------------------------------------------
/**
* \brief A 2D <a href="https://en.wikipedia.org/wiki/Sprite_%28computer_graphics%29">sprite</a>
*
* Note: uses a database of commonly shared Sprites so not totally thread-safe.
*/

class Sprite
{
public:
	Sprite(const char* filename);
	Sprite(const Image& image);
	void draw() const;
	void draw(float opacity) const;//between 0.f and 1.f
	void drawFragment(const Int2& topLeftInSprite, const Int2& bottomRightInSprite, const Int2& sizeOnTarget, float opacity) const;
	void setPosition(const Int2& newPos) { m_pos = newPos; }
	void setScale(const Float2& newScale) { m_scale = newScale; }
	void setAngle(float angle) { m_angle = angle; }
	void setHotSpotPoint(const Int2& pos) { m_hotSpotPoint = pos; }
	void setHotSpotPointCenter() { m_hotSpotPoint = this->getScaledSize() * 0.5f; }

	Int2 getScaledSize() const;
	Int2 getOriginalSize() const;
	inline const Int2& getPosition() const { return m_pos; }
	inline const float getAngle() const { return m_angle; }
	inline const Int2& getHotSpotPoint() const { return m_hotSpotPoint; }
	inline const Float2& getScale() const { return m_scale; }
	~Sprite();
	//void updateImage();
	void setSizeDelimiters(const Int2& rectSize);
	// no rotations considerations:
	inline Int2 getMinPos() const { return m_pos-m_hotSpotPoint;}
	inline Int2 getMaxPos() const { return m_pos-m_hotSpotPoint+this->getScaledSize();}
	inline CoreUtils::RectangleInt getCollisionRectangle() const { return CoreUtils::RectangleInt(getMinPos(), getMaxPos()); }
	inline bool isIn(const Int2& posToTest) const {return posToTest >= this->getMinPos() && posToTest < this->getMaxPos();}
	bool isColliding(const Sprite& otherSprite) const;// do not check alpha value
	float distanceTo(const Sprite& otherSprite) const { return otherSprite.getPosition().distanceTo(this->getPosition()); }
	void setMirrorX(bool yesNo) { m_mirrorX = yesNo; }
	void setMirrorY(bool yesNo) { m_mirrorY = yesNo; }
	bool isMirrorX() const { return m_mirrorX; }
	bool isMirrorY() const { return m_mirrorY; }
private:
	Int2 m_pos;
	Float2 m_scale;
	Bitmap* m_bitmap;
	float m_angle;// in degree
	Int2 m_hotSpotPoint;
	static std::map<std::string, Bitmap*> m_database;
	static std::map<Bitmap*, int> m_counter;
	bool m_mirrorX;
	bool m_mirrorY;
	bool m_inDatabase;
};

//---------------------------------------------------------------------

#endif //Sprite_h_INCLUDED
