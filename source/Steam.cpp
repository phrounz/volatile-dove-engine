// parts of this source file are Copyright 1996-2008, Valve Corporation, All rights reserved.

#ifdef USES_STEAM_INTEGRATION

#ifdef VERSION_X64
#pragma comment(lib,"steam_api64.lib")
#else
#pragma comment(lib,"steam_api.lib")
#endif

#ifdef VERSION_X64
#pragma comment(lib,"sdkencryptedappticket64.lib")
#else
#pragma comment(lib,"sdkencryptedappticket.lib")
#endif

#include "steam/steam_api.h"
#include "steam/isteamuserstats.h"
#include "steam/isteamremotestorage.h"
#include "steam/isteammatchmaking.h"
#include "steam/steam_gameserver.h"

#ifdef STEAM_CEG
// Steam DRM header file
#include "cegclient.h"
#else
#define Steamworks_InitCEGLibrary() (true)
#define Steamworks_TermCEGLibrary()
#define Steamworks_TestSecret()
#define Steamworks_SelfCheck()
#endif

//-----------------------------------------------------

#include "../include/Utils.h"

//-----------------------------------------------------

#include "Steam.h"

//-----------------------------------------------------

namespace Steam
{
	//----------------------------------------------------------------------------------------------------
	// achievements
	//----------------------------------------------------------------------------------------------------

	struct Achievement_t
	{
		int m_eAchievementID;
		const char* m_pchAchievementID;
		char m_rgchName[128];
		char m_rgchDescription[256];
		bool m_bAchieved;
		int m_iIconImage;
	};

	class CSteamAchievements
	{
	private:
		int64 m_iAppID; // Our current AppID
		Achievement_t *m_pAchievements; // Achievements data
		int m_iNumAchievements; // The number of Achievements
		bool m_bInitialized; // Have we called Request stats and received the callback?

	public:
		CSteamAchievements(Achievement_t *Achievements, int NumAchievements);
		~CSteamAchievements() {}

		bool RequestStats();
		bool SetAchievement(const char* ID);
		bool ClearAchievement(const char* ID);

		STEAM_CALLBACK(CSteamAchievements, OnUserStatsReceived, UserStatsReceived_t,     m_CallbackUserStatsReceived);
		STEAM_CALLBACK(CSteamAchievements, OnUserStatsStored,   UserStatsStored_t,       m_CallbackUserStatsStored);
		STEAM_CALLBACK(CSteamAchievements, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
	};

	CSteamAchievements::CSteamAchievements(Achievement_t *Achievements, int NumAchievements) :
		m_iAppID(0),
		m_bInitialized(false),
		m_CallbackUserStatsReceived(this, &CSteamAchievements::OnUserStatsReceived),
		m_CallbackUserStatsStored(this, &CSteamAchievements::OnUserStatsStored),
		m_CallbackAchievementStored(this, &CSteamAchievements::OnAchievementStored)
	{
		m_iAppID = SteamUtils()->GetAppID();
		m_pAchievements = Achievements;
		m_iNumAchievements = NumAchievements;
	}

	bool CSteamAchievements::RequestStats()
	{
		// Is Steam loaded? If not we can't get stats.
		if (NULL == SteamUserStats() || NULL == SteamUser())
		{
			return false;
		}
		// Is the user logged on?  If not we can't get stats.
		if (!SteamUser()->BLoggedOn())
		{
			return false;
		}
		// Request user stats.
		return SteamUserStats()->RequestCurrentStats();
	}

	bool CSteamAchievements::SetAchievement(const char* ID)
	{
		// Have we received a call back from Steam yet?
		if (m_bInitialized)
		{
			SteamUserStats()->SetAchievement(ID);
			return SteamUserStats()->StoreStats();
		}
		// If not then we can't set achievements yet
		return false;
	}

	bool CSteamAchievements::ClearAchievement(const char* ID)
	{
		// Have we received a call back from Steam yet?
		if (m_bInitialized)
		{
			SteamUserStats()->ClearAchievement(ID);
			return SteamUserStats()->StoreStats();
		}
		// If not then we can't set achievements yet
		return false;
	}

