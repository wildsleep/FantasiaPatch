#include "Patcher_ExtraThreads.h"
#include "../Addr.h"
#include "../FileSystem.h"
#include "../IJLHook.h"
#include "../ModifyWindow.h"
#include "../ThreadWatch.h"

//-----------------------------------------------------------------------------
// Constructor

CPatcher_ExtraThreads::CPatcher_ExtraThreads( void )
{
	patchname = "Extra Threads";
}

//-----------------------------------------------------------------------------
// Overriden Patching Functions

bool CPatcher_ExtraThreads::Install( void ) {
	bool result = true;
	result &= CThreadWatch::SpawnWatcher();
	result &= CModifyWindow::SpawnModifier();
	installed = result;
	return result;
}

bool CPatcher_ExtraThreads::Uninstall( void ) {
	// This mod cannot be uninstalled once installed
	return false;
}

bool CPatcher_ExtraThreads::Toggle( void ) {
	// This mod cannot be uninstalled once installed
	return false;
}

//-----------------------------------------------------------------------------
// INI Functions

bool CPatcher_ExtraThreads::ReadINI( void )
{
	if ( installed )
		return false;

	if ( !ReadINI_Bool( L"DisableExtraThreads" )) {
		int cpuReduction = ReadINI_Int( L"CPUReduction", 0, 90 );
		CThreadWatch::SetReductionPercentage( cpuReduction );
		bool cpuOnlyMinimized = ReadINI_Bool( L"CPUReduction_OnlyMinimized" );
		CThreadWatch::SetOptionOnlyMinimized( cpuOnlyMinimized );
		return Install();
	}

	return true;
}

bool CPatcher_ExtraThreads::WriteINI( void )
{
	if ( installed ) {
		int cpuReduction = ReadINI_Int( L"CPUReduction", 0, 90 );
		CThreadWatch::SetReductionPercentage( cpuReduction );
		bool cpuOnlyMinimized = ReadINI_Bool( L"CPUReduction_OnlyMinimized" );
		CThreadWatch::SetOptionOnlyMinimized( cpuOnlyMinimized );
	}
	return true;
}
