
#include <string>
#include <sstream>
#include <cmath>


namespace
{
	inline double round2(double x) { return (x-floor(x))>0.5 ? ceil(x) : floor(x); }

	const double RADIUS_MVT = 1224.0;
	const double DIST_SELECTOR_FROM_CENTER = 420.0;
	const int LIST_OF_DICES_IN_CIRCLE[] = { 5, 6, 1, 2, 6, 1, 4, 1, 5, 3, 2, 3, 2, 6, 4, 2 };
	enum DieColors { DC_RED=0, DC_BLUE=1, DC_GREEN=2, DC_BLACK=3 };
	const int LIST_OF_DIE_COLORS_IN_CIRCLE[] = {
		DC_RED,DC_RED,DC_BLACK,DC_GREEN,
		DC_GREEN,DC_RED,DC_BLACK,DC_RED,
		DC_BLACK,DC_RED,DC_BLUE,DC_BLACK,
		DC_BLACK,DC_BLUE,DC_BLACK,DC_GREEN };
	
	const int FLIP_COMBINATION_VALUES[] = { 1,2,3,2,5,6,4, 2,3,5,1,6,6};//1,
	const int FLIP_COMBINATION_COLORS[] = { DC_BLACK, DC_BLUE, DC_RED, DC_GREEN, DC_RED, DC_BLUE, DC_BLACK, DC_BLACK, DC_BLACK, DC_BLACK, DC_RED, DC_GREEN, DC_RED };//DC_GREEN,
	const int NB_DICES_TWIST = 13;

	enum ModeMvt { MODE_MVT_MANUAL=0, MODE_MVT_AUTO_CLOCKWISE=1, MODE_MVT_AUTO_COUNTERCLOCKWISE=2 };

	const int TIME_SELECTION = 10000;// in miliseconds

	const double SPEED_MVT = 0.002;
	const double SPEED_MVT_AUTO = 0.0005;

	const int NB_METALEVELS = 7;
}

class MainClass : public AbstractMainClass
{
public:
	MainClass(const std::string& arg0, const AppSetupInfos& appSetupInfos)
		:
		AbstractMainClass(arg0.c_str(), appSetupInfos), 
			m_level(0), m_transitionEnd(-1), m_visualAngle(0.0), m_angle(0.0), 
			m_metaLevel(-1), m_diceTwistLeft(-1), m_diceTwistRight(-1), 
			m_timeSelection(-1), m_timeEndSelection(-1), m_modeMvt(MODE_MVT_AUTO_COUNTERCLOCKWISE), 
			m_endTimeShowMode(-1), m_winTheGame(false), m_chosenDie(0), m_youForgotPermutation(false)
	{
	}

	virtual ~MainClass() {}

