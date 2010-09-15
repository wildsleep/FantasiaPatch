#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("test");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* test
 * Description
 */

/* Patch addresses */
static LPBYTE addrSetCamera = NULL;
static LPBYTE addrTarget = NULL;

///////////////////////////////////////////////////////////////////////////////

bool CPatch::test(void)
{
	try {
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		float widthLimit = patchSetting * 1.0f;
		WriteMem(addrSetCamera + 0xC2, &widthLimit, sizeof(widthLimit));

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
	addrSetCamera = CAddr::Addr(CCameraControl_SetCamera);
	if (!addrSetCamera) {
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Missing dependency.\n");
		return false;
	}
	addrTarget = 
	return true;
}

static bool CheckKeys(void)
{
	/*
	if (*(LPWORD)(addrSetCamera + 0xC0) != 0x05D9)
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	*/
	return true;
}