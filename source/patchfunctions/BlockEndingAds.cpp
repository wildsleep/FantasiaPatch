#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("BlockEndingAds");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* BlockEndingAds
 * Block the popup ads on game exit.
 * PATCHES: Pleione.dll
 */

/* Patch addresses */
static LPBYTE addrCleanUp = NULL;

///////////////////////////////////////////////////////////////////////////////

bool CPatch::BlockEndingAds(void)
{
	try {
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		BYTE code[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		WriteMem(addrCleanUp + 0x536, code, sizeof(code));

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
	addrCleanUp = CAddr::Addr(CPleione_CleanUp);
	if (!addrCleanUp) {
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Missing dependency.\n");
		return false;
	}
	return true;
}

static bool CheckKeys(void)
{
	if ((*(LPWORD)(addrCleanUp + 0x526) != 0x4D8D) ||
		(*(LPWORD)(addrCleanUp + 0x536) != 0x15FF))
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}