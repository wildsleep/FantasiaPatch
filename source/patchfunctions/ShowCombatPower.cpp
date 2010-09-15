#include "patch.h"
#include "addr.h"

static const wchar_t *patchname = _T("ShowCombatPower");
static bool GetAddresses(void);
static bool CheckKeys(void);

/* ShowCombatPower
 * Show combat power numerically.
 * PATCHES: Pleione.dll
 */

/* Patch addresses */
static LPBYTE addrCStringTConstr = NULL;
static LPBYTE addrCStringTEquals = NULL;
static LPBYTE addrGetCombatPower = NULL;
static LPBYTE addrTarget1 = NULL;
static LPBYTE addrTarget2 = NULL;
static LPBYTE addrTargetReturn = NULL;
/* Data for assembly patches */
static wchar_t dataBuf[256];
static wchar_t dataStrNull[] = _T("");
/* Assembly patches */
void patchCP1(void);
void patchCP2(void);
void __stdcall patchCPFormat(wchar_t * buff,double f,wchar_t* str);

///////////////////////////////////////////////////////////////////////////////

bool CPatch::ShowCombatPower(void)
{
	try {
		if (!ReadINI_Bool(patchname)) return false;
		if (!GetAddresses()) return false;
		if (!CheckKeys()) return false;
		
		/* Patching addrTarget1 */

		BYTE code[] = { 0x90, 0x90 };
		WriteMem(addrTarget1 + 0x6F, code, sizeof(code));

		/* Patching addrTarget2 */

		DWORD JumpOffsetLong;

		JumpOffsetLong = (LPBYTE)patchCP1 - (addrTarget2 + 0x1B) - 5;	// Players
		WriteMem(addrTarget2 + 0x1B + 1, &JumpOffsetLong, sizeof(JumpOffsetLong));

		JumpOffsetLong = (LPBYTE)patchCP1 - (addrTarget2 + 0x2B) - 5;	// NPCs
		WriteMem(addrTarget2 + 0x2B + 1, &JumpOffsetLong, sizeof(JumpOffsetLong));

		JumpOffsetLong = (LPBYTE)patchCP1 - (addrTarget2 + 0x3D) - 5;	// ???? (Core::ICharacter::+1A8h returns nz)
		WriteMem(addrTarget2 + 0x3D + 1, &JumpOffsetLong, sizeof(JumpOffsetLong));

		// Offset of -0x7C-2 takes us to the beginning of our function
		// Then +0x3C takes us to +0x3C relative to the target
		// This makes a short jump to our long jump above!
		BYTE JumpOffsetShort = 0x3C - 0x7C - 2;							// Enemies, animals, etc...?
		WriteMem(addrTarget2 + 0x7C + 1, &JumpOffsetShort, sizeof(JumpOffsetShort));

		BYTE jump[] = {0xE9, 0, 0, 0, 0, 0x90, 0x90, 0x90, 0x90};
		*(LPDWORD)(jump + 1) = (LPBYTE)patchCP2 - (addrTarget2 + 0x9F) - 5;
		WriteMem(addrTarget2 + 0x9F, jump, sizeof(jump));

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
	addrCStringTConstr = CAddr::Addr(CStringTUni_Constr);
	addrCStringTEquals = CAddr::Addr(CStringTUni_Equals);
	addrGetCombatPower = CAddr::Addr(IParameterBase2_GetCombatPower);

	if (!addrCStringTConstr ||
		!addrCStringTEquals ||
		!addrGetCombatPower)
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Missing dependency.\n");
		return false;
	}
	
	// CCatchBox::Method0xA4 + 0x73: call addrTarget1
    addrTarget1 = (LPBYTE)(0x63926068);
	addrTarget2 = NULL;

    DWORD offset = (*(LPDWORD)(addrTarget1 + 0x76)); // addrTarget1+0x75:  call short addrTarget2
    addrTarget2 = (LPBYTE)(addrTarget1 + 0x7A + offset);  // address of next instruction

	addrTargetReturn = addrTarget2 + 0xA8;
	return true;
}

static bool CheckKeys(void)
{
	if ((*(LPWORD)(addrTarget1 + 0x6F) != 0x1A74)) {
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key 1 does not match.\n");
		return false;
	}

	if ((*(LPWORD)(addrTarget2 + 0x1A) != 0x840F) ||	// JE xxxxxxxx
		(*(LPWORD)(addrTarget2 + 0x2A) != 0x850F) ||	// JNZ xxxxxxxx
		(*(LPWORD)(addrTarget2 + 0x3C) != 0x850F) ||	// JNZ xxxxxxxx
		(*(LPWORD)(addrTarget2 + 0x7C) != 0x2A75) ||	// JNZ $+2CH
		(*(LPWORD)(addrTarget2 + 0x9F) != 0x4D8D) ||	// LEA ECX, [EBP-10]
		(*(LPWORD)(addrTarget2 + 0xA8) != 0x016A))		// PUSH 1 (lpRet)
	{
		WriteLog("Patch failed: %ls\n", patchname);
		WriteLog("   Key 2 does not match.\n");
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////

/* patchCP1 performs the initialization necessary to get us the character name
 * prefix CStringT object.  We jump to it for all cases where we do NOT want to
 * show BOSS, AWFUL, etc. relative CP -- players, NPCs, etc.
 * For normal enemies we should never get here.
 */
NAKED static void patchCP1(void)
{
	__asm {
		LEA		ECX, DWORD PTR SS:[EBP-10h] // [EBP-10h] is the CStringT object for the text prefixing the character name
		CALL	addrCStringTConstr			// No arguments -- init to ""
		AND     DWORD PTR SS:[EBP-4h], 0	// The Pleione code does this after initializing the CStringT...?

		PUSH	offset dataStrNull
		JMP     patchCP2
	}
}

/* patchCP2 takes the string prefix on the stack, gets the target CP,
 * formats it with our formatting function, and fills our CStringT object
 * with the formatted text.
 */
NAKED static void patchCP2(void)
{
	__asm {
		MOV		ECX, ESI					// ESI holds the "this" pointer... but what object are we?
		MOV		EAX, DWORD PTR DS:[ECX]		// EAX = vftable
		CALL	DWORD PTR DS:[EAX+4Ch]		// Wtf does this do? -> returns the target ICharacter??
		MOV		ECX, EAX					// Ok, ECX should be our target ICharacter object now
		CALL	addrGetCombatPower

		SUB		ESP, 8h						// Allocate stack space for the CP value
		FSTP	QWORD PTR SS:[ESP]			// Store the CP value on the stack

		PUSH	offset dataBuf				// Push the CP value
		CALL	patchCPFormat				// Call our formatting function.  The string arg was pushed
											// either by the Pleione code (for enemies-- <mini>BOSS </mini>
											// or whatever) or by patchCP1.

		PUSH	offset dataBuf				// Our buffer was now filled with the formatted string
		LEA		ECX, DWORD PTR SS:[EBP-10h]	// Put the CStringT object into the "this" register...
		CALL	addrCStringTEquals			// and set its text to our formatted string!

		JMP		addrTargetReturn;			// Jump back to Pleione code.
	}
}

/* patchCPFormat takes our string prefix, the CP value, and writes
 * into the buffer a complete prefix including both.
 */
static void __stdcall patchCPFormat(wchar_t * buff,double f,wchar_t* str)
{
	swprintf_s(buff,255,L"%.2f %s ",f,str);
}