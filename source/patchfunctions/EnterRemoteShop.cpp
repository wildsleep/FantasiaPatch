#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("EnterRemoteShop");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* EnterRemoteShop
 * Enable opening of player shops from a distance.
 * PATCHES: Pleione.dll
 */

/* Patch addresses */
static LPBYTE addrTarget = NULL;

///////////////////////////////////////////////////////////////////////////////

bool CPatch::EnterRemoteShop(void)
{
	try	{
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		BYTE code[] = { 0x90, 0x90 };
		WriteMem(addrTarget + 0x128, code, sizeof(code));

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

/*
$+117   D95D F0         fstp    dword ptr [ebp-10]
$+11A   D905 1033C363   fld     dword ptr [63C33310]
$+120   D85D F0         fcomp   dword ptr [ebp-10]
$+123   DFE0            fstsw   ax
$+125   F6C4 01         test    ah, 1
$+128   75 15           jnz     short 63845B38
$+12A   FF75 14         push    dword ptr [ebp+14]
$+12D   8BCE            mov     ecx, esi
$+12F   FF75 10         push    dword ptr [ebp+10]
$+132   FF75 0C         push    dword ptr [ebp+C]
$+135   FF75 08         push    dword ptr [ebp+8]
*/

static bool GetAddresses(void)
{
    addrTarget = (LPBYTE)(0x63845774);
	return true;
}

static bool CheckKeys(void)
{
	if ((*(LPWORD)(addrTarget + 0x117) != 0x5DD9) ||
		(*(LPWORD)(addrTarget + 0x123) != 0xE0DF) ||
		(*(LPWORD)(addrTarget + 0x128) != 0x1575))
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}
