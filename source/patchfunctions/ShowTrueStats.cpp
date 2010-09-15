#include "patch.h"

#define NAKED	__declspec(naked)

static CHAR pszPleioneStartup[] =	// pleione.dll
	"?StartUp@CPleione@pleione@@QAE_NPAUHINSTANCE__@@0ABV?$CStringT@_WVunicode_string_trait@esl@@Vunicode_string_implement@2@@esl@@11_N2@Z";

static CHAR psz_UpdateEtcs[] =			// pleione.dll::Startup()+0xA9EC1
	"?_UpdateEtcs@CCharacterView@pleione@@IAEXPAVCCharacter@2@@Z";
static CHAR pszUpdateWindow[] =			// pleione.dll::Startup()+0xAF57B
	"?UpdateWindow@CCharacterView@pleione@@UAEXXZ";
static CHAR pszswprintf[] =				// MSVCR71.dll
	"swprintf";

static LPVOID lpfnswprintf = NULL;
static LPVOID lpRet1 = NULL;
static LPVOID lpRet2 = NULL;
static LPVOID lpRet3 = NULL;
static LPVOID lpRet4 = NULL;

void Jump1(void);
void Jump2(void);
void Jump3(void);
void Jump4(void);
static bool init(void);
static LPBYTE FindLuckAddress(LPBYTE lpBase, DWORD Size);


// Poorly translated by Google:
// Associated with some more
// _UpdateEtcs In eight jump Jump1, 1 Department Jump2 jump
// UpdateWindow in two jump Jump1
// A print function in the string 2, respectively, to Jump3 Jump4 
bool CPatch::ShowTrueStats(void)
{
	UINT fgPatchOn = GetPrivateProfileInt(_T("PATCH"), _T("ShowTrueStats"), 0, iniFile.c_str());
	if (!fgPatchOn)
	{
		DebugPrint("ShowTrueStats = 0\n");
		return false;
	}
	DebugPrint("ShowTrueStats = 1\n");

	if (!init())
	{
		DebugPrint("ShowTrueStats(): init() failed\n");
		return false;
	}

	try
	{
		LPBYTE lpbuf1 = (LPBYTE)GetProcAddress( GetModuleHandle(_T("pleione.dll")),
												pszPleioneStartup );
		LPBYTE lpbuf2 = lpbuf1+0xAFCB6;
		lpbuf1+=0xA9EC1;
		LPBYTE lpbuf3 = NULL;

		if  (*(lpbuf1 + 0x2D) == 0xE8)
		{
			//lpbuf3 = (lpbuf1 + 0x2D) + (*(LPDWORD)(lpbuf1 + 0x3F))+5;
			lpbuf3=lpbuf1-0x10C;
		}
		else
		{
			DebugPrint("CPatch::ShowTrueStats(): lpbuf3 address is not correct\n");
			return false;
		}

		// Check for patch consistency
		if ((*(lpbuf1 + 0x13) != 0xE8) ||		// StrBase
			(*(lpbuf1 + 0x21) != 0xE8) ||		// Str
			(*(lpbuf1 + 0x3C) != 0xE8) ||		// IntBase
			(*(lpbuf1 + 0x4A) != 0xE8) ||		// Int
			(*(lpbuf1 + 0x65) != 0xE8) ||		// DexBase
			(*(lpbuf1 + 0x73) != 0xE8) ||		// Dex
			(*(lpbuf1 + 0x8E) != 0xE8) ||		// WillBase
			(*(lpbuf1 + 0x9C) != 0xE8) ||		// Will
			(*(lpbuf1 + 0xCD) != 0xE8) ||		// Def
			(*(lpbuf2 + 0x06) != 0xE8) ||		// LuckBase
			(*(lpbuf2 + 0x15) != 0xE8) ||		// Luck
			(*(LPWORD)(lpbuf3 + 0x56) != 0x0A6A) ||		// "Total quality"
			(*(LPWORD)(lpbuf3 + 0x7C) != 0x358B) ||		// "ret for the overall quality of"
			(*(LPWORD)(lpbuf3 + 0xA6) != 0x0A6A) ||		// "white quality"
			(*(LPWORD)(lpbuf3 + 0xB8) != 0x8D4D))		// "ret for white quality"
		{
			DebugPrint("CPatch::ShowTrueStats(): key does not match\n");
			return false;
		}
		else
		{
			DWORD CallOffset;

			lpRet1 = (lpbuf1 + 0x13) + (*(LPDWORD)(lpbuf1 + 0x14)) + 5;
			lpRet2 = lpbuf3;
			lpRet3 = lpbuf3 + 0x7C;
			lpRet4 = lpbuf3 + 0xB8;

			// Str.
			CallOffset = (LPBYTE)Jump1 - (lpbuf1 + 0x13) - 5;
			WriteMem(lpbuf1 + 0x14, &CallOffset, sizeof(CallOffset));
			CallOffset = (LPBYTE)Jump1 - (lpbuf1 + 0x21) - 5;
			WriteMem(lpbuf1 + 0x22, &CallOffset, sizeof(CallOffset));

			// Int.
			CallOffset = (LPBYTE)Jump1 - (lpbuf1 + 0x3C) - 5;
			WriteMem(lpbuf1 + 0x3D, &CallOffset, sizeof(CallOffset));
			CallOffset = (LPBYTE)Jump1 - (lpbuf1 + 0x4A) - 5;
			WriteMem(lpbuf1 + 0x4B, &CallOffset, sizeof(CallOffset));

			// Dex.
			CallOffset = (LPBYTE)Jump1 - (lpbuf1 + 0x65) - 5;
			WriteMem(lpbuf1 + 0x66, &CallOffset, sizeof(CallOffset));
			CallOffset = (LPBYTE)Jump1 - (lpbuf1 + 0x73) - 5;
			WriteMem(lpbuf1 + 0x74, &CallOffset, sizeof(CallOffset));

			// Will
			CallOffset = (LPBYTE)Jump1 - (lpbuf1 + 0x8E) - 5;
			WriteMem(lpbuf1 + 0x8F, &CallOffset, sizeof(CallOffset));
			CallOffset = (LPBYTE)Jump1 - (lpbuf1 + 0x9C) - 5;
			WriteMem(lpbuf1 + 0x9D, &CallOffset, sizeof(CallOffset));

			// Def.
			CallOffset = (LPBYTE)Jump2 - (lpbuf1 + 0xCD) - 5;
			WriteMem(lpbuf1 + 0xCE, &CallOffset, sizeof(CallOffset));

			// Luck
			CallOffset = (LPBYTE)Jump1 - (lpbuf2 +0x06) - 5;
			WriteMem(lpbuf2 + 0x07, &CallOffset, sizeof(CallOffset));
			CallOffset = (LPBYTE)Jump1 - (lpbuf2 + 0x15) - 5;
			WriteMem(lpbuf2 + 0x16, &CallOffset, sizeof(CallOffset));


			BYTE JmpCode[] = {0xE9, 0, 0, 0, 0, 0x90, 0x90, 0x90};

			*(LPDWORD)(JmpCode + 1) = (LPBYTE)Jump3 - (lpbuf3 + 0x56) - 5;
			WriteMem(lpbuf3 + 0x56, JmpCode, sizeof(JmpCode));

			*(LPDWORD)(JmpCode + 1) = (LPBYTE)Jump4 - (lpbuf3 + 0xA6) - 5;
			WriteMem(lpbuf3 + 0xA6, JmpCode, sizeof(JmpCode));
		}
	}
	catch(...)
	{
		DebugPrint("CPatch::ShowTrueStats() exception\n");
		return false;
	}

	WriteLog("Patch success: ShowTrueStats\n");
	return true;
}

