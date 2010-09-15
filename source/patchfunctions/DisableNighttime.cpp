#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("DisableNighttime");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* DisableNighttime
 * Cause daylight appearance between the hours of 18:00 and 4:00.
 * PATCHES: Renderer2.dll
 */

/* Patch addresses */
static LPBYTE addrSetSkyTime = NULL;
/* Assembly patches */
void patchNighttime(void);

///////////////////////////////////////////////////////////////////////////////

bool CPatch::DisableNighttime(void)
{
	try	{
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		BYTE code[] = {0xE9, 0, 0, 0, 0, 0x90};
		*(LPDWORD)(code + 1) = (LPBYTE)patchNighttime - (addrSetSkyTime + 1) - 4;
		WriteMem(addrSetSkyTime, code, sizeof(code));

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

static bool GetAddresses(void) {
	addrSetSkyTime = CAddr::Addr(CAtmosphere_SetSkyTime);
	if (!addrSetSkyTime) {
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Missing dependency.\n");
		return false;
	}
	return true;
}

static bool CheckKeys(void) {
	// We are replacing the entire function with a jump to our own
	// code, so there is no key to check for!
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Note on SetSkyTime
// 0~24 hours maps linearly to 0.0~1.0, such that 12:00 noon is 0.5.
// If less than 4:00, then set to 4:00.
// If greater than 18:00, then set to 18:00.
///////////////////////////////////////////////////////////////////////////////

NAKED static void patchNighttime(void)
{
	__asm {
		MOV		EAX, DWORD PTR DS:[ECX]
		MOV		ECX, 3E2AAAADh				// 0x3E2AAAAD = 1/6 = 4:00
		CMP		ECX, DWORD PTR SS:[ESP+4h]
		JA		jmp_exit
		MOV		ECX, 3F400000h				// 0x3F400000 = 3/4 = 18:00
		CMP		ECX, DWORD PTR SS:[ESP+4h]
		JB		jmp_exit
		MOV		ECX, DWORD PTR SS:[ESP+4h]
	jmp_exit:								// ECX = time to set
		MOV		DWORD PTR DS:[EAX+0Ch], ECX
		RETN	4h
	}
}