#include "ModifyWindow.h"
#include "FileSystem.h"
#include "IJLHook.h"
#include "ThreadWatch.h"
#include "patchers/MasterPatcher.h"

//-------------------------------------------------------------------
// Local variables

static HWND mabiHWnd = NULL;
static WNDPROC wpPrevWndProc = NULL;
static HICON hIcon = NULL;
static HMENU fantasiaSubMenu = NULL;
static bool isMinimized = false;

//-------------------------------------------------------------------
// Functions

bool CModifyWindow::SpawnModifier( void )
{
	SECURITY_ATTRIBUTES ThreadAttributes;
	ThreadAttributes.bInheritHandle = false;
	ThreadAttributes.lpSecurityDescriptor = NULL;
	ThreadAttributes.nLength = sizeof( SECURITY_ATTRIBUTES );

	HANDLE hThr = CreateThread( &ThreadAttributes, 0, ModifierThreadMain, NULL, 0, NULL );
	if (hThr == NULL) {
		DebugLog("ERROR: CreateThread for watcher thread failed.\n");
		return false;
	}

	return true;
}

DWORD WINAPI CModifyWindow::ModifierThreadMain( LPVOID lpParam )
{
	while(!FindMabiHWnd()) {
		Sleep(1000);
	}
	DebugLog("Mabi window found.  HWND=0x%08lX\n", mabiHWnd);
	ModifyMenu();
	return 0;
}

bool CModifyWindow::FindMabiHWnd()
{
	HWND hwnd = NULL;

	set<DWORD> threadList = CThreadWatch::GetList();
	set<DWORD>::iterator it;
	for (it = threadList.begin(); it != threadList.end(); it++ )
	{
		if (!EnumThreadWindows( *it, (WNDENUMPROC)EnumThreadWindows_fn, (LPARAM)&hwnd ))
			break;
	}
	if (hwnd == NULL)
		return false;

	mabiHWnd = hwnd;
	return true;
}

BOOL CModifyWindow::EnumThreadWindows_fn(HWND hwnd, LPARAM lparam)
{
	wchar_t buf[255];
	GetClassName(hwnd, (LPWSTR)&buf, 255);
	if (wcscmp(buf, L"Mabinogi") == 0) {
		*((HWND *)lparam) = hwnd;
		return false; // Do not continue enumeration
	}
	return true; // Continue enumeration
}

bool CModifyWindow::ModifyMenu()
{
	HMENU systemMenu = GetSystemMenu( mabiHWnd, false );
	HMENU fantasiaMenu = LoadMenu( g_hInstance, MAKEINTRESOURCE(IDR_FANTASIA_MENU) );
	if (fantasiaMenu == NULL) {
		DebugLog("ERROR: Load Fantasia menu failed.\n");
		return false;
	}

	fantasiaSubMenu = GetSubMenu( fantasiaMenu, 0 );
	if (fantasiaSubMenu == NULL) {
		DebugLog("ERROR: Loading Fantasia submenu failed.\n");
		return false;
	}

	BOOL result = true;
	result &= InsertMenu( systemMenu, -1, MF_SEPARATOR, 0, 0 );
	result &= InsertMenu( systemMenu, -1, MF_BYPOSITION | MF_POPUP, (UINT_PTR)fantasiaSubMenu, L"Fantasia Patch" );
	if (!result) {
		DebugLog("ERROR: Inserting Fantasia submenu failed.\n");
		return false;
	}

	wpPrevWndProc = (WNDPROC)SetWindowLong( mabiHWnd, GWLP_WNDPROC, (LONG)FantasiaWndFunc );
	if (wpPrevWndProc == 0) {
		DebugLog("ERROR: Overwriting system window function failed.\n");
		return false;
	}

	DebugLog("Menu modification successful.\n");
	return true;
}

