#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("ModifyZoomWidthLimit");
static uint patchSetting;
static bool GetAddresses(void);
static bool CheckKeys(void);

/* ModifyZoomWidthLimit
 * Modify the maximum zoom distance (1~5000).
 * PATCHES: Renderer2.dll
 */

/* Patch addresses */
static LPBYTE addrSetCamera = NULL;
static LPBYTE addrLimitVariable = NULL;

///////////////////////////////////////////////////////////////////////////////

bool CPatch::ModifyZoomWidthLimit(void)
{
	try {
		if (!(patchSetting = ReadINI_Int(patchname, 1, 5000))) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		float widthLimit = patchSetting * 1.0f;

		WriteMem(addrLimitVariable, &widthLimit, sizeof(widthLimit));

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
    addrLimitVariable = (LPBYTE)(*(LPDWORD)(addrSetCamera + 0xC2));
	return true;
}

static bool CheckKeys(void)
{
	if (*(LPWORD)(addrSetCamera + 0xC0) != 0x05D9 ||
        *(LPDWORD)(addrLimitVariable) != 0x44BB8000)
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}