	void CSteamAchievements::OnUserStatsReceived(UserStatsReceived_t *pCallback)
	{
		// we may get callbacks for other games' stats arriving, ignore them
		if (m_iAppID == pCallback->m_nGameID)
		{
			if (k_EResultOK == pCallback->m_eResult)
			{
				Utils::print("Received stats and achievements from Steam\n");
				m_bInitialized = true;

				// load achievements
				for (int iAch = 0; iAch < m_iNumAchievements; ++iAch)
				{
					Achievement_t &ach = m_pAchievements[iAch];

					SteamUserStats()->GetAchievement(ach.m_pchAchievementID, &ach.m_bAchieved);
					_snprintf(ach.m_rgchName, sizeof(ach.m_rgchName), "%s", SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID, "name"));
					_snprintf(ach.m_rgchDescription, sizeof(ach.m_rgchDescription), "%s", SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID, "desc"));
				}
			}
			else
			{
				char buffer[128];
				_snprintf(buffer, 128, "RequestStats - failed, %d\n", pCallback->m_eResult);
				Utils::print(buffer);
				outputln("AccountId: " << pCallback->m_steamIDUser.GetAccountID());// << ";" << pCallback->m_steamIDUser.Render() );
			}
		}
	}

	void CSteamAchievements::OnUserStatsStored(UserStatsStored_t *pCallback)
	{
		// we may get callbacks for other games' stats arriving, ignore them
		if (m_iAppID == pCallback->m_nGameID)
		{
			if (k_EResultOK == pCallback->m_eResult)
			{
				Utils::print("Stored stats for Steam\n");
			}
			else
			{
				char buffer[128];
				_snprintf(buffer, 128, "StatsStored - failed, %d\n", pCallback->m_eResult);
				Utils::print(buffer);
			}
		}
	}

	void CSteamAchievements::OnAchievementStored(UserAchievementStored_t *pCallback)
	{
		// we may get callbacks for other games' stats arriving, ignore them
		if (m_iAppID == pCallback->m_nGameID)
		{
			Utils::print("Stored Achievement for Steam\n");
		}
	}

	// Achievement array which will hold data about the achievements and their state
	/*Achievement_t g_Achievements[] =
	{
		{ 0, "ACH_WIN_ONE_GAME", "Winner", "", 0, 0 },
		{ 1, "ACH_WIN_100_GAMES", "Champion", "", 0, 0 },
		{ 2, "ACH_TRAVEL_FAR_ACCUM", "Interstellar", "", 0, 0 },
		{ 3, "ACH_TRAVEL_FAR_SINGLE", "Orbiter", "", 0, 0 },
	};*/
	std::vector<AchievementInfo> g_AchievementInfosCopy;
	std::vector<Achievement_t> g_Achievements;

	// Global access to Achievements object
	CSteamAchievements*	g_SteamAchievements = NULL;

	//----------------------------------------------------------------------------------------------------
	// functions
	//----------------------------------------------------------------------------------------------------

	// Purpose: callback hook for debug text emitted from the Steam API
	static void SteamAPIDebugTextHook(int nSeverity, const char *pchDebugText)
	{
		// if you're running in the debugger, only warnings (nSeverity >= 1) will be sent
		// if you add -debug_steamapi to the command-line, a lot of extra informational messages will also be sent
#ifdef USES_LINUX
		printf(pchDebugText);
#else
		::OutputDebugStringA(pchDebugText);
#endif

		if (nSeverity >= 1)
		{
			// place to set a breakpoint for catching API errors
			int x = 3;
			x = x;
		}
	}

	void init(const std::vector<AchievementInfo>& achievementInfos) //const char *pchCmdLine, HINSTANCE hInstance, int nCmdShow)
	{
		g_AchievementInfosCopy = achievementInfos;
		for (int i = 0; i < achievementInfos.size(); ++i)
		{
			Achievement_t achievementData;
			achievementData.m_eAchievementID = i;
			achievementData.m_pchAchievementID = g_AchievementInfosCopy[i].id.c_str();
			strncpy(achievementData.m_rgchName, g_AchievementInfosCopy[i].name.c_str(), 128);
			strncpy(achievementData.m_rgchDescription, g_AchievementInfosCopy[i].description.c_str(), 128);
			achievementData.m_bAchieved = false;
			achievementData.m_iIconImage = 0;
			g_Achievements.push_back(achievementData);
		}
		
		if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
		{
			// if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the 
			// local Steam client and also launches this game again.

			// Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
			// removed steam_appid.txt from the game depot.

			AssertMessage(false, "SteamAPI_RestartAppIfNecessary failed");
		}

		// Init Steam CEG
		if (!Steamworks_InitCEGLibrary())
		{
			AssertMessage(false, "Steam must be running to play this game (InitDrmLibrary() failed).");
		}

		// Initialize SteamAPI, if this fails we bail out since we depend on Steam for lots of stuff.
		// You don't necessarily have to though if you write your code to check whether all the Steam
		// interfaces are NULL before using them and provide alternate paths when they are unavailable.
		//
		// This will also load the in-game steam overlay dll into your process.  That dll is normally
		// injected by steam when it launches games, but by calling this you cause it to always load,
		// even when not launched via steam.
		if (!SteamAPI_Init())
		{
			AssertMessage(false, "Steam must be running to play this game (SteamAPI_Init() failed).\n");
		}

		// Create the SteamAchievements object if Steam was successfully initialized
		g_SteamAchievements = new CSteamAchievements(&g_Achievements[0], g_Achievements.size());


		bool res = g_SteamAchievements->RequestStats();
		if (!res)
		{
			AssertMessage(false, "Could not get stats from Steam.\nPlease check that your Internet connection is working correctly and that Steam is up.\n");
		}


		// set our debug handler
		SteamClient()->SetWarningMessageHook(&SteamAPIDebugTextHook);

		// Tell Steam where it's overlay should show notification dialogs, this can be top right, top left,
		// bottom right, bottom left. The default position is the bottom left if you don't call this.  
		// Generally you should use the default and not call this as users will be most comfortable with 
		// the default position.  The API is provided in case the bottom right creates a serious conflict 
		// with important UI in your game.
		SteamUtils()->SetOverlayNotificationPosition(k_EPositionTopRight);

		// Ensure that the user has logged into Steam. This will always return true if the game is launched
		// from Steam, but if Steam is at the login prompt when you run your game from the debugger, it
		// will return false.
		if (!SteamUser()->BLoggedOn())
		{
			AssertMessage(false, "Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).\n");
		}

		// We are going to use the controller interface, initialize it, which is a seperate step as it 
		// create a new thread in the game proc and we don't want to force that on games that don't
		// have native Steam controller implementations

		std::string rgchCWD = Utils::getCurrentDirectory();
		
		char rgchFullPath[1024];
#if defined(_WIN32)
		_snprintf(rgchFullPath, sizeof(rgchFullPath), "%s\\%s", rgchCWD.c_str(), "controller.vdf");
#elif defined(OSX)
		// hack for now, because we do not have utility functions available for finding the resource path
		// alternatively we could disable the SteamController init on OS X
		_snprintf(rgchFullPath, sizeof(rgchFullPath), "%s/steamworksexample.app/Contents/Resources/%s", rgchCWD.c_str(), "controller.vdf");
#else
		_snprintf(rgchFullPath, sizeof(rgchFullPath), "%s/%s", rgchCWD.c_str(), "controller.vdf");
#endif
		if (!SteamController()->Init(rgchFullPath))
		{
			AssertMessage(false, "Steam Controller Init failed. Is controller.vdf in the current working directory?\n");
		}

		/*bool bUseVR = SteamUtils()->IsSteamRunningInVR();
		const char *pchServerAddress, *pchLobbyID;
		ParseCommandLine(pchCmdLine, &pchServerAddress, &pchLobbyID, &bUseVR);*/

		// do a DRM self check
		Steamworks_SelfCheck();

		// init VR before we make the window

		// Construct a new instance of the game engine 
		// bugbug jmccaskey - make screen resolution dynamic, maybe take it on command line?
		/*IGameEngine *pGameEngine =
#if defined(_WIN32)
			new CGameEngineWin32(hInstance, nCmdShow, 1024, 768, bUseVR);
#elif defined(OSX)
			CreateGameEngineOSX();
#elif defined(SDL)
			CreateGameEngineSDL(bUseVR);
#else
#error	Need CreateGameEngine()
#endif*/
	}

	void runStep()
	{
		// Run Steam client callbacks
		SteamAPI_RunCallbacks();
	}

	void unlockAchievement(const std::string& str)
	{
		//clearAchievement(str);// ENABLE FOR DEBUG
		if (g_SteamAchievements)
		{
			g_SteamAchievements->SetAchievement(str.c_str());
		}
	}

	void clearAchievement(const std::string& str)
	{
		if (g_SteamAchievements)
		{
			g_SteamAchievements->ClearAchievement(str.c_str());
		}
	}

	void deinit()
	{
		// Shutdown the SteamAPI
		SteamController()->Shutdown();
		SteamAPI_Shutdown();

		// Delete the SteamAchievements object
		if (g_SteamAchievements)
			delete g_SteamAchievements;

		// Shutdown Steam CEG
		Steamworks_TermCEGLibrary();
	}
}

#endif //USES_STEAM_INTEGRATION