	void init()
	{
		m_circularDices = new Sprite("data/circular_dices.png");
		m_circularDices->setHotSpotPointCenter();

		m_selector = new Sprite("data/selector.png");
		m_selector->setHotSpotPointCenter();

		m_dicesInTower[0] = new Sprite("data/dice_tower/0.png");
		m_dicesInTower[1] = new Sprite("data/dice_tower/1.png");
		m_dicesInTower[2] = new Sprite("data/dice_tower/2.png");
		m_dicesInTower[3] = new Sprite("data/dice_tower/3.png");
		m_dicesInTower[4] = new Sprite("data/dice_tower/4.png");
		m_dicesInTower[5] = new Sprite("data/dice_tower/5.png");
		m_dicesInTower[6] = new Sprite("data/dice_tower/6.png");

		m_dicesTwist[0] = new Sprite("data/dice_twist/1_black.png");
		m_dicesTwist[1] = new Sprite("data/dice_twist/2_blue.png");
		m_dicesTwist[2] = new Sprite("data/dice_twist/3_red.png");
		m_dicesTwist[3] = new Sprite("data/dice_twist/2_green.png");
		m_dicesTwist[4] = new Sprite("data/dice_twist/5_red.png");
		m_dicesTwist[5] = new Sprite("data/dice_twist/6_blue.png");
		m_dicesTwist[6] = new Sprite("data/dice_twist/4_black.png");
		m_dicesTwist[7] = new Sprite("data/dice_twist/2_black.png");
		m_dicesTwist[8] = new Sprite("data/dice_twist/3_black.png");
		m_dicesTwist[9] = new Sprite("data/dice_twist/5_black.png");
		m_dicesTwist[10] = new Sprite("data/dice_twist/1_red.png");
		m_dicesTwist[11] = new Sprite("data/dice_twist/6_green.png");
		m_dicesTwist[12] = new Sprite("data/dice_twist/6_red.png");


		for (int i=0; i < NB_DICES_TWIST; ++i)
		{
			m_dicesTwist[i]->setScale(Float2(0.5f,0.5f));
			m_dicesTwist[i]->setHotSpotPointCenter();
		}
		m_flipArrows = new Sprite("data/dice_twist/flip.png");
		m_flipArrows->setScale(Float2(0.5f,0.5f));
		m_flipArrows->setHotSpotPointCenter();

		m_modeShow[0] = new Sprite("data/manual_mode.png");
		m_modeShow[1] = new Sprite("data/auto_clockwise_mode.png");
		m_modeShow[2] = new Sprite("data/auto_counterclockwise_mode.png");

		m_spriteWinTheGame = new Sprite("data/win_the_game.png");
		m_spriteWinTheGame->setHotSpotPointCenter();

		m_spriteForgotPermutation = new Sprite("data/forgot_permutation.png");
		m_spriteForgotPermutation->setHotSpotPointCenter();

		m_sounds[0] = Engine::instance().getSoundMgr().loadSound("data/sounds/0.wav");
		m_sounds[1] = Engine::instance().getSoundMgr().loadSound("data/sounds/1.wav");
		m_sounds[2] = Engine::instance().getSoundMgr().loadSound("data/sounds/2.wav");
		m_sounds[3] = Engine::instance().getSoundMgr().loadSound("data/sounds/3.wav");
		m_sounds[4] = Engine::instance().getSoundMgr().loadSound("data/sounds/4.wav");
		m_sounds[5] = Engine::instance().getSoundMgr().loadSound("data/sounds/5.wav");
		m_sounds[6] = Engine::instance().getSoundMgr().loadSound("data/sounds/6.wav");
		for (int i=0; i < NB_METALEVELS; ++i)
		{
			m_sounds[i]->stop();
			m_sounds[i]->setVolume(255);
		}

		m_soundSuccess = Engine::instance().getSoundMgr().loadSound("data/sounds/success.wav");
		m_soundFailure = Engine::instance().getSoundMgr().loadSound("data/sounds/failure.wav");
		m_soundNewLevel = Engine::instance().getSoundMgr().loadSound("data/sounds/new_level.wav");
		
		this->newMetaLevel(false);
		this->resetSelectionTime();
	}

	void newMetaLevel(bool playSound)
	{
		m_metaLevel++;
		m_level = 0;
		if (playSound)
		{
			m_soundNewLevel->play();
		}
		m_diceTwistLeft = Utils::random(6);
		m_diceTwistRight = Utils::random(6);
		ModeMvt prevModeMvt = m_modeMvt;
		while (prevModeMvt == m_modeMvt)
		{
			m_modeMvt = (ModeMvt)Utils::random(3);
		}
		m_endTimeShowMode = Utils::getMillisecond()+10000;
		this->changeDie();
		int angleRounded = (int)round2((double)m_angle*8.0/(double)MathUtils::PI);
		m_angle = (double)angleRounded*(double)MathUtils::PI/8.0;

		for (int i=0; i < NB_METALEVELS; ++i)
		{
			m_sounds[i]->stop();
			m_sounds[i]->setVolume(255);
		}
		m_sounds[m_metaLevel]->play();
	}