LRESULT APIENTRY CModifyWindow::FantasiaWndFunc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) != 0)
			isMinimized = true;
		else
			isMinimized = false;
		UpdateIcon();
		break;
	case WM_SYSCOMMAND:
		// Menu item selected
		switch (wParam)
		{
		case ID_SAVESETTINGS:
			CMasterPatcher::SaveToINI();
			break;
		case ID_CPUMENU_WHILEMINIMIZED:
			if (CThreadWatch::GetOptionOnlyMinimized())
				CThreadWatch::SetOptionOnlyMinimized(false);
			else
				CThreadWatch::SetOptionOnlyMinimized(true);
			UpdateIcon();
			break;
		case ID_CPUMENU_90:
			CThreadWatch::SetReductionPercentage( 90 );
			UpdateIcon();
			break;
		case ID_CPUMENU_80:
			CThreadWatch::SetReductionPercentage( 80 );
			UpdateIcon();
			break;
		case ID_CPUMENU_70:
			CThreadWatch::SetReductionPercentage( 70 );
			UpdateIcon();
			break;
		case ID_CPUMENU_60:
			CThreadWatch::SetReductionPercentage( 60 );
			UpdateIcon();
			break;
		case ID_CPUMENU_50:
			CThreadWatch::SetReductionPercentage( 50 );
			UpdateIcon();
			break;
		case ID_CPUMENU_40:
			CThreadWatch::SetReductionPercentage( 40 );
			UpdateIcon();
			break;
		case ID_CPUMENU_30:
			CThreadWatch::SetReductionPercentage( 30 );
			UpdateIcon();
			break;
		case ID_CPUMENU_20:
			CThreadWatch::SetReductionPercentage( 20 );
			UpdateIcon();
			break;
		case ID_CPUMENU_10:
			CThreadWatch::SetReductionPercentage( 10 );
			UpdateIcon();
			break;
		case ID_CPUMENU_OFF:
			CThreadWatch::SetReductionPercentage( 0 );
			UpdateIcon();
			break;
		case ID_TOGGLE_BLOCKENDINGADS:
			SngPatcher_BlockEndingAds::Instance()->Toggle();
			break;
		case ID_TOGGLE_CLEARDUNGEONFOG:
			SngPatcher_ClearDungeonFog::Instance()->Toggle();
			break;
		case ID_TOGGLE_DISABLENIGHTTIME:
			SngPatcher_DisableNighttime::Instance()->Toggle();
			break;
		case ID_TOGGLE_ENABLEFSAA:
			SngPatcher_EnableFSAA::Instance()->Toggle();
			break;
		case ID_TOGGLE_ENABLENAMECOLORING:
			SngPatcher_EnableNameColoring::Instance()->Toggle();
			break;
		case ID_TOGGLE_ENABLESELFRIGHTCLICK:
			SngPatcher_EnableSelfRightClick::Instance()->Toggle();
			break;
		case ID_TOGGLE_ENTERREMOTESHOP:
			SngPatcher_EnterRemoteShop::Instance()->Toggle();
			break;
		case ID_TOGGLE_LARGECLOCKTEXT:
			SngPatcher_LargeClockText::Instance()->Toggle();
			break;
		case ID_ZOOMLIMIT_5000:
			SngPatcher_ModifyZoomLimit::Instance()->SetLimit( 5000 );
			SngPatcher_ModifyZoomLimit::Instance()->Install();
			break;
		case ID_ZOOMLIMIT_3500:
			SngPatcher_ModifyZoomLimit::Instance()->SetLimit( 3500 );
			SngPatcher_ModifyZoomLimit::Instance()->Install();
			break;
		case ID_ZOOMLIMIT_1500:
			SngPatcher_ModifyZoomLimit::Instance()->SetLimit( 1500 );
			SngPatcher_ModifyZoomLimit::Instance()->Install();
			break;
		case ID_TOGGLE_MOVETOSAMECHANNEL:
			SngPatcher_MoveToSameChannel::Instance()->Toggle();
			break;
		case ID_TOGGLE_MOVEWHILETALKING:
			SngPatcher_MoveWhileTalking::Instance()->Toggle();
			break;
		case ID_TOGGLE_REMOVELOGINDELAY:
			SngPatcher_RemoveLoginDelay::Instance()->Toggle();
			break;
		case ID_SSQUALITY_100:
			SngIJLHook::Instance()->SetSSQuality(100);
			break;
		case ID_SSQUALITY_90:
			SngIJLHook::Instance()->SetSSQuality(90);
			break;
		case ID_SSQUALITY_75:
			SngIJLHook::Instance()->SetSSQuality(75);
			break;
		case ID_SSQUALITY_50:
			SngIJLHook::Instance()->SetSSQuality(50);
			break;
		case ID_TOGGLE_SHOWCOMBATPOWER:
			SngPatcher_ShowCombatPower::Instance()->Toggle();
			break;
		case ID_TOGGLE_SHOWEXPLORATIONPERCENT:
			SngPatcher_ShowExplorationPercent::Instance()->Toggle();
			break;
		case ID_TOGGLE_SHOWITEMPRICE:
			SngPatcher_ShowItemPrice::Instance()->Toggle();
			break;
		case ID_TOGGLE_SHOWTRUEDURABILITY:
			SngPatcher_ShowTrueDurability::Instance()->Toggle();
			break;
		case ID_TOGGLE_SHOWITEMCOLOR:
			SngPatcher_ShowTrueDurability::Instance()->ToggleColor();
			break;
		case ID_TOGGLE_SHOWTRUEFOODQUALITY:
			SngPatcher_ShowTrueFoodQuality::Instance()->Toggle();
			break;
		case ID_TOGGLE_TALKTOUNEQUIPPEDEGO:
			SngPatcher_TalkToUnequippedEgo::Instance()->Toggle();
			break;
		case ID_TOGGLE_TARGETPROPS:
			SngPatcher_TargetProps::Instance()->Toggle();
			break;
		case ID_TOGGLE_USEBITMAPFONTS:
			SngPatcher_UseBitmapFonts::Instance()->Toggle();
			break;
		case ID_TOGGLE_USEDATAFOLDER:
			SngPatcher_UseDataFolder::Instance()->Toggle();
			break;

		//case ID_TOGGLE_NAMEOFMOD:
		//	SngPatcher_NameOfMod::Instance()->Toggle();
		//	break;
		}
		break;
	case WM_INITMENU:
		// Update display of menu
		UpdateMenu();
		break;
	}

	return CallWindowProc( wpPrevWndProc, hwnd, uMsg, wParam, lParam );
}