static void FloattoString(wchar_t * buff,wchar_t* format,...)
{
	va_list	valist;
	va_start( valist, format );
	swprintf_s(buff,255,format,valist);
	va_end( valist );
}


static bool init(void)
{
	try
	{
		lpfnswprintf = FloattoString;
	}
	catch(...)
	{
		return false;
	}

	if (lpfnswprintf)
	{
		return true;
	}
	else
		return false;
}

// ==========================================================================
// "The quality of thousands of times (non-defense)"
NAKED static void Jump1(void)
{
	__asm
	{
		PUSH	3E8h
		FIMUL	DWORD PTR SS:[ESP]
		POP		EAX
		JMP		lpRet1
	}
}

// "Defense thousands of times"
NAKED static void Jump2(void)
{
	__asm
	{
		MOV		ECX, 3E8h
		MOV		EAX, DWORD PTR SS:[ESP+8]
		MUL		ECX
		MOV		DWORD PTR SS:[ESP+8], EAX
		MOV		EAX, EBX
		MUL		ECX
		MOV		EBX, EAX
		JMP		lpRet2
	}
}

static TCHAR pszFormat[] = _T("%.3f");
// "Thousands of times to restore"
NAKED static void Jump3(void)
{
	__asm
	{
		; ebx = 总数	[EBP+C] = 基础值
		CMP		EBX, DWORD PTR SS:[EBP+0Ch]
		JE		jmp_onlyone

		PUSH	0Ah
		LEA		EAX, DWORD PTR SS:[EBP-110h]
		PUSH	EAX
		MOV		ECX, 3E8h
		MOV		EAX, EBX
		MOV		EDX, 0
		DIV		ECX
		PUSH	EAX
		CALL	EDI
		JMP		lpRet3

	jmp_onlyone:
		FILD	DWORD PTR SS:[EBP+0Ch]
		PUSH	3E8h
		FIDIV	DWORD PTR SS:[ESP]
		PUSH	ECX
		FSTP	QWORD PTR SS:[ESP]
		PUSH	offset pszFormat
		LEA		EAX, DWORD PTR SS:[EBP-110h]
		PUSH	EAX
		CALL	lpfnswprintf
		ADD     ESP, 4h
		JMP		lpRet3
	}
}

// "Restore the thousands of times - the basic quality"
static TCHAR pszColor[] = _T("</color>(");
NAKED static void Jump4(void)
{
	__asm
	{
		FILD	DWORD PTR SS:[EBP+0Ch]
		PUSH	3E8h
		FIDIV	DWORD PTR SS:[ESP]
		PUSH	ECX
		FSTP	QWORD PTR SS:[ESP]
		PUSH	offset pszFormat
		LEA		EAX, DWORD PTR SS:[EBP-110h]
		PUSH	EAX
		CALL	lpfnswprintf
		ADD     ESP, 0Ch

		PUSH	offset pszColor
		JMP		lpRet4
	}
}