	void deinit()
	{
		delete m_circularDices;
		delete m_selector;
		for (int i=0; i < NB_METALEVELS; ++i)
		{
			delete m_dicesInTower[i];
		}
		for (int i=0; i < NB_DICES_TWIST; ++i)
		{
			delete m_dicesTwist[i];
		}
		delete m_flipArrows;
		for (int i=0; i < 3; ++i)
		{
			delete m_modeShow[i];
		}
		for (int i=0; i < NB_METALEVELS; ++i)
		{
			Engine::instance().getSoundMgr().removeSound(m_sounds[i]);
		}
		delete m_spriteWinTheGame;
		delete m_spriteForgotPermutation;
		Engine::instance().getSoundMgr().removeSound(m_soundSuccess);
		Engine::instance().getSoundMgr().removeSound(m_soundFailure);
		Engine::instance().getSoundMgr().removeSound(m_soundNewLevel);
	}

	bool update()
	{
		if (m_winTheGame)
		{
			return true;
		}
		if (!m_sounds[m_metaLevel]->isPlaying())
		{
			m_sounds[m_metaLevel]->play();
		}
		
		int64_t timeNow = Utils::getMillisecond();

		if (m_modeMvt == MODE_MVT_AUTO_CLOCKWISE || m_modeMvt == MODE_MVT_AUTO_COUNTERCLOCKWISE)
		{
			m_angle += (m_modeMvt == MODE_MVT_AUTO_COUNTERCLOCKWISE ? -1.0 : 1.0) 
				* (double)Engine::instance().getFrameDuration()*SPEED_MVT_AUTO*(1.0+0.1*(double)m_level+0.6*(double)m_metaLevel);
			m_visualAngle = m_angle;

			/*if (timeNow > m_timeChangeDie)
			{
				this->changeDie();
			}*/
		}
		else
		{
			if (timeNow < m_endTimeVisualAngle)
			{
				double visualAngleCorrected = m_visualAngle;
				if (abs(m_visualAngle-MathUtils::PI2 - m_angle) < abs(m_visualAngle - m_angle)) visualAngleCorrected = m_visualAngle-MathUtils::PI2;
				if (abs(m_visualAngle+MathUtils::PI2 - m_angle) < abs(m_visualAngle - m_angle)) visualAngleCorrected = m_visualAngle+MathUtils::PI2;
				m_visualAngle = visualAngleCorrected;
				
				int64_t prevTime = timeNow - Engine::instance().getFrameDuration();
				// previous: visualAngleCorrected @ prevTime
				// now: ?? @ timeNow
				// target: m_angle @ m_endTimeVisualAngle

				int64_t timeBetweenPreviousAndTarget = m_endTimeVisualAngle - prevTime;
				double diffBetweenPreviousAndTarget = m_angle - visualAngleCorrected;
				int64_t timeBetweenNowAndTarget = m_endTimeVisualAngle - timeNow;
				Assert(timeBetweenPreviousAndTarget > 0 && timeBetweenNowAndTarget > 0);

				double diffBetweenNowAndTarget = diffBetweenPreviousAndTarget * (double)timeBetweenNowAndTarget/(double)timeBetweenPreviousAndTarget;

				m_visualAngle = m_angle - diffBetweenNowAndTarget;
			}
			else
			{
				m_visualAngle = m_angle;
			}

			if (Utils::getMillisecond() > m_timeEndSelection)
			{
				this->validateChoice();
			}
		}

		this->keepAngleInBounds();

		if (m_visualAngle > MathUtils::PI2) m_visualAngle -= MathUtils::PI2;
		else if (m_visualAngle < 0) m_visualAngle += MathUtils::PI2;

		return true;
	}

