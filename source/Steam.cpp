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
	//-----------------------------------------------------------------------------
	// Purpose: callback hook for debug text emitted from the Steam API
	//-----------------------------------------------------------------------------
	static void SteamAPIDebugTextHook(int nSeverity, const char *pchDebugText)
	{
		// if you're running in the debugger, only warnings (nSeverity >= 1) will be sent
		// if you add -debug_steamapi to the command-line, a lot of extra informational messages will also be sent
		::OutputDebugStringA(pchDebugText);

		if (nSeverity >= 1)
		{
			// place to set a breakpoint for catching API errors
			int x = 3;
			x = x;
		}
	}

	void init() //const char *pchCmdLine, HINSTANCE hInstance, int nCmdShow)
	{
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
		// This call will block and run until the game exits
		//RunGameLoop(pGameEngine, pchServerAddress, pchLobbyID);



		// Run Steam client callbacks
		SteamAPI_RunCallbacks();

		// Update steam controller override mode appropriately
		/*if (bInMenuNow && !m_bLastControllerStateInMenu)
		{
			m_bLastControllerStateInMenu = true;
			SteamController()->SetOverrideMode("menu");
		}
		else if (!bInMenuNow && m_bLastControllerStateInMenu)
		{
			m_bLastControllerStateInMenu = false;
			SteamController()->SetOverrideMode("");
		}*/
	}

	void deinit()
	{
		// Shutdown the SteamAPI
		SteamController()->Shutdown();
		SteamAPI_Shutdown();

		// Shutdown Steam CEG
		Steamworks_TermCEGLibrary();
	}
}

#endif //USES_STEAM_INTEGRATION
