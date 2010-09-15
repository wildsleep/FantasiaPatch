#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("TargetProps");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* TargetProps
 * Enable CTRL-targeting props while in combat mode.
 * PATCHES: Pleione.dll
 */

/* Patch addresses */
static LPBYTE addrTarget = NULL;
/* Data */
static TCHAR dataStr[] = _T("enemy|prop");
static DWORD addrStr = NULL;

///////////////////////////////////////////////////////////////////////////////

bool CPatch::TargetProps(void)
{
	try	{
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		WriteMem(addrTarget + 0x1D7, &addrStr, sizeof(addrStr));

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
	addrStr = (DWORD)(&dataStr);
    addrTarget = (LPBYTE)(0x63853103);
	return true;
}

static bool CheckKeys(void)
{
	if ((*(LPWORD)(addrTarget + 0x1D4) != 0x0775) ||	// JNZ SHORT +0x07
		(*(LPBYTE)(addrTarget + 0x1D6) != 0x68)   ||	// PUSH xxxxxxxx
		(*(LPWORD)(addrTarget + 0x1DB) != 0x05EB) ||	// JMP SHORT +0x05
		(*(LPBYTE)(addrTarget + 0x1DD) != 0x68))		// PUSH xxxxxxxx
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}