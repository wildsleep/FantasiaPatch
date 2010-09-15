#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("MoveToSameChannel");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* MoveToSameChannel
 * Enable moving to the same channel you are currently on
 * PATCHES: Pleione.dll
 */

/* Patch addresses */
static LPBYTE addrTarget = NULL;

///////////////////////////////////////////////////////////////////////////////

bool CPatch::MoveToSameChannel(void)
{
	try	{
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		BYTE code[] = { 0x90, 0xE9 };
		WriteMem(addrTarget + 0xF6, code, sizeof(code));

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
    addrTarget = (LPBYTE)(0x638ABCDB);
	return true;
}

static bool CheckKeys(void)
{
	if ((*(LPWORD)(addrTarget + 0xF6) != 0x840F) ||
		(*(LPDWORD)(addrTarget + 0xF8) != 0x0000008C))
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}