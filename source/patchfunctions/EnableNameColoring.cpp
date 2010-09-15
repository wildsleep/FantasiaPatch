#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("EnableNameColoring");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* EnableNameColoring
 * Enable coloring of ALT names based on character type.
 * PATCHES: Pleione.dll
 */

/* Patch addresses */
static LPBYTE addrTarget = NULL;
static LPBYTE addrIsElf = NULL;
static LPBYTE addrIsGiant = NULL;
static LPBYTE addrIsPet = NULL;
static LPBYTE addrIsNPC = NULL;
static LPBYTE addrIsGoodNPC = NULL;

/* Assembly patches */
void patchNameColoring(void);

///////////////////////////////////////////////////////////////////////////////

bool CPatch::EnableNameColoring(void)
{
	try {
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;
		
		// Change the first hardcoded colors (?)
		BYTE colorCode1[] = { 0x64, 0x82, 0xC8, 0xFF };
		BYTE colorCode2[] = { 0x64, 0x82, 0xC8, 0x00 };
		WriteMem(addrTarget + 0x48, colorCode1, sizeof(colorCode1));
		WriteMem(addrTarget + 0x4F, colorCode2, sizeof(colorCode2));

		BYTE code1[] = { 0xEB, 0x0B, 0xEB, 0x0E, 0xB8, 0, 0, 0, 0,
						 0xFF, 0xD0, 0xEB, 0x0D, 0x8B, 0xCF, 0xEB, 0x0D, 0x90, 0x8B, 0x1D };
		*(LPDWORD)(code1 + 5) = (DWORD)patchNameColoring;
		WriteMem(addrTarget + 0x53, code1, sizeof(code1));

		BYTE code2[] = { 0xEB, 0xEA };
		WriteMem(addrTarget + 0x6B, code2, sizeof(code2));

		WriteLog("Patch success: %ls\n", patchname);
		return true;
	}
	catch(...)
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Unhandled exception.\n");
		return false;
	}
}

///////////////////////////////////////////////////////////////////////////////

static bool GetAddresses(void)
{
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
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Missing dependency.\n");
		return false;
	}

	// addrTarget = ShowCombatPower addrTarget1
	addrTarget = (LPBYTE)(0x63926068);
	return true;
}

static bool CheckKeys(void)
{
	if ((*(LPWORD)(addrTarget + 0x6F) != 0x1A74)) {
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////

NAKED static void patchNameColoring(void)
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