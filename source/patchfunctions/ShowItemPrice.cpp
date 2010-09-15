#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("ShowItemPrice");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* ShowItemPrice
 * Show the shop purchase and selling price in item descriptions.
 * PATCHES: Pleione.dll
 */

/* Patch addresses */
static LPBYTE addrTarget = NULL;

///////////////////////////////////////////////////////////////////////////////

bool CPatch::ShowItemPrice(void)
{
	try	{
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		BYTE code[] = {0x83, 0xF8, 0x02, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
		WriteMem(addrTarget + 0x155, code, sizeof(code));

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
	// CPocketWindow::Method0x120 = addrTarget
	addrTarget = (LPBYTE)(0x6396FB36);
	return true;
}

static bool CheckKeys(void)
{
	// +155: F8 02 0F 85 86 01 00 00
	if ((*(LPDWORD)(addrTarget + 0x155) != 0x850F02F8) ||
		(*(LPDWORD)(addrTarget + 0x159) != 0x00000186))
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}