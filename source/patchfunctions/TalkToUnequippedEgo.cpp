#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("TalkToUnequippedEgo");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* TalkToUnequippedEgo
 * Allow conversation with unequipped spirit weapons.
 * PATCHES: Pleione.dll
 */

/* Patch addresses */
static LPBYTE addrTarget = NULL;

///////////////////////////////////////////////////////////////////////////////

bool CPatch::TalkToUnequippedEgo(void)
{
	try	{
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		BYTE code[] = { 0x0F, 0x81 };
		WriteMem(addrTarget + 0x5D, code, sizeof(code));

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
	// CCharacterController::Method0x48 + 0x306
    addrTarget = (LPBYTE)(0x639FF5D2);
	return true;
}

static bool CheckKeys(void)
{
	if ((*(LPWORD)(addrTarget + 0x5B) != 0xC084) ||	// TEST AL, AL
		(*(LPWORD)(addrTarget + 0x5D) != 0x850F) ||	// JNZ +0x83
		(*(LPDWORD)(addrTarget + 0x5F) != 0x00000083))
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}