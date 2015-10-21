
#include <sstream>
#include <list>

#include "include/AbstractMainClass.h"
#include "include/Bitmap.h"
#include "include/Sprite.h"

#include "Chara.h"
#include "Cannon.h"
#include "Enemy.h"
#include "Cloud.h"
#include "Burning.h"
#include "WaterSystem.h"
#include "Swarm.h"
#include "EngineSystem.h"

//---------------------------------------------------------------------

namespace
{
	const int WINDOW_WIDTH = 1600;
	const int WINDOW_HEIGHT = 768;

	const int INIT_REAL_WINDOW_WIDTH = 800;
	const int INIT_REAL_WINDOW_HEIGHT = 384;

	const bool IS_FULLSCREEN = false;

	const bool AUTOMATIC_FIT_TO_WINDOW_SIZE = true;

	const std::string WINDOW_TITLE_DESKTOP_APP = "An Unconventional Weapon - Big Bertha vs alien robots!";
	const std::wstring APP_DATA_FOLDER_BASENAME = L"AnUnconventionalWeapon";
	const std::string IMG_FONT_PATH = "data/default_font.png";
}

//---------------------------------------------------------------------

class MainClass: public AbstractMainClass
{
public:
	MainClass(const std::vector<std::string>& arguments);
	virtual ~MainClass() {}

	std::vector<std::pair<std::string, std::string> > getCharmSettings() { return std::vector<std::pair<std::string, std::string> >(); }

	void init();
	void realInit();
	void deinit();

	bool update();
	void render();

	void onPointerPressed(MouseManager::ButtonType button, int x, int y);
	void onPointerReleased(MouseManager::ButtonType button, int x, int y);
	void onPointerWheelChanged(int wheelDelta, int x, int y) {}
	void onPointerMoved(bool isPressed, int x, int y) {}
	void onKeyPressed(Key virtualKey);
	void onKeyReleased(Key virtualKey) {}
	void onWindowResize() {}
	void onSuspending() {}
	void onEdgeSettingsButton(const char* idStr) {}
	void onSemanticZoom(float value) {}
	bool getSemanticZoomIsEnabled() { return false; }
private:
	EngineSystem* m_engineSystem;
	Sprite* m_background;
	Image* m_collisionImage;
	Bitmap* m_structure;
	Bitmap* m_structureForeground;
	std::list<Cloud*> m_clouds;
	Chara* m_chara;
	Cannon* m_cannon;
	Swarm* m_swarm;
	Burning* m_burning;
	WaterSystem* m_waterSystem;
	Bitmap* m_gameOver;
	Sprite* m_help;
	bool m_isHelpOpened;
	Sprite* m_pressF1Help;
	Sound* m_music;
	unsigned char m_musicVolume;
	bool m_musicEnabled;
};

//---------------------------------------------------------------------

AppSetupInfos getInitInfos()
{
	AppSetupInfos initInfos;
	initInfos.windowTitle = WINDOW_TITLE_DESKTOP_APP;
	initInfos.windowSize = Int2(AUTOMATIC_FIT_TO_WINDOW_SIZE ? INIT_REAL_WINDOW_WIDTH : WINDOW_WIDTH, AUTOMATIC_FIT_TO_WINDOW_SIZE ? INIT_REAL_WINDOW_HEIGHT : WINDOW_HEIGHT);
	initInfos.isFullscreen = IS_FULLSCREEN;
	initInfos.defaultFontImgPath = IMG_FONT_PATH;
	initInfos.appDataFolderBasename = APP_DATA_FOLDER_BASENAME;
	initInfos.automaticFitToWindowSize = AUTOMATIC_FIT_TO_WINDOW_SIZE;
	initInfos.virtualSize = Int2(WINDOW_WIDTH, WINDOW_HEIGHT);
	initInfos.virtualSizeAllowRatioDeformation = false;
	return initInfos;
}

//---------------------------------------------------------------------

MainClass::MainClass(const std::vector<std::string>& arguments)
:
	AbstractMainClass(arguments[0].c_str(), getInitInfos()), m_musicEnabled(true), m_musicVolume(100)
{
	
}

