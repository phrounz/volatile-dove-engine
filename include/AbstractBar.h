#ifndef AbstractBar_h_INCLUDED
#define AbstractBar_h_INCLUDED

#include "CoreUtils.h"
#include "Sprite.h"

//---------------------------------------------------------------------------------------------

class AbstractBar
{
public:

	AbstractBar(const char* spriteFile, int initialValue, const Int2& posStart, bool useDefaultSpacing, const Int2& spacingBetweenSprites)
		:m_initialValue(initialValue), m_posStart(posStart), m_spacingBetweenSprites(spacingBetweenSprites)
	{
		reset();
		m_sprite = new Sprite(spriteFile);
		if (useDefaultSpacing) m_spacingBetweenSprites = Int2(m_sprite->getScaledSize().width(), 0);
	}

	void reset() { m_value = m_initialValue; }

	void draw() const
	{
		Int2 pos = m_posStart;
		for (int i = 0; i < m_value; ++i)
		{
			m_sprite->setPosition(pos);
			m_sprite->draw();
			pos += m_spacingBetweenSprites;
		}
	}

	~AbstractBar()
	{
		delete m_sprite;
	}

	void set(int lifes) { m_value = lifes; }
	int get() const { return m_value; }

private:
	Int2 m_posStart;
	int m_value;
	int m_initialValue;
	Int2 m_spacingBetweenSprites;
	Sprite* m_sprite;
};

//---------------------------------------------------------------------------------------------

#endif //AbstractBar_h_INCLUDED
