#include "Patcher_ModifyZoomLimit.h"
#include "../Addr.h"
#include "../FileSystem.h"

//-----------------------------------------------------------------------------
// Static variable initialization

int CPatcher_ModifyZoomLimit::zoomLimit = 1500;

//-----------------------------------------------------------------------------
// Constructor

CPatcher_ModifyZoomLimit::CPatcher_ModifyZoomLimit( void )
{
	patchname = "Modify Zoom Limit";

	LPBYTE addrSetCamera = CAddr::Addr(CCameraControl_SetCamera);
	if (!addrSetCamera) {
		WriteLog("Patch initialization failed: %s.\n", patchname.c_str());
		WriteLog("   Missing dependency.\n");
		return;
	}

	vector<WORD> check;
	check +=
		0xD9, 0x05, -1, -1, -1, -1,	// +C0: FLD DWORD PTR DS:xxxxxxxx
		0x6A, 0x00,					// +C6: PUSH 0
		0x51,						// +C8: PUSH ECX
		0x8B, 0xCB,					// +C9: MOV ECX, EBX
		0xD9, 0x1C, 0x24;			// +CB: FSTP DWORD PTR SS:[ESP]
	MemoryPatch mpCheck( addrSetCamera + 0xC0, check, check );
	if (!mpCheck.Check()) {
		WriteLog("Patch initialization failed: %s.\n", patchname.c_str());
		return;
	}

	LPBYTE addrLimitVariable = (LPBYTE)(*(LPDWORD)(addrSetCamera + 0xC2));
	vector<WORD> patchPlaceholder;

	patchPlaceholder += 
		0x00, 0x80, 0xBB, 0x44;		// Float: 1500

	MemoryPatch mp( addrLimitVariable, patchPlaceholder, patchPlaceholder );
	
	patches += mp;
	if (CheckPatches())
		WriteLog("Patch initialization successful: %s.\n", patchname.c_str());
	else
		WriteLog("Patch initialization failed: %s.\n", patchname.c_str());
}

//-----------------------------------------------------------------------------
// Options

bool CPatcher_ModifyZoomLimit::SetLimit( int value )
{
	if ( patches.empty() ) return false;
	zoomLimit = value;
	float valueFloat = value * 1.0f;
	vector<WORD> newPatch;
	newPatch +=
		*((BYTE *)(&valueFloat)),
		*((BYTE *)(&valueFloat)+1),
		*((BYTE *)(&valueFloat)+2),
		*((BYTE *)(&valueFloat)+3);
	patches[0].SetPatch( newPatch );
	return true;
}

int CPatcher_ModifyZoomLimit::GetLimit( void )
{
	return zoomLimit;
}

//-----------------------------------------------------------------------------
// INI Functions

bool CPatcher_ModifyZoomLimit::ReadINI( void )
{
	if ( ReadINI_Bool( L"ModifyZoomLimit" ))
	{
		int zoomSetting = ReadINI_Int( L"ModifyZoomLimit", 1, 5000 );
		if ( !SetLimit( zoomSetting ))
			return false;
		return Install();
	}
	return true;
}

bool CPatcher_ModifyZoomLimit::WriteINI( void )
{
	if ( IsInstalled() )
		WriteINI_Int( L"ModifyZoomLimit", GetLimit() );
	return true;
}