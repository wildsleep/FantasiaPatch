#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("ShowTrueFoodQuality");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* ShowTrueFoodQuality
 * Show food quality numerically.
 * PATCHES: Pleione.dll
 */

/* Patch addresses */
static LPBYTE addrCStringTEquals = NULL;
static LPBYTE addrTarget = NULL;
static LPBYTE addrTargetReturn = NULL;
/* Data for assembly patches */
static wchar_t dataBuf[256];
/* Assembly patches */
void patchFoodQuality(void);
void patchFoodQualityFormat(wchar_t * buff,wchar_t* str,int d);

///////////////////////////////////////////////////////////////////////////////

bool CPatch::ShowTrueFoodQuality(void)
{
	try	{
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		BYTE jumpCode[] = {0xE9, 0, 0, 0, 0, 0x90, 0x90, 0x90};
		*(LPDWORD)(jumpCode + 1) = (LPBYTE)patchFoodQuality - (addrTarget + 0xCC + 1) - 4;
		WriteMem(addrTarget + 0xCC, jumpCode, sizeof(jumpCode));

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
	addrCStringTEquals = CAddr::Addr(CStringTUni_Equals);

	if (!addrCStringTEquals)
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Missing dependency.\n");
		return false;
	}

    addrTarget = (LPBYTE)(0x63ACE0C9);
	addrTargetReturn = addrTarget + 0xD4;
	return true;
}

static bool CheckKeys(void)
{
	if ((*(LPWORD)(addrTarget + 0xCC) != 0xCB8B) ||		//	mov	ecx,ebx;
		(*(LPWORD)(addrTarget + 0xD4) != 0x7D80))		//	cmp byte ptr[ebp+18],0
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////

NAKED static void patchFoodQuality(void)
{
	//EAX: Numeric food quality
	//EBX: String pointer
	//ECX: none
	__asm {
		pop		ecx				
		push	eax
		push	ecx
		push	offset dataBuf
		call	patchFoodQualityFormat
		add		esp, 0Ch

		push	offset dataBuf
		mov		ecx, ebx;
		call	addrCStringTEquals

		jmp		addrTargetReturn
	}
}

static void patchFoodQualityFormat(wchar_t * buff,wchar_t* str,int d)
{
	swprintf_s(buff,255,L"%s (%d)",str,d);
}
