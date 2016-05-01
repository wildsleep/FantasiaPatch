#include "Patcher_ShowCombatPower.h"
#include "../Addr.h"
#include "../FileSystem.h"

//-----------------------------------------------------------------------------
// Static variable initialization

wchar_t CPatcher_ShowCombatPower::dataStrNull[1] = L"";
wchar_t CPatcher_ShowCombatPower::dataBuf[256];
LPBYTE CPatcher_ShowCombatPower::addrCStringTConstr = NULL;
LPBYTE CPatcher_ShowCombatPower::addrCStringTEquals = NULL;
LPBYTE CPatcher_ShowCombatPower::addrGetCombatPower = NULL;
LPBYTE CPatcher_ShowCombatPower::addrTargetReturn = NULL;

//-----------------------------------------------------------------------------
// Constructor

CPatcher_ShowCombatPower::CPatcher_ShowCombatPower( void )
{
	patchname = "Show Combat Power";

	addrCStringTConstr = CAddr::Addr(CStringTUni_Constr);
	addrCStringTEquals = CAddr::Addr(CStringTUni_Equals);
	addrGetCombatPower = CAddr::Addr(IParameterBase2_GetCombatPower);

	if (!addrCStringTConstr ||
		!addrCStringTEquals ||
		!addrGetCombatPower)
	{
		WriteLog("Patch initialization failed: %s.\n", patchname.c_str());
		WriteLog("   Missing dependency.\n");
		return;
	}

	// Patch mp1
	// We have to be careful here not to conflict with the Enable Name Coloring patch--
	// it modifies the same function up through +6C

	vector<WORD> patch1;
	vector<WORD> backup1;
	backup1 +=
		0x8B, 0xCF,				// +6D: MOV ECX, EDI
		0x74, 0x1A,				// +6F: JZ SHORT +1A
		0x8D, 0x45, 0x08,		// +71: LEA EAX, [EBP+08h]
		0x50,					// +74: PUSH EAX
		0xE8, -1, -1, -1, -1,	// +75: CALL NEAR xxxxxxxx (addrTarget2)
		0x8B, 0x16,				// +7A: MOV EDX, [ESI]
		0x83, 0x65, 0xFC, 0x00;	// +7C: AND [EBP-04h], 0
	patch1 +=
		-1, -1,
		0x90, 0x90,				// +6F: JZ => NOP
		-1, -1, -1,	-1,	-1, -1, -1, -1, -1,	-1, -1,	-1, -1, -1, -1;

	MemoryPatch mp1( NULL, patch1, backup1 );
	if (!mp1.Search( L"Pleione.dll" )) {
		WriteLog("Patch initialization failed: %s.\n", patchname.c_str());
		return;
	}

	// Patch mp2
	// The second target address is obtained from a CALL instruction in Patch 1,
	// so we check Patch 1's key first to make sure we get a valid address!

	DWORD offset =    (*(LPDWORD)(mp1.GetAddr() - 0x6D + 0x76));			// addrTarget1+0x75:  CALL NEAR addrTarget2
    LPBYTE addrTarget2 = (LPBYTE)(mp1.GetAddr() - 0x6D + 0x7A + offset);	// addr is relative to the next instruction
	addrTargetReturn = addrTarget2 + 0xA8;

	vector<WORD> patch2;
	vector<WORD> backup2;
	backup2 +=
		0x0F, 0x84, 0xC3, 0x00, 0x00, 0x00,			// +1A: JZ NEAR +0xC3
		0x8B, 0xCE,									// +20: MOV ECX, ESI
		0xFF, 0x15, -1, -1, -1, -1,					// +22: CALL xxxxxxxx (core::ICharacter::IsNamedNPC)
		0x84, 0xC0,									// +28: TEST AL, AL
		0x0F, 0x85, 0xB3, 0x00, 0x00, 0x00,			// +2A: JZ NEAR +0xB3
		0x8B, 0x06,									// +30: MOV ECX, [ESI]
		0x8B, 0xCE,									// +32: MOV ECX, ESI
		0xFF, 0x90, 0xA8, 0x01, 0x00, 0x00,			// +34: CALL DWORD PTR [EAX+1A8h]
		0x84, 0xC0,									// +3A: TEST AL, AL
		0x0F, 0x85, 0xA1, 0x00, 0x00, 0x00;			// +3C: JNZ NEAR +0xA1
	patch2 +=
		-1, -1, 0x01FF, 0x01FF, 0x01FF, 0x01FF,		// +1A: JZ patchCP1
		-1, -1,	-1, -1, -1, -1, -1, -1,	-1, -1,
		-1, -1, 0x01FF, 0x01FF, 0x01FF, 0x01FF,		// +2A: JZ patchCP1
		-1, -1,	-1, -1,	-1, -1, -1, -1, -1, -1,	-1, -1,
		-1, -1, 0x01FF, 0x01FF, 0x01FF, 0x01FF;		// +3C: JNZ patchCP1
	MemoryPatch mp2( addrTarget2 + 0x1A, patch2, backup2 );
	mp2.PatchRelativeAddress( 0x01, (LPBYTE)patchCP1 );

	// Patch mp3
	// Not to be confused with anything audio-related

	vector<WORD> patch3;
	vector<WORD> backup3;
	backup3 += 0x75, 0x2A;	// +7C: JNZ SHORT +2Ah
	patch3 += -1, 0xBE;		// +7C: JNZ SHORT -42h (destination: +3Ch)
	MemoryPatch mp3( addrTarget2 + 0x7C, patch3, backup3 );

	// Patch mp4
	// Not to be confused with anything video-related

	vector<WORD> patch4;
	vector<WORD> backup4;
	backup4 +=
		0x8D, 0x4D, 0xF0,						// +9F: LEA ECX, [EBP-10h]
		0xFF, 0x15, -1, -1, -1, -1;				// +A2: CALL xxxxxxxx (esl::CStringT::operator=(wchar_t const*))
	patch4 +=
		0xE9, 0x01FF, 0x01FF, 0x01FF, 0x01FF,	// +9F: JMP patchCP2
		0x90, 0x90, 0x90, 0x90;					// +A4: NOP (x4)
	MemoryPatch mp4( addrTarget2 + 0x9F, patch4, backup4 );
	mp4.PatchRelativeAddress( 0x01, (LPBYTE)patchCP2 );

	// Add and check all of our patches!

	patches += mp1;
	patches += mp2;
	patches += mp3;
	patches += mp4;
	if (CheckPatches())
		WriteLog("Patch initialization successful: %s.\n", patchname.c_str());
	else
		WriteLog("Patch initialization failed: %s.\n", patchname.c_str());
}