void MainClass::init()
{
	Scene2D& scene2D = Engine::instance().getScene2DMgr();

	Int2 winSize = scene2D.getWindowSize();

	Engine::instance().startRender();
	//Engine::instance().clearScreen(Color(57,155,230, 255));
	Engine::instance().getScene2DMgr().drawRectangle(Int2(0,0), winSize, Color(57,155,230, 255), true);//clearScreen(Color(57,155,230, 255));
	Bitmap* loading = new Bitmap("data/loading.png");
	Int2 bitmapPos = winSize*0.5f - loading->size()*0.5f;
	loading->draw(winSize*0.5f-loading->size()*0.5f, 1.f, Float2(1.f, 1.f));
#if defined(USES_WINDOWS8_METRO)
	Engine::instance().getScene2DMgr().drawRectangle(bitmapPos + Int2(362, 411), bitmapPos + Int2(463, 440), CoreUtils::colorWhite, true);// hide "Esc: quit"
#endif
	delete loading;
	Engine::instance().endRender();

	//-----

	m_isHelpOpened = true;

	m_help = new Sprite("data/help.png");
	m_help->setPosition(winSize*0.5f-m_help->getScaledSize()*0.5f);

	m_pressF1Help = new Sprite("data/pressf1help.png");
	m_pressF1Help->setPosition(Int2(10, winSize.height()-30));

	m_background = new Sprite("data/background.png");
	m_background->setPosition(Int2(0,0));

	m_collisionImage = new Image("data/collision.png");
	m_structure = new Bitmap("data/structure.png");

	m_structureForeground = new Bitmap("data/structure_foreground.png");

	m_engineSystem = new EngineSystem;

	m_chara = new Chara();

	m_cannon = new Cannon();

	m_clouds.push_back(new Cloud);
	m_clouds.push_back(new Cloud);
	m_clouds.push_back(new Cloud);
	m_clouds.push_back(new Cloud);

	m_burning = new Burning;
	m_waterSystem = new WaterSystem;

	m_swarm = new Swarm;

	m_gameOver = new Bitmap("data/game_over.png");
	
#if defined(USES_WINDOWS_OPENGL) || defined(USES_LINUX)
	m_music = Engine::instance().getSoundMgr().loadSound("data/sounds/opengameart_cynicmusic_battleThemeA.ogg");
#else
	m_music = Engine::instance().getSoundMgr().loadSound("data/sounds/opengameart_cynicmusic_battleThemeA.wav");
#endif
	m_music->setVolume(m_musicVolume);
	m_music->play();
}

bool MainClass::update()
{
	if (m_isHelpOpened && (
		Engine::instance().getJoystickMgr().isPressed(JoystickManager::PLAYER_ONE, JoystickManager::BUTTON_X) || 
		Engine::instance().getJoystickMgr().isPressed(JoystickManager::PLAYER_ONE, JoystickManager::BUTTON_START) || 
		Engine::instance().getJoystickMgr().isPressed(JoystickManager::PLAYER_TWO, JoystickManager::BUTTON_X) ||
		Engine::instance().getJoystickMgr().isPressed(JoystickManager::PLAYER_TWO, JoystickManager::BUTTON_START)))
	{
		m_isHelpOpened = false;
	}

	iterateList(m_clouds, Cloud*)
	{
		(*it)->manage();
	}

	if (!m_burning->isGameOver() && !m_isHelpOpened)
	{
		m_chara->manage(m_collisionImage, m_waterSystem);
		
		m_cannon->manage(m_swarm, *m_engineSystem, m_burning->hasCannonDamage());
	
		m_burning->manage(m_cannon->getCollisionPos());
		
		m_engineSystem->manage(m_burning);

		std::list<WaterDrop*>& waterDrops = m_waterSystem->manage();
		iterateList(waterDrops, WaterDrop*)
		{
			m_burning->checkCollisions(*it);
		}
		iterateList(waterDrops, WaterDrop*)
		{
			m_swarm->checkCollisions(*it);
		}
		iterateList(waterDrops, WaterDrop*)
		{
			m_cannon->checkCollisions(*it);
		}
		
		m_swarm->manage();
		m_swarm->checkCollisions(*m_collisionImage, m_burning);
		m_swarm->checkCollisions(m_cannon, m_burning);

		m_cannon->updateBonuses(m_chara->popBonuses());
	}

	if (!m_music->isPlaying() && m_musicEnabled) m_music->play();
	if (!m_musicEnabled && m_music->isPlaying()) m_music->stop();

	return true;
}

void MainClass::onKeyPressed(Key key)
{
	if (key==KeyboardManager::KEY_F1 && !m_isHelpOpened)
	{
		m_isHelpOpened = true;
	}
	else if (key==KeyboardManager::KEY_ENTER && m_isHelpOpened)
	{
		m_isHelpOpened = false;
	}
	else if (key==KeyboardManager::KEY_F2)
	{
		this->deinit();
		Engine::instance().resetSoundSystem();
		this->init();
	}
	else if (key==KeyboardManager::KEY_F3)
	{
		m_musicEnabled = !m_musicEnabled;
	}
	else if (key==KeyboardManager::KEY_F4 && m_musicVolume > 30)
	{
		m_musicVolume = m_musicVolume-30;
		m_music->setVolume(m_musicVolume);
	}
	else if (key==KeyboardManager::KEY_F5 && m_musicVolume < 255-30)
	{
		m_musicVolume = m_musicVolume+30;
		m_music->setVolume(m_musicVolume);
	}
	else if (key==KeyboardManager::KEY_ESCAPE)
	{
		Engine::instance().stopRunning();
	}
}

void MainClass::onPointerPressed(MouseManager::ButtonType button, int x, int y)
{
}

void MainClass::onPointerReleased(MouseManager::ButtonType button, int x, int y)
{
}

void MainClass::render()
{
	Int2 winSize = Engine::instance().getScene2DMgr().getWindowSize();

	Engine::instance().clearScreen(CoreUtils::colorBlack);

	m_background->draw();
	iterateList(m_clouds, Cloud*)
	{
		(*it)->draw();
	}
	m_structure->draw(Int2(0,0), 1.f, Float2(1.f, 1.f));
	m_cannon->draw();
	m_engineSystem->draw();

	m_chara->draw();

	m_burning->draw();
	m_waterSystem->draw();

	m_structureForeground->draw(Int2(0,0), 1.f, Float2(1.f, 1.f));

	m_swarm->draw();

	m_cannon->drawBonusesCounter();

	if (m_burning->isGameOver())
	{
		Int2 centerScreen = winSize*0.5f;
		m_gameOver->draw(centerScreen-m_gameOver->size()*0.5f, 1.f, Float2(1.f, 1.f));
		std::stringstream sstr;
		sstr << "Level reached: " << m_swarm->getLevel() << "\n";
		sstr << m_swarm->getNbEnemiesExploded() << " alien robots exploded by the big bertha\n";
		sstr << m_swarm->getNbEnemiesSunk() << " alien robots sunk by the hosepipe\n";
		sstr << m_swarm->getNbEnemiesExplodedAndSunk() << " alien robots sunk and then exploded\n";
		Engine::instance().getScene2DMgr().drawText(NULL, sstr.str().c_str(),centerScreen+Int2(-m_gameOver->size().width()*0.5f+30, -10), 16, CoreUtils::colorBlack);
	}

	m_pressF1Help->draw();
	std::stringstream sstr;
	sstr << m_swarm->getLevel();// << " - FRAME DURATION: " << Engine::instance().getFrameDuration();
	Engine::instance().getScene2DMgr().drawText(NULL, sstr.str().c_str(), m_pressF1Help->getPosition()+Int2(m_pressF1Help->getScaledSize().width()+2, 2), 25, CoreUtils::colorBlack);

	if (m_isHelpOpened)
	{
		m_help->draw();
#if defined(USES_WINDOWS8_METRO)
		Engine::instance().getScene2DMgr().drawRectangle(m_help->getPosition() + Int2(362, 411), m_help->getPosition() + Int2(463, 440), CoreUtils::colorWhite, true);// hide "Esc: quit"
#endif
	}
}

void MainClass::deinit()
{
	delete m_chara;
	delete m_cannon;
	delete m_background;
	delete m_structure;
	delete m_collisionImage;
	delete m_swarm;
	delete m_burning;
	delete m_waterSystem;
	delete m_structureForeground;
	delete m_engineSystem;
	delete m_gameOver;
	delete m_help;
	delete m_pressF1Help;
	iterateList(m_clouds, Cloud*)
	{
		delete (*it);
	}
	m_clouds.clear();
	Engine::instance().getSoundMgr().removeSound(m_music);
}

//---------------------------------------------------------------------

AbstractMainClass* createAbstractMainClass(const std::vector<std::string>& arguments) { return new MainClass(arguments); }

//---------------------------------------------------------------------
