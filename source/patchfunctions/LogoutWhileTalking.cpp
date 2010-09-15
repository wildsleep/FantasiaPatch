#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("LogoutWhileTalking");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* LogoutWhileTalking
 * Allow logging out or exiting the game while talking to NPCs.
 * PATCHES: Pleione.dll
 */

/* Patch addresses */
static LPBYTE addrTarget = NULL;

///////////////////////////////////////////////////////////////////////////////

bool CPatch::LogoutWhileTalking(void)
{
	try	{
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		BYTE code1[] = { 0x84, 0xD1, 0xFF, 0xFF, 0xFF };
		WriteMem(addrTarget + 0x57, code1, sizeof(code1));

		BYTE code2[] = { 0x3B, 0xC0, 0x90, 0xEB, 0xEE };
		WriteMem(addrTarget + 0x63, code2, sizeof(code2));

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
    addrTarget = (LPBYTE)(0x6389BA63);
	return true;
}

static bool CheckKeys(void)
{
	if ((*(LPBYTE)(addrTarget + 0x4E) != 0x48)   ||	// DEC EAX
		(*(LPWORD)(addrTarget + 0x4F) != 0x840F) ||	// JZ xxxxxxxx
		(*(LPBYTE)(addrTarget + 0x55) != 0x48)   ||	// DEC EAX
		(*(LPWORD)(addrTarget + 0x56) != 0x840F) ||	// JZ xxxxxxxx
		(*(LPBYTE)(addrTarget + 0x5C) != 0x48)   ||	// DEC EAX
		(*(LPWORD)(addrTarget + 0x5D) != 0x850F))	// JNZ xxxxxxxx
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}