
#include <string>

#include "../../include/AbstractMainClass.h"
#include "../../include/Sprite.h"

class MainClass : public AbstractMainClass
{
public:
	MainClass(const std::string& arg0, const AppSetupInfos& appSetupInfos)
		:
		AbstractMainClass(arg0.c_str(), appSetupInfos),
		m_str("Press any key and click"),
		m_spritePositionAccurate(-100.0, -100.0) {
	}

	virtual ~MainClass() {}

	void init() {
		m_sprite = new Sprite("data/default_font.png");
	}

	void deinit() {
		delete m_sprite;
	}

	bool update() {
		m_spritePositionAccurate.data[0] +=
			Engine::instance().getFrameDuration() * 0.1;
		return true;
	}

	void render() {
		Engine::instance().clearScreen(CoreUtils::colorBlack);
		Engine::instance().getScene2DMgr().drawText(
			NULL, m_str.c_str(), Int2(10, 10), 18, CoreUtils::colorWhite);
		m_sprite->setPosition(CoreUtils::fromDouble2ToInt2(m_spritePositionAccurate));
		m_sprite->draw();
	}

	void onPointerPressed(MouseManager::ButtonType button, int x, int y) {
		m_spritePositionAccurate = CoreUtils::fromInt2ToDouble2(Int2(x, y));
	}

	void onKeyPressed(int virtualKey) {
		if (Engine::instance().getScene2DMgr().isDrawableCharacter((char)virtualKey)) {
			m_str = " ";
			m_str[0] = (char)virtualKey;
		}
	}

private:
	Sprite* m_sprite;
	Double2 m_spritePositionAccurate;
	std::string m_str;
};


AbstractMainClass* createAbstractMainClass(
	const std::vector<std::string>& arguments)
{
	AppSetupInfos appSetupInfos;
	appSetupInfos.windowSize = Int2(400, 300);
	appSetupInfos.defaultFontImgPath = "data/default_font.png";
	return new MainClass(arguments[0], appSetupInfos);
}
