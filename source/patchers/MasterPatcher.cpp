#include "MasterPatcher.h"

vector<IGenericPatcher *> CMasterPatcher::PatchList;

bool CMasterPatcher::PatchFromINI( void )
{
	DebugLog("Reading settings from INI...\n");
	GetPatchList();
	vector<IGenericPatcher *>::iterator it;
	for (it = PatchList.begin(); it != PatchList.end(); it++) {
		(*it)->ReadINI();
	}
	DebugLog("Settings loaded!\n");
	return true;
}

bool CMasterPatcher::SaveToINI( void )
{
	DebugLog("Saving settings to INI...\n");
	GetPatchList();
	vector<IGenericPatcher *>::iterator it;
	for (it = PatchList.begin(); it != PatchList.end(); it++) {
		(*it)->WriteINI();
	}
	DebugLog("Settings saved!\n");
	return true;
}

bool CMasterPatcher::GetPatchList( void )
{
	PatchList.clear();
	PatchList +=
		SngPatcher_BlockEndingAds::Instance(),
		SngPatcher_ClearDungeonFog::Instance(),
		SngPatcher_DisableNighttime::Instance(),
		SngPatcher_EnableFSAA::Instance(),
		SngPatcher_EnableNameColoring::Instance(),
		SngPatcher_EnableSelfRightClick::Instance(),
		SngPatcher_EnterRemoteShop::Instance(),
		SngPatcher_LargeClockText::Instance(),
		SngPatcher_ModifyZoomLimit::Instance(),
		SngPatcher_MoveToSameChannel::Instance(),
		SngPatcher_MoveWhileTalking::Instance(),
		SngPatcher_RemoveLoginDelay::Instance(),
		SngPatcher_ScreenshotQuality::Instance(),
		SngPatcher_ShowCombatPower::Instance(),
		SngPatcher_ShowExplorationPercent::Instance(),
		SngPatcher_ShowItemPrice::Instance(),
		SngPatcher_ShowTrueDurability::Instance(),
		SngPatcher_ShowTrueFoodQuality::Instance(),
		SngPatcher_TalkToUnequippedEgo::Instance(),
		SngPatcher_TargetProps::Instance(),
		SngPatcher_UseBitmapFonts::Instance(),
		SngPatcher_UseDataFolder::Instance(),
		SngPatcher_ExtraThreads::Instance();
	return true;	
}