//-----------------------------------------------------------------------------
// Hook functions

/* patchCP1 performs the initialization necessary to get us the character name
 * prefix CStringT object.  We jump to it for all cases where we do NOT want to
 * show BOSS, AWFUL, etc. relative CP -- players, NPCs, etc.
 * For normal enemies we should never get here.
 */
NAKED void CPatcher_ShowCombatPower::patchCP1(void)
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
NAKED void CPatcher_ShowCombatPower::patchCP2(void)
{
	__asm {
		MOV		ECX, ESI					// ESI holds the "this" pointer... but what object are we?
		MOV		EAX, DWORD PTR DS:[ECX]		// EAX = vftable
		CALL	DWORD PTR DS:[EAX+4Ch]		// Returns the target ICharacter? (not 100% sure on that)
		MOV		ECX, EAX					// ECX should be our target ICharacter object now
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
void __stdcall CPatcher_ShowCombatPower::patchCPFormat(wchar_t * buff,double f,wchar_t* str)
{
	swprintf_s(buff,255,L"%.2f %s ",f,str);
}

//-----------------------------------------------------------------------------
// INI Functions

bool CPatcher_ShowCombatPower::ReadINI( void )
{
	if ( ReadINI_Bool( L"ShowCombatPower" ))
		return Install();
	return true;
}

bool CPatcher_ShowCombatPower::WriteINI( void )
{
	WriteINI_Bool( L"ShowCombatPower", IsInstalled() );
	return true;
}