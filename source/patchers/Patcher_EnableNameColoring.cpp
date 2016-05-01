#include "Patcher_EnableNameColoring.h"
#include "../Addr.h"
#include "../FileSystem.h"

//-----------------------------------------------------------------------------
// Static variable initialization

LPBYTE CPatcher_EnableNameColoring::addrTarget = NULL;
LPBYTE CPatcher_EnableNameColoring::addrIsElf = NULL;
LPBYTE CPatcher_EnableNameColoring::addrIsGiant = NULL;
LPBYTE CPatcher_EnableNameColoring::addrIsPet = NULL;
LPBYTE CPatcher_EnableNameColoring::addrIsNPC = NULL;
LPBYTE CPatcher_EnableNameColoring::addrIsGoodNPC = NULL;

//-----------------------------------------------------------------------------
// Constructor

CPatcher_EnableNameColoring::CPatcher_EnableNameColoring( void )
{
	patchname = "Enable Name Coloring";

	addrIsElf = CAddr::Addr(ICharacter_IsElf);
	addrIsGiant = CAddr::Addr(ICharacter_IsGiant);
	addrIsPet = CAddr::Addr(ICharacter_IsPet);
	addrIsNPC = CAddr::Addr(ICharacter_IsNPC);
	addrIsGoodNPC = CAddr::Addr(ICharacter_IsGoodNPC);

	if (!addrIsElf   ||
		!addrIsGiant ||
		!addrIsPet   ||
		!addrIsNPC   ||
		!addrIsGoodNPC)
	{
		WriteLog("Patch initialization failed: %s.\n", patchname.c_str());
		WriteLog("   Missing dependency.\n");
		return;
	}

	vector<WORD> patch;
	vector<WORD> backup;

	backup +=
		0xEB, 0x0E,									// +53: JMP SHORT +0E
		0xC7, 0x46, 0x58, 0xFF, 0x66, 0x33, 0xFF,	// +55: MOV DWORD PTR DS:[ESI+58h], 0xFF3366FF
		0xC7, 0x46, 0x5C, 0xFF, 0x66, 0x33, 0x30,	// +5C: MOV DWORD PTR DS:[ESI+5Ch], 0x303366FF
		0x8B, 0xCF,									// +63: MOV ECX, EDI
		0xFF, 0x15,	-1, -1, -1, -1,					// +65: CALL xxxxxxxx (core::ICharacter::IsNPC)
		0x84, 0xC0;									// +6B: TEST AL, AL

	patch +=
		0xEB, 0x0B,									// +53: JMP SHORT +0Bh (+60)
		0xEB, 0x0E,									// +55: JMP SHORT +0Eh (+65)
		0xB8, 0x01FF, 0x01FF, 0x01FF, 0x01FF,		// +57: MOV EAX, patchNameColoring
		0xFF, 0xD0,									// +5C: CALL EAX
		0xEB, 0x0D,									// +5E: JMP SHORT +0Dh (+6D)
		0x8B, 0xCF,									// +60: MOV ECX, EDI
		0xEB, 0x0D,									// +62: JMP SHORT +0Dh (+71)
		0x90,										// +64: NOP
		0x8B, 0x1D, -1, -1, -1, -1,					// +65: MOV EBX, xxxxxxxx (core::ICharacter::IsNPC)
		0xEB, 0xEA;									// +6B: JMP SHORT -16h (+57)

	MemoryPatch mp( NULL, patch, backup );
	mp.Search( L"Pleione.dll" );
	mp.PatchExactAddress( 0x01, (LPBYTE)patchNameColoring );
	
	patches += mp;
	if (CheckPatches())
		WriteLog("Patch initialization successful: %s.\n", patchname.c_str());
	else
		WriteLog("Patch initialization failed: %s.\n", patchname.c_str());
}

//-----------------------------------------------------------------------------
// Hook functions

NAKED void CPatcher_EnableNameColoring::patchNameColoring(void)
{
	__asm {
		MOV		ECX, EDI
		CALL	addrIsElf
		TEST	EAX, EAX
		JNE SHORT LblElf

		MOV		ECX, EDI
		CALL	addrIsGiant
		TEST	EAX, EAX
		JNE SHORT LblGiant
		
		MOV		ECX, EDI
		CALL	addrIsPet
		TEST	EAX, EAX
		JNE SHORT LblPet

		MOV		ECX, EDI
		CALL	addrIsGoodNPC
		TEST	EAX, EAX
		JNE SHORT LblGoodNPC

		MOV		ECX,EDI
		CALL	addrIsNPC
		TEST	EAX,EAX
		JNE SHORT LblEnemy

//	LblHuman:
		MOV		DWORD PTR DS:[ESI+58h], 0xFF00CCFF
		MOV		DWORD PTR DS:[ESI+5Ch], 0x0000CCFF
		JMP		LblEnd
	LblElf:
		MOV		DWORD PTR DS:[ESI+58h], 0xFFFFC0CB
		MOV		DWORD PTR DS:[ESI+5Ch], 0x00FFC0CB
		JMP		LblEnd
	LblGiant:
		MOV		DWORD PTR DS:[ESI+58h], 0xFF00CC00
		MOV		DWORD PTR DS:[ESI+5Ch], 0x0000CC00
		JMP		LblEnd
	LblPet:
		MOV		DWORD PTR DS:[ESI+58h], 0xFFFFCC00
		MOV		DWORD PTR DS:[ESI+5Ch], 0x00FFCC00
		JMP		LblEnd
	LblGoodNPC:
		MOV		DWORD PTR DS:[ESI+58h], 0xFFFF6600
		MOV		DWORD PTR DS:[ESI+5Ch], 0x00993300
		JMP		LblEnd
	LblEnemy:
		MOV		DWORD PTR DS:[ESI+58h], 0xFF3E62FF
		MOV		DWORD PTR DS:[ESI+5Ch], 0x003E62FF
//		JMP		LblEnd

	LblEnd:
		MOV		ECX, EDI
		CALL	addrIsNPC
		TEST	EAX, EAX
		RETN
	}
}

//-----------------------------------------------------------------------------
// INI Functions

bool CPatcher_EnableNameColoring::ReadINI( void )
{
	if ( ReadINI_Bool( L"EnableNameColoring" ))
		return Install();
	return true;
}

bool CPatcher_EnableNameColoring::WriteINI( void )
{
	WriteINI_Bool( L"EnableNameColoring", IsInstalled() );
	return true;
}