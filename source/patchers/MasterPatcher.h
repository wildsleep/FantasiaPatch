#pragma once

#include "../Main.h"
#include "Patcher_BlockEndingAds.h"
#include "Patcher_ClearDungeonFog.h"
#include "Patcher_DisableNighttime.h"
#include "Patcher_EnableFSAA.h"
#include "Patcher_EnableNameColoring.h"
#include "Patcher_EnableSelfRightClick.h"
#include "Patcher_EnterRemoteShop.h"
#include "Patcher_ExtraThreads.h"
#include "Patcher_HackShield.h"
#include "Patcher_LargeClockText.h"
#include "Patcher_ModifyZoomLimit.h"
#include "Patcher_MoveToSameChannel.h"
#include "Patcher_MoveWhileTalking.h"
#include "Patcher_RemoveLoginDelay.h"
#include "Patcher_ScreenshotQuality.h"
#include "Patcher_ShowCombatPower.h"
#include "Patcher_ShowExplorationPercent.h"
#include "Patcher_ShowItemPrice.h"
#include "Patcher_ShowTrueDurability.h"
#include "Patcher_ShowTrueFoodQuality.h"
#include "Patcher_TalkToUnequippedEgo.h"
#include "Patcher_TargetProps.h"
#include "Patcher_UseBitmapFonts.h"
#include "Patcher_UseDataFolder.h"

class CMasterPatcher {
public:
	static bool PatchFromINI( void );
	static bool SaveToINI( void );
private:
	static vector<IGenericPatcher *> PatchList;
	static bool GetPatchList( void );
};