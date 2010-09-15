#include "Patcher_MoveWhileTalking.h"
#include "../Addr.h"
#include "../FileSystem.h"

//-----------------------------------------------------------------------------
// Constructor

CPatcher_MoveWhileTalking::CPatcher_MoveWhileTalking( void )
{
	patchname = "Move While Talking";
    
	vector<WORD> patch;
	vector<WORD> backup;

	backup +=
		0x56,								// +00: PUSH ESI
		0x8B, 0xF1,							// +01: MOV ESI, ECX
		0xE8, -1, -1, -1, -1,				// +03: CALL xxxxxxxx
		0x83, 0x78, 0x4C, 0x00,				// +08: CMP DWORD PTR [EAX+4Ch], 0
		0x74, 0x13,							// +0C: JZ SHORT +13h
		0x8B, 0x06,							// +0E: MOV EAX, [ESI]
		0x8B, 0xCE,							// +10: MOV ECX, ESI
		0xFF, 0x90, 0xAC, 0x01, 0x00, 0x00;	// +12: CALL DWORD PTR [EAX+1ACh]
	patch +=
		-1,
		-1, -1,
		-1, -1, -1, -1, -1,
		-1, -1, -1, -1,
		0xEB, -1,							// +0C: JZ => JMP SHORT +13h
		-1, -1,
		-1, -1,
		-1, -1, -1, -1, -1, -1;

	MemoryPatch mp( NULL, patch, backup );
	mp.Search( L"Pleione.dll" );
	
	patches += mp;
	if (CheckPatches())
		WriteLog("Patch initialization successful: %s.\n", patchname.c_str());
	else
		WriteLog("Patch initialization failed: %s.\n", patchname.c_str());
}

//-----------------------------------------------------------------------------
// INI Functions

bool CPatcher_MoveWhileTalking::ReadINI( void )
{
	if ( ReadINI_Bool( L"MoveWhileTalking" ))
		return Install();
	return true;
}

bool CPatcher_MoveWhileTalking::WriteINI( void )
{
	WriteINI_Bool( L"MoveWhileTalking", IsInstalled() );
	return true;
}