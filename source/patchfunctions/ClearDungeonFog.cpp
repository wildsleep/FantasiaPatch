#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("ClearDungeonFog");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* ClearDungeonFog
 * Clear fog of war on dungeon minimaps.
 * PATCHES: Standard.dll
 */

/* Patch addresses */
static LPBYTE addrSetFog = NULL;

///////////////////////////////////////////////////////////////////////////////

bool CPatch::ClearDungeonFog(void)
{
	try	{
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		BYTE code[] = { 0x32, 0xC9, 0x90, 0x90 };
		WriteMem(addrSetFog + 0x02, code, sizeof(code));

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
	addrSetFog = CAddr::Addr(CDungeonRegion_SetFog);
	if (!addrSetFog) {
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Missing dependency.\n");
		return false;
	}
	return true;
}

static bool CheckKeys(void)
{
	if (*(LPDWORD)(addrSetFog + 0x02) != 0x04244C8A) {
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}