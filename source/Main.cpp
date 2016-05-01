// Mabinogi Fantasia IJL Patch
// Created by spr33

// This source requires the Boost C++ libraries.
// http://www.boost.org/
// The default project configuration includes from
// the directory C:\Program Files\boost\boost_1_40_0\
// To change this, go to Project -> ijl15 Properties... ->
// Configuration Properties -> C/C++ -> General, and
// modify "Additional Include Directories".

#include "Main.h"

#include "Addr.h"
#include "FileSystem.h"
#include "IJLHook.h"
#include "patchers\MasterPatcher.h"

#define IJL_VERSION "R48 v3.1"

//-------------------------------------------------------------------
// Global variables

bool g_isRunning;
HINSTANCE g_hInstance;

//-------------------------------------------------------------------
// Functions

bool WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
	try
	{
		switch (fdwReason)
		{
		case DLL_PROCESS_ATTACH:
			{
				g_hInstance = hinstDLL;
				g_isRunning = true;

				g_iniFile = GetCurPath();
				g_iniFile += L"\\ijl15.ini";
				DeleteFile(L".\\ijl_patchlog.txt");

                WriteLog("Mabinogi Fantasia Patch for %s\n", IJL_VERSION);
				WriteLog("Created by spr33 for lemu.faucet\n");
                WriteLog("Initialization successful.\n");
				
				if(!SngIJLHook::Instance()->Hook()) return false;
				CAddr::PreCacheAddresses();
				CMasterPatcher::PatchFromINI();
				SngPatcher_HackShield::Instance()->ReadINI();
				WriteLog("Mabinogi Fantasia Patch successfully completed.\n");
				break;
			}

		case DLL_THREAD_ATTACH:break;
		case DLL_THREAD_DETACH:break;
		case DLL_PROCESS_DETACH:
			{
				WriteLog("Mabinogi Fantasia Patch unloaded.\n");
				break;
			}
		}
		return true;
	}
	catch(...)
	{
		WriteLog("Mabinogi Fantasia Patch initialization failed.");
	}
}