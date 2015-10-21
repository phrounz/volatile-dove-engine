#ifndef EnemyBullet_h_INCLUDED
#define EnemyBullet_h_INCLUDED

#include "include/AbstractMainClass.h"
#include "include/Sprite.h"
#include "include/Image.h"

#include "Autodestroyable.h"

//---------------------------------------------------------------------

class EnemyBullet: public Autodestroyable
{
public:

	EnemyBullet(const Double2& pos, const Double2& speed);
	void manage();
	bool isColliding(const Image& collisionImage, const Int2& collisionImagePos) const;
	Int2 getPosition() const { return CoreUtils::fromDouble2ToInt2(m_pos); }
	void draw() const;
	void destroy() { this->autodestroy(); }
	~EnemyBullet();

private:
	Sprite* m_sprite;
	Double2 m_pos;
	Double2 m_speed;
};

//---------------------------------------------------------------------

#endif //EnemyBullet_h_INCLUDED
