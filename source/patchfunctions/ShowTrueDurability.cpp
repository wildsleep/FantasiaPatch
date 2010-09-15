#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("ShowTrueDurability");
static uint patchSetting;
static bool GetAddresses(void);
static bool CheckKeys(void);

/* ShowTrueDurability
 * Show item durability with 1000x precision.
 * PATCHES: Pleione.dll
 */

/* Patch addresses */
static LPBYTE addrGetInterfaceDurability = NULL;
static LPBYTE addrGetInterfaceDurabilityMax = NULL;
static LPBYTE addrCFormatterInsert = NULL;
static LPBYTE addrTarget = NULL;
static LPBYTE addrTargetReturn = NULL;
static LPBYTE addrColorReturn = NULL;
static LPBYTE addrColorReturnPerfect = NULL;
static DWORD addrDurabilityString = NULL;
static LPBYTE addrTargetPatch = NULL;
/* Data for assembly patches */
static TCHAR pszDurabilityString1[] = _T("Durability {1}/{3} ({0}/{2})");	// Durability 12345/25000 (13/25)
static TCHAR pszDurabilityString2[] = _T("Durability {0}/{2} ({1}/{3})");	// Durability 13/25 (12345/25000)
static TCHAR pszDurabilityString3[] = _T("Durability {0}/{1}");				// Durability 12345/25000
static TCHAR pszDurabilityString4[] = _T("{1}/{3} ({0}/{2})");				// 12345/25000 (13/25)
static TCHAR pszDurabilityString5[] = _T("{0}/{2} ({1}/{3})");				// 13/25 (12345/25000)
static TCHAR pszDurabilityString6[] = _T("{0}/{1}");						// 12345/25000
static TCHAR pszColorString[] = _T("<color=3>");
/* Assembly patches */
void GetDurability(void);
void GetDurabilityMax(void);
void TargetPatch4Val(void);
void TargetPatch2Val(void);
void patchDurabilityColor(void);

///////////////////////////////////////////////////////////////////////////////

