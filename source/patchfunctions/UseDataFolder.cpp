#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("UseDataFolder");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* UseDataFolder
 * Load data from Data folder before loading from pack files.
 * PATCHES: ESL.dll
 */

/* Patch addresses */
static LPBYTE addrSetLookUpOrder = NULL;

///////////////////////////////////////////////////////////////////////////////

bool CPatch::UseDataFolder(void)
{
	try	{
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		BYTE code[] = {0x83, 0xE8, 0x01};
		WriteMem(addrSetLookUpOrder + 0xB8, code, sizeof(code));

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
	addrSetLookUpOrder = CAddr::Addr(CFileSystem_SetLookUpOrder);
	if (!addrSetLookUpOrder) {
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Missing dependency.\n");
		return false;
	}
	return true;
}

static bool CheckKeys(void)
{
	if ((*(LPBYTE)(addrSetLookUpOrder + 0xB8) != 0x83) ||
		(*(LPBYTE)(addrSetLookUpOrder + 0xB9) != 0xE8) ||
		(*(LPBYTE)(addrSetLookUpOrder + 0xBA) != 0x00))
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}