bool CModifyWindow::UpdateMenu()
{
	bool result = true;
	MENUITEMINFO info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;

	// CPU Mod: Only when minimized
	info.fState = (CThreadWatch::GetOptionOnlyMinimized()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_CPUMENU_WHILEMINIMIZED, false, &info );

	// CPU Mod: Reduction percentage
	switch (CThreadWatch::GetReductionPercentage()) {
	case 90:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_CPUMENU_90, ID_CPUMENU_OFF, ID_CPUMENU_90, 0 );	break;
	case 80:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_CPUMENU_90, ID_CPUMENU_OFF, ID_CPUMENU_80, 0 );	break;
	case 70:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_CPUMENU_90, ID_CPUMENU_OFF, ID_CPUMENU_70, 0 );	break;
	case 60:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_CPUMENU_90, ID_CPUMENU_OFF, ID_CPUMENU_60, 0 );	break;
	case 50:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_CPUMENU_90, ID_CPUMENU_OFF, ID_CPUMENU_50, 0 );	break;
	case 40:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_CPUMENU_90, ID_CPUMENU_OFF, ID_CPUMENU_40, 0 );	break;
	case 30:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_CPUMENU_90, ID_CPUMENU_OFF, ID_CPUMENU_30, 0 );	break;
	case 20:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_CPUMENU_90, ID_CPUMENU_OFF, ID_CPUMENU_20, 0 );	break;
	case 10:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_CPUMENU_90, ID_CPUMENU_OFF, ID_CPUMENU_10, 0 );	break;
	case 0:		result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_CPUMENU_90, ID_CPUMENU_OFF, ID_CPUMENU_OFF, 0 );	break;
	}
	// Screenshot quality
	switch (SngIJLHook::Instance()->GetSSQuality()) {
	case 100:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_SSQUALITY_100, ID_SSQUALITY_50, ID_SSQUALITY_100, 0 );	break;
	case 90:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_SSQUALITY_100, ID_SSQUALITY_50, ID_SSQUALITY_90, 0 );	break;
	case 75:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_SSQUALITY_100, ID_SSQUALITY_50, ID_SSQUALITY_75, 0 );	break;
	case 50:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_SSQUALITY_100, ID_SSQUALITY_50, ID_SSQUALITY_50, 0 );	break;
	}
	// Zoom limit
	switch (SngPatcher_ModifyZoomLimit::Instance()->GetLimit()) {
	case 5000:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_ZOOMLIMIT_5000, ID_ZOOMLIMIT_1500, ID_ZOOMLIMIT_5000, 0 );	break;
	case 3500:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_ZOOMLIMIT_5000, ID_ZOOMLIMIT_1500, ID_ZOOMLIMIT_3500, 0 );	break;
	case 1500:	result &= 0 != CheckMenuRadioItem( fantasiaSubMenu, ID_ZOOMLIMIT_5000, ID_ZOOMLIMIT_1500, ID_ZOOMLIMIT_1500, 0 );	break;
	}
	// Block ending ads
	info.fState = (SngPatcher_BlockEndingAds::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_BLOCKENDINGADS, false, &info );
	// Clear dungeon fog
	info.fState = (SngPatcher_ClearDungeonFog::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_CLEARDUNGEONFOG, false, &info );
	// Disable nighttime
	info.fState = (SngPatcher_DisableNighttime::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_DISABLENIGHTTIME, false, &info );
	// Enable FSAA
	info.fState = (SngPatcher_EnableFSAA::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_ENABLEFSAA, false, &info );
	// Enable name coloring
	info.fState = (SngPatcher_EnableNameColoring::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_ENABLENAMECOLORING, false, &info );
	// Enable self right-click
	info.fState = (SngPatcher_EnableSelfRightClick::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_ENABLESELFRIGHTCLICK, false, &info );
	// Enter remote shop
	info.fState = (SngPatcher_EnterRemoteShop::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_ENTERREMOTESHOP, false, &info );
	// Large clock text
	info.fState = (SngPatcher_LargeClockText::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_LARGECLOCKTEXT, false, &info );
	// Move to same channel
	info.fState = (SngPatcher_MoveToSameChannel::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_MOVETOSAMECHANNEL, false, &info );
	// Move while talking
	info.fState = (SngPatcher_MoveWhileTalking::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_MOVEWHILETALKING, false, &info );
	// Remove login delay
	info.fState = (SngPatcher_RemoveLoginDelay::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_REMOVELOGINDELAY, false, &info );
	// Show combat power
	info.fState = (SngPatcher_ShowCombatPower::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_SHOWCOMBATPOWER, false, &info );
	// Show exploration percent
	info.fState = (SngPatcher_ShowExplorationPercent::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_SHOWEXPLORATIONPERCENT, false, &info );
	// Show item price
	info.fState = (SngPatcher_ShowItemPrice::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_SHOWITEMPRICE, false, &info );
	// Show true durability
	info.fState = (SngPatcher_ShowTrueDurability::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_SHOWTRUEDURABILITY, false, &info );
	// Show item color
	info.fState = (SngPatcher_ShowTrueDurability::Instance()->IsInstalled()) ? MFS_ENABLED : MFS_DISABLED;
	info.fState |= (SngPatcher_ShowTrueDurability::Instance()->IsColorEnabled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_SHOWITEMCOLOR, false, &info );
	// Show true food quality
	info.fState = (SngPatcher_ShowTrueFoodQuality::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_SHOWTRUEFOODQUALITY, false, &info );
	// Talk to unequipped ego
	info.fState = (SngPatcher_TalkToUnequippedEgo::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_TALKTOUNEQUIPPEDEGO, false, &info );
	// Target props
	info.fState = (SngPatcher_TargetProps::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_TARGETPROPS, false, &info );
	// Use bitmap fonts
	info.fState = (SngPatcher_UseBitmapFonts::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_USEBITMAPFONTS, false, &info );
	// Use data folder
	info.fState = (SngPatcher_UseDataFolder::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_USEDATAFOLDER, false, &info );

	// // NameOfMod
	// info.fState = (SngPatcher_NameOfMod::Instance()->IsInstalled()) ? MFS_CHECKED : MFS_UNCHECKED;
	// result &= 0 != SetMenuItemInfo( fantasiaSubMenu, ID_TOGGLE_NAMEOFMOD, false, &info );

	if (!result)
		DebugLog("ERROR: Update of system menu failed.\n");
	return result;
}

bool CModifyWindow::UpdateIcon( void )
{
	LPWSTR iconResource;
	if ((CThreadWatch::GetReductionPercentage() > 0) &&
		(!CThreadWatch::GetOptionOnlyMinimized() || isMinimized))
		iconResource = MAKEINTRESOURCE( IDI_ICON_BLUE );
	else iconResource = MAKEINTRESOURCE( IDI_ICON_ORANGE );

	if (hIcon)
		DeleteObject( hIcon );
	HICON icon = LoadIcon( g_hInstance, iconResource );
	if (icon == NULL) {
		DebugLog("ERROR: Loading icon resource 0x%08lX failed.\n", iconResource);
		return false;
	}
	SendMessage( mabiHWnd, WM_SETICON, ICON_SMALL, (LPARAM)icon );
	return true;	
}

bool CModifyWindow::IsMinimized( void )
{
	return isMinimized;
}