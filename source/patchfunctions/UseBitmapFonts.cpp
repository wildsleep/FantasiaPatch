#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("UseBitmapFonts");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* UseBitmapFonts
 * Use bitmap fonts instead of vector fonts to prevent window lag.
 * PATCHES: Renderer2.dll
 */

/* Patch addresses */
static LPBYTE addrCompiler = NULL;
static LPBYTE addrTagCompiler = NULL;
static LPBYTE addrHTMLCompiler = NULL;

///////////////////////////////////////////////////////////////////////////////

bool CPatch::UseBitmapFonts(void)
{
	try	{
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		BYTE code[] = { 0x90, 0x90 };
		WriteMem(addrCompiler + 0x4D, code, sizeof(code));
		WriteMem(addrTagCompiler + 0x15C, code, sizeof(code));
		WriteMem(addrHTMLCompiler + 0xE2, code, sizeof(code));

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
	addrCompiler = CAddr::Addr(CDefaultCompiler_Compile);
	addrTagCompiler = CAddr::Addr(CDefaultTagCompiler_Compile);
	addrHTMLCompiler = CAddr::Addr(CHTMLCompiler_Compile);

	if (!addrCompiler ||
		!addrTagCompiler ||
		!addrHTMLCompiler)
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Missing dependency.\n");
		return false;
	}

	return true;
}

static bool CheckKeys(void)
{
	if ((*(LPWORD)(addrCompiler + 0x4D)  != 0x0774) ||
		(*(LPWORD)(addrTagCompiler + 0x15C)  != 0x1174) ||
		(*(LPWORD)(addrHTMLCompiler + 0xE2) != 0x0B74))
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}