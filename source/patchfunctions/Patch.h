#pragma once

#include "..\main.h"

extern wstring iniFile;
extern DWORD ssQuality;

class CPatch
{
public:
	static bool Install(void);

private:
	static bool ReadINI_Bool(const wchar_t *key);
	static int ReadINI_Int(const wchar_t *key, uint min, uint max);
	static int ReadINI_Int(const wchar_t *key, uint min, uint max, uint def);

	static bool UseDataFolder(void);
	static bool BlockEndingAds(void);
	static bool ClearDungeonFog(void);
	// static bool DisableCopyright(void);
	static bool DisableNighttime(void);
	static bool EnableFSAA(void);
	static bool EnableNameColoring(void);
	static bool EnterRemoteShop(void);
	static bool LogoutWhileTalking(void);
	static bool ModifyZoomWidthLimit(void);
	static bool MoveToSameChannel(void);
	static bool ScreenshotQuality(void);
	static bool ShowCombatPower(void);
	static bool ShowItemPrice(void);
	static bool ShowTrueDurability(void);
	static bool ShowTrueFoodQuality(void);
	// static bool ShowTrueStats(void);
	static bool TalkToUnequippedEgo(void);
	static bool TargetProps(void);
	static bool UseBitmapFonts(void);
#ifdef DEV_RELEASE
	static bool test(void);
#endif
};