	void render()
	{
		int halfSize = (int)(m_dicesTwist[0]->getScaledSize().width()*0.5);

		Engine::instance().clearScreen(CoreUtils::colorBlack);
		Int2 winSize =  Engine::instance().getScene2DMgr().getWindowSize();

		if (m_winTheGame)
		{
			m_spriteWinTheGame->setPosition(winSize*0.5f);
			m_spriteWinTheGame->draw();
			return;
		}
		
		//-------------
		// circular background (left)

		m_circularDices->setPosition(Int2(cos(m_visualAngle)*612+RADIUS_MVT/2, sin(m_visualAngle)*612+RADIUS_MVT/2));
		m_circularDices->draw();
		
		//-------------
		// selector

		m_selector->setPosition(Int2(-cos(m_angle)*DIST_SELECTOR_FROM_CENTER+RADIUS_MVT/2, -sin(m_visualAngle)*DIST_SELECTOR_FROM_CENTER+RADIUS_MVT/2));
		m_selector->draw();

		//-------------
		// chosen die indicator

		Engine::instance().getScene2DMgr().drawRectangle(Int2(RADIUS_MVT/2-halfSize-10,RADIUS_MVT/2-halfSize-10), Int2(RADIUS_MVT/2+halfSize+10,RADIUS_MVT/2+halfSize+10), CoreUtils::colorBlack, true);
		m_dicesTwist[m_chosenDie]->setPosition(Int2(RADIUS_MVT,RADIUS_MVT)*0.5);
		m_dicesTwist[m_chosenDie]->draw();

		//-------------
		// meta-level indicator

		m_dicesInTower[m_metaLevel]->setPosition(Int2(RADIUS_MVT,0));
		m_dicesInTower[m_metaLevel]->draw();

		Engine::instance().getScene2DMgr().drawRectangle(Int2(0,RADIUS_MVT), winSize+Int2(0,10), CoreUtils::colorBlack, true);// fix glitch

		//-------------
		// success or error

		int64_t timeNow = Utils::getMillisecond();
		if (timeNow < m_transitionEnd)
		{
			//set alpha channel
			int alpha = (timeNow < m_transitionEnd-256 ? 256-(m_transitionEnd-255-timeNow) : m_transitionEnd-timeNow);
			if (alpha > 255) alpha = 255;
			else if (alpha < 0) alpha = 0;
			m_transitionColor.data[3] = alpha;
			//Engine::instance().getScene2DMgr().drawRectangle(Int2(0,0), Int2(RADIUS_MVT,RADIUS_MVT), m_transitionColor, true);
			Engine::instance().getScene2DMgr().drawRectangle(Int2(RADIUS_MVT/2-halfSize,RADIUS_MVT/2-halfSize), Int2(RADIUS_MVT/2+halfSize,RADIUS_MVT/2+halfSize), m_transitionColor, true);

		}
		if (timeNow < m_transitionEnd + 768)
		{
			if (m_youForgotPermutation)
			{
				m_spriteForgotPermutation->setPosition(Int2(RADIUS_MVT/2,RADIUS_MVT/2));
				m_spriteForgotPermutation->draw();
			}
		}

		//-------------
		// level indicator

		for (int i = 0; i < m_level; ++i)
		{
			//Engine::instance().getScene2DMgr().drawRectangle(Int2(RADIUS_MVT+20,20+i*40), Int2(RADIUS_MVT+40,20+i*40+20), CoreUtils::colorBlack, true);
			Engine::instance().getScene2DMgr().drawRectangle(
				Int2(RADIUS_MVT/2+128+10+10,RADIUS_MVT/2+128+10-20-i*40), 
				Int2(RADIUS_MVT/2+128+10+30,RADIUS_MVT/2+128+10-i*40), 
				CoreUtils::colorBlack, 
				true);
		}

		//-------------
		// permutations

		if (m_diceTwistLeft != -1)
		{
			Assert(m_diceTwistRight != -1);
			m_dicesTwist[m_diceTwistLeft]->setPosition(Int2(20+RADIUS_MVT+halfSize,RADIUS_MVT-halfSize-20));
			m_dicesTwist[m_diceTwistLeft]->draw();
			m_flipArrows->setPosition(Int2(20+RADIUS_MVT+halfSize*3,RADIUS_MVT-halfSize));
			m_flipArrows->draw();
			m_dicesTwist[m_diceTwistRight]->setPosition(Int2(20+RADIUS_MVT+halfSize*5,RADIUS_MVT-halfSize-20));
			m_dicesTwist[m_diceTwistRight]->draw();
		}
		
		//-------------
		// time counter
		
		if (m_modeMvt == MODE_MVT_MANUAL)
		{
			Engine::instance().getScene2DMgr().drawRectangle(
				Int2(0,0), 
				Int2(RADIUS_MVT*(m_timeEndSelection-Utils::getMillisecond())/m_timeSelection,30),
				CoreUtils::colorGrey, true);
		}

		// vertical separation line
		Engine::instance().getScene2DMgr().drawRectangle(Int2(RADIUS_MVT-10,0), Int2(RADIUS_MVT+10, RADIUS_MVT), CoreUtils::colorBlack, true);
		// horizontal separation line
		Engine::instance().getScene2DMgr().drawRectangle(Int2(RADIUS_MVT-10,RADIUS_MVT-halfSize*2-60), Int2(winSize.width(), RADIUS_MVT-halfSize*2-40), CoreUtils::colorBlack, true);

		//-------------
		/*double angleInDegree = m_angle/MathUtils::PI_OVER_180;
		int index = (int)round2((angleInDegree-90)/22.5);
		if (index >= 16) index -= 16;else if (index < 0) index += 16;
		int currentDie = LIST_OF_DICES_IN_CIRCLE[index];

		std::stringstream sstr;
		sstr << "curdie:" << currentDie;
		Engine::instance().getScene2DMgr().drawText(
			NULL, sstr.str().c_str(), Int2(10, 10), 18, CoreUtils::colorWhite);
		//Utils::print((sstr.str() + "\n").c_str());*/
		//-------------

		if (true)//timeNow < m_endTimeShowMode)
		{
			Sprite* spriteToShow = NULL;
			switch(m_modeMvt)
			{
				case MODE_MVT_MANUAL:spriteToShow = m_modeShow[0];break;
				case MODE_MVT_AUTO_CLOCKWISE:spriteToShow = m_modeShow[1];break;
				case MODE_MVT_AUTO_COUNTERCLOCKWISE:spriteToShow = m_modeShow[2];break;
			}
			spriteToShow->setPosition(Int2(RADIUS_MVT,0));
			spriteToShow->draw();
			//spriteToShow->draw((double)(m_endTimeShowMode - timeNow) / 10000.0);
		}
	}