bool CPatch::ShowTrueDurability(void)
{
	try	{
		if (!(patchSetting = ReadINI_Int(patchname, 1, 6))) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;

		// Patch core::IItem::GetInterfaceDurability[Max] to jump to our functions below
		// Fill unnecessary leftover code with NOPs
		BYTE jumpCode1[] = { 0xE9, 0, 0, 0, 0,
			0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
			0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		*(LPDWORD)(jumpCode1 + 1) = (LPBYTE)GetDurability - (addrGetInterfaceDurability) - 5;
		WriteMem(addrGetInterfaceDurability, jumpCode1, sizeof(jumpCode1));
		*(LPDWORD)(jumpCode1 + 1) = (LPBYTE)GetDurabilityMax - (addrGetInterfaceDurabilityMax) - 5;
		WriteMem(addrGetInterfaceDurabilityMax, jumpCode1, sizeof(jumpCode1));

		// Patch durability string to our new string
		BYTE pushCode[] = { 0x68, 0, 0, 0, 0 };
		*(LPDWORD)(pushCode + 1) = addrDurabilityString;
		WriteMem(addrTarget + 0x47, pushCode, sizeof(pushCode));

		// Insert our patch code
		// Fill unnecessary leftover code with NOPs
		BYTE jumpCode2[] = { 0xE9, 0, 0, 0, 0,
			0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
			0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
			0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		*(LPDWORD)(jumpCode2 + 1) = addrTargetPatch - (addrTarget + 0x7A) - 5;
		WriteMem(addrTarget + 0x7A, jumpCode2, sizeof(jumpCode2));

		// Insert the color patch code
		BYTE jumpCode3[] = { 0xE9, 0, 0, 0, 0 };
		*(LPDWORD)(jumpCode3 + 1) = (LPBYTE)patchDurabilityColor - (addrTarget + 0x177) - 5;
		WriteMem(addrTarget + 0x177, jumpCode3, sizeof(jumpCode3));

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
	addrGetInterfaceDurability = CAddr::Addr(IItem_GetInterfaceDurability);
	addrGetInterfaceDurabilityMax = CAddr::Addr(IItem_GetInterfaceDurabilityMax);
	addrCFormatterInsert = CAddr::Addr(CFormatter_Insert);
	
	switch (patchSetting) {
		case 1:
			addrDurabilityString = (DWORD)(&pszDurabilityString1);
			addrTargetPatch = (LPBYTE)TargetPatch4Val;
			break;
		case 2:
			addrDurabilityString = (DWORD)(&pszDurabilityString2);
			addrTargetPatch = (LPBYTE)TargetPatch4Val;
			break;
		case 3:
			addrDurabilityString = (DWORD)(&pszDurabilityString3);
			addrTargetPatch = (LPBYTE)TargetPatch2Val;
			break;
		case 4:
			addrDurabilityString = (DWORD)(&pszDurabilityString4);
			addrTargetPatch = (LPBYTE)TargetPatch4Val;
			break;
		case 5:
			addrDurabilityString = (DWORD)(&pszDurabilityString5);
			addrTargetPatch = (LPBYTE)TargetPatch4Val;
			break;
		case 6:
			addrDurabilityString = (DWORD)(&pszDurabilityString6);
			addrTargetPatch = (LPBYTE)TargetPatch2Val;
			break;
		default:
			break;
	}

	if (!addrGetInterfaceDurability ||
		!addrGetInterfaceDurabilityMax ||
		!addrCFormatterInsert ||
		!addrDurabilityString ||
		!addrTargetPatch)
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Missing dependency.\n");
		return false;
	}

    addrTarget = (LPBYTE)(0x63A37B74);
	addrTargetReturn = addrTarget + 0x94;
	addrColorReturn = addrTarget + 0x17C;
	addrColorReturnPerfect = addrTarget + 0x1BC;
	return true;
}

static bool CheckKeys(void)
{
	if ((*(LPWORD)(addrTarget + 0x3B)  != 0x1D8B) ||	// mov edi, GetInterfaceDurability
		(*(LPWORD)(addrTarget + 0x7A)  != 0x15FF) ||	// call GetInterfaceDurabilityMax
		(*(LPWORD)(addrTarget + 0x177) != 0xD3FF) ||	// call ebx (GetInterfaceDurability)
		(*(LPWORD)(addrTarget + 0x179) != 0xF883) ||	// cmp eax, 0ah
		(*(LPWORD)(addrTarget + 0x17C) != 0x870F) ||	// ja $+9d
		(*(LPWORD)(addrTarget + 0x184) != 0x15FF) ||	// call GetInterfaceDurabilityMax
		(*(LPWORD)(addrTarget + 0x1A0) != 0x05DC))		// fadd (0.99)
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key does not match.\n");
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////

/* This replacement for IItem::GetInterfaceDurability[Max](void) performs the
 * same functions as the old, giving the same return value on EAX.  It also
 * returns the high-precision value on EDX, which is legal because the
 * __thiscall calling conventions treat EDX as a caller-save register.  This
 * allows our own code to access the high-precision value, while maintaining
 * the functionality of any existing code which may use the function.
 */

NAKED static void GetDurability(void)
{
	__asm {
		MOV		EAX, DWORD PTR DS:[ECX+4Ch]  // EAX = item durability x1000
		PUSH	EBX       // EBX is a callee-save register -> save it!
		MOV		EBX, EAX  // EBX = item durability x1000
		ADD		EAX, 999  // EAX = item dura x1000 + 999
		XOR		EDX, EDX  // EDX = 0
		MOV		ECX, 1000 // ECX = 1000
		DIV		ECX       // EAX = (item dura+999)/1000 = item durability
		MOV		EDX, EBX  // EDX = item durability x1000
		POP		EBX       // Restore value of EBX
		RETN              // EAX = item dura, EDX = item dura x1000
	}
}

NAKED static void GetDurabilityMax(void)
{
	__asm {
		MOV		EAX, DWORD PTR DS:[ECX+50h] // EAX = item max durability x1000
		PUSH	EBX
		MOV		EBX, EAX
		ADD		EAX, 999
		XOR		EDX, EDX
		MOV		ECX, 1000
		DIV		ECX
		MOV		EDX, EBX
		POP		EBX
		RETN
	}
}

/* Our patch for our target function overwrites the string formatting section
 * such that it passes our own values into the string formatter.
 */

NAKED static void TargetPatch4Val(void)
{
	__asm {
		CALL	GetDurabilityMax
		PUSH	EDX			// Push max dura x1000 onto stack
		PUSH	EAX			// Push max dura onto stack
		MOV		ECX, EDI	// EDI was the location of our IItem object -> put into ECX for __thiscall calling conventions
		CALL	GetDurability
		PUSH	EDX			// Push dura x1000 onto stack
		PUSH	EAX			// Push dura onto stack
		MOV		ECX, ESI	// ESI was the location of our CFormatter object -> put into ECX for __thiscall calling conventions
		MOV		ESI, addrCFormatterInsert
		CALL	ESI			// Take dura from stack
		MOV		ECX, EAX	// CFormatter::operator<< puts our CFormatter object back in EAX.
		CALL	ESI			// Take dura x1000 from stack
		MOV		ECX, EAX	// ...
		CALL	ESI			// Take max dura from stack
		MOV		ECX, EAX	// ...
		CALL	ESI			// Take max dura x1000 from stack
		MOV		ECX, EAX	// Put CFormatter back in ECX (later code expects it to be there, and we patched over the code that puts it there)
		JMP		addrTargetReturn
	}
}

NAKED static void TargetPatch2Val(void)
{
	__asm {
		CALL	GetDurabilityMax
		PUSH	EDX			// Push max dura x1000 onto stack
		MOV		ECX, EDI
		CALL	GetDurability
		PUSH	EDX			// Push dura x1000 onto stack
		MOV		ECX, ESI
		MOV		ESI, addrCFormatterInsert
		CALL	ESI
		MOV		ECX, EAX
		CALL	ESI
		MOV		ECX, EAX
		JMP		addrTargetReturn
	}
}

/* This patch enables a different color text if at 100% dura.
 * The line of code we are replacing would call GetDurability(),
 * and compare the result to 0x0A.  We leave EAX just as expected
 * and perform the comparison.  The values of ECX and EDX are
 * irrelevant because they are caller-save registers.
 */

NAKED static void patchDurabilityColor(void)
{
	__asm {
		CALL	GetDurabilityMax
		PUSH	EDX				// Push Max x1000
		MOV		ECX, EDI
		CALL	GetDurability
		POP		ECX				// EDX = Dura x1000, ECX = Max x1000
		CMP		ECX, EDX
		JE		returnPerfect
		CMP		EAX, 0x0A
		JMP		addrColorReturn // Normal or red text if dura != max dura
	returnPerfect:
		PUSH	offset pszColorString
		JMP		addrColorReturnPerfect
	}
}