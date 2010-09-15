#include "patch.h"

bool CPatch::ReadINI_Bool(const wchar_t *key)
{
	uint patchOn = GetPrivateProfileInt(_T("PATCH"), key, 0, iniFile.c_str());
	if (patchOn==0) {
		DebugLog("%ls off\n", key);
		return false;
	}
	DebugLog("%ls on\n", key);
	return true;
}

int CPatch::ReadINI_Int(const wchar_t *key, uint min, uint max)
{
	uint patchSetting = GetPrivateProfileInt(_T("PATCH"), key, 0, iniFile.c_str());
	if ((patchSetting < min) || (patchSetting > max)) {
		DebugLog("%ls off\n", key);
		return 0;
	}
	DebugLog("%ls set to %u\n", key, patchSetting);
	return patchSetting;
}

int CPatch::ReadINI_Int(const wchar_t *key, uint min, uint max, uint def)
{
	uint patchSetting = GetPrivateProfileInt(_T("PATCH"), key, 0, iniFile.c_str());
	if ((patchSetting < min) || (patchSetting > max)) {
		DebugLog("%ls defaulting to %u\n", key, def);
		return def;
	}
	DebugLog("%ls set to %u\n", key, patchSetting);
	return patchSetting;
}

bool CPatch::Install(void)
{
	UseDataFolder();			//  Load data from Data folder before loading from pack files.
	BlockEndingAds();			//	Cancel the advertisements at the end of the game
	ClearDungeonFog();			//	Disable fog of war on dungeon minimaps
	//DisableCopyright();		//  Disable copyright display in screenshots
	DisableNighttime();			//	Increase the brightness of lights at night
	EnableFSAA();				//	Enable anti-aliasing
	EnableNameColoring();		//  Enable coloring of ALT names based on character type
	EnterRemoteShop();			//	Enable access to personal shops from a distance
	LogoutWhileTalking();		//  Allow logging out or exiting the game while talking to NPCs.
	ModifyZoomWidthLimit();		//	Increase the maximum zoom distance
	MoveToSameChannel();		//  Enable moving to the same channel you are currently on
	ScreenshotQuality();		//  Modify the quality of screenshots.
	ShowCombatPower();			//	Show combat power of all characters
	ShowItemPrice();			//	Show the shop purchase and selling price in item descriptions.
	ShowTrueDurability();		//	Show durability of items with 1000x precision
	ShowTrueFoodQuality();		//	Display food quality as a number
	//ShowTrueStats();			//  Show character stats with 1000x precision.
	TalkToUnequippedEgo();		//  Allow conversation with unequipped spirit weapons.
	TargetProps();				//  Enable CTRL-targeting props while in combat mode.
	UseBitmapFonts();			//  Use bitmap fonts instead of vector fonts to prevent window lag.
#ifdef DEV_RELEASE
	test();						//  ????
#endif
	return true;
}