	void onPointerPressed(MouseManager::ButtonType button, int x, int y)
	{
		if (m_winTheGame)
		{
			return;
		}
		if (m_modeMvt == MODE_MVT_MANUAL)
		{
			if (button == MouseManager::MOUSE_LEFT_BUTTON)
			{
				m_angle -= MathUtils::PI/8.0;
				m_endTimeVisualAngle = Utils::getMillisecond() + 128;
			}
			else if (button == MouseManager::MOUSE_RIGHT_BUTTON)
			{
				m_angle += MathUtils::PI/8.0;
				m_endTimeVisualAngle = Utils::getMillisecond() + 128;
			}
			this->keepAngleInBounds();
		}
		else
		{
			this->validateChoice();
		}
	}

	void onKeyPressed(int virtualKey)
	{
		
	}

	void resetSelectionTime()
	{
		m_timeSelection = TIME_SELECTION/(1.0+0.1*(double)m_level+0.6*(double)m_metaLevel);
		m_timeEndSelection = Utils::getMillisecond()+m_timeSelection;
	}

	void validateChoice()
	{
		double angleInDegree = m_angle/MathUtils::PI_OVER_180;
		int index = (int)round2((angleInDegree-90)/22.5);
		if (index >= 16) index -= 16;else if (index < 0) index += 16;
		int currentDie = LIST_OF_DICES_IN_CIRCLE[index];
		int currentColor = LIST_OF_DIE_COLORS_IN_CIRCLE[index];

		this->resetSelectionTime();
		
		if (m_diceTwistLeft != -1)
		{
			if (currentDie == FLIP_COMBINATION_VALUES[m_diceTwistLeft] && currentColor == FLIP_COMBINATION_COLORS[m_diceTwistLeft])
			{
				currentDie = FLIP_COMBINATION_VALUES[m_diceTwistRight];
				currentColor = FLIP_COMBINATION_COLORS[m_diceTwistRight];
			}
			else if (currentDie == FLIP_COMBINATION_VALUES[m_diceTwistRight] && currentColor == FLIP_COMBINATION_COLORS[m_diceTwistRight])
			{
				currentDie = FLIP_COMBINATION_VALUES[m_diceTwistLeft];
				currentColor = FLIP_COMBINATION_COLORS[m_diceTwistLeft];
			}
		}

		m_youForgotPermutation = false;

		//if (currentDie == m_level+1) // good
		if (currentDie == FLIP_COMBINATION_VALUES[m_chosenDie] && currentColor == FLIP_COMBINATION_COLORS[m_chosenDie]) // good
		{
			m_transitionColor = Color(0,255,0,255);//green
			m_transitionEnd = Utils::getMillisecond()+512;
			if (m_level < 6)
			{
				m_level++;
				this->changeDie();
			}
			else // good (new meta-level)
			{
				if (m_metaLevel == NB_METALEVELS-1)
				{
					m_winTheGame = true;
				}
				else
				{
					this->newMetaLevel(true);
				}
			}
			m_soundSuccess->play();
		}
		else // bad
		{
			// if original die was good
			if (LIST_OF_DICES_IN_CIRCLE[index] == FLIP_COMBINATION_VALUES[m_chosenDie] && LIST_OF_DIE_COLORS_IN_CIRCLE[index] == FLIP_COMBINATION_COLORS[m_chosenDie])
			{
				m_youForgotPermutation = true;
			}
			m_transitionColor = Color(255,0,0,255);//red
			m_transitionEnd = Utils::getMillisecond()+512;
			m_level = 0;

			m_soundFailure->play();
		}
	}

	void keepAngleInBounds()
	{
		if (m_angle > MathUtils::PI2) m_angle -= MathUtils::PI2;
		else if (m_angle < 0) m_angle += MathUtils::PI2;
	}

	void changeDie()
	{
		int prevChosenDie = m_chosenDie;
		while (prevChosenDie == m_chosenDie)
		{
			m_chosenDie = Utils::random(NB_DICES_TWIST);
		}
		m_timeChangeDie = Utils::getMillisecond() + 10000;
	}
private:
	Sprite* m_circularDices;
	Sprite* m_selector;
	double m_angle;//radians
	Sprite* m_dicesInTower[NB_METALEVELS];

	int m_chosenDie;
	int64_t m_timeChangeDie;

	int m_level;
	int m_metaLevel;
	int64_t m_transitionEnd;
	Color m_transitionColor;

	Sprite* m_dicesTwist[NB_DICES_TWIST];
	Sprite* m_flipArrows;
	int m_diceTwistLeft;
	int m_diceTwistRight;

	int m_timeSelection;
	int m_timeEndSelection;

	ModeMvt m_modeMvt;
	double m_visualAngle;
	int m_endTimeVisualAngle;
	Sprite* m_modeShow[3];
	int m_endTimeShowMode;

	bool m_winTheGame;
	Sprite* m_spriteWinTheGame;

	bool m_youForgotPermutation;
	Sprite* m_spriteForgotPermutation;

	Sound* m_sounds[NB_METALEVELS];
	Sound* m_soundSuccess;
	Sound* m_soundFailure;
	Sound* m_soundNewLevel;
};


AbstractMainClass* createAbstractMainClass(
	const std::vector<std::string>& arguments)
{
	AppSetupInfos appSetupInfos;
	appSetupInfos.windowSize = Int2(2176/2, RADIUS_MVT/2);
	appSetupInfos.virtualSize = Int2(2176, RADIUS_MVT);
	appSetupInfos.automaticFitToWindowSize = true;
	appSetupInfos.isFullscreen = false;
	appSetupInfos.automaticFitToWindowSize;
	appSetupInfos.windowTitle = "The Mad Circle For The Glorious Dice Tower - A Ludum Dare 34 entry by Volatile Dove";
	return new MainClass(arguments[0], appSetupInfos);
}
