#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("EnableFSAA");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* EnableFSAA
 * Enable full-screen antialiasing.
 * PATCHES: Pleione.dll
 */

/* Patch addresses */
static LPBYTE addrReadFSAA = NULL;
static LPBYTE addrReadFSAAQuality = NULL;
static LPBYTE addrTarget1 = NULL;
static LPBYTE addrTarget2 = NULL;
static LPBYTE addrTargetReturn = NULL;
/* Assembly patches */
void patchFSAA(void);

///////////////////////////////////////////////////////////////////////////////

bool CPatch::EnableFSAA(void)
{
	try {
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		BYTE jump[] = { 0xE9, 0, 0, 0, 0, 0x90 }; // jmp patchFSAA; nop
		*(LPDWORD)(jump + 1) = (LPBYTE)patchFSAA - (addrTarget1 + 0x3B + 1) - 4;
		WriteMem(addrTarget1 + 0x3B, jump, sizeof(jump));

		BYTE code[] = { 0x6A, 0x01 }; // push 0x01
		WriteMem(addrTarget2 + 0x5AF, code, sizeof(code));

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
	addrReadFSAA = CAddr::Addr(CGameOptionMgr_ReadFSAA);
	addrReadFSAAQuality = CAddr::Addr(CGameOptionMgr_ReadFSAAQuality);

	if (!addrReadFSAA ||
        !addrReadFSAAQuality)
    {
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Missing dependency.\n");
		return false;
	}

	// CPleione::StartUp + 0x40F: call addrTarget1
	// CMainChatView::Method0xA8 + 0x853 -> + 0x84 -> + 0x1D0 -> +0xBB: call addrTarget2
	addrTarget1 = (LPBYTE)(0x6380AB30);
	addrTarget2 = (LPBYTE)(0x63991843);
	addrTargetReturn = addrTarget1 + 0x3B + 6; // Location of patch + size of patch
	return true;
}

static bool CheckKeys(void) {
	if ((*(LPBYTE)(addrTarget1 + 0x3B) != 0x89) ||		// MOV [ESI+10h], EDI
		(*(LPBYTE)(addrTarget1 + 0x3C) != 0x7E) ||		// ...
		(*(LPBYTE)(addrTarget1 + 0x3D) != 0x10) ||		// ...
		(*(LPBYTE)(addrTarget1 + 0x3E) != 0x89) ||		// MOV [ESI+14h], EDI
		(*(LPBYTE)(addrTarget1 + 0x3F) != 0x7E) ||		// ...
		(*(LPBYTE)(addrTarget1 + 0x40) != 0x14))		// ...
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key 1 does not match.\n");
		return false;
	}

	if ((*(LPBYTE)(addrTarget2 + 0x5AF) != 0x6A) ||		// PUSH 0
		(*(LPBYTE)(addrTarget2 + 0x5B0) != 0x00))		// ...
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key 2 does not match.\n");
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////

NAKED static void patchFSAA(void)
{
	__asm {
		pushad;
		pushf;
		call	addrReadFSAA;
		mov		ecx,esi;
		mov		[esi+0x10],eax;
		call	addrReadFSAAQuality;
		mov		ecx,esi;
		mov		[esi+0x14],eax;
		popf;
		popad;
		jmp		addrTargetReturn;
	}
}