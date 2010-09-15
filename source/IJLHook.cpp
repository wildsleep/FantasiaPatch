#include "IJLHook.h"
#include "FileSystem.h"

// Global addresses of IJL.dat functions
IJL_VOID lpfnIJLGetLibVersion = NULL;
IJL_VOID lpfnIJLInit = NULL;
IJL_VOID lpfnIJLFree = NULL;
IJL_VOID lpfnIJLRead = NULL;
IJL_VOID lpfnIJLWrite = NULL;
IJL_VOID lpfnIJLErrorStr = NULL;

// Screenshot quality mod setting
DWORD ssQuality = 75;

//-------------------------------------------
// Methods

CIJLHook::~CIJLHook()
{
	if ( hIJL != NULL ) {
		FreeLibrary( hIJL );
	}
}

bool CIJLHook::Hook()
{
	wstring ijlDat=GetCurPath();
	ijlDat+=L"\\ijl15.dat";
	
	hIJL = LoadLibrary(ijlDat.c_str());
	if (hIJL == NULL)
	{
		DebugLog("Failed to load IJL.dat.\n");
		WriteLog("IJL hook installation failed.\n");
		return false;
	}

	lpfnIJLGetLibVersion	= (IJL_VOID)GetProcAddress(hIJL, "ijlGetLibVersion");
	lpfnIJLInit				= (IJL_VOID)GetProcAddress(hIJL, "ijlInit");
	lpfnIJLFree				= (IJL_VOID)GetProcAddress(hIJL, "ijlFree");
	lpfnIJLRead				= (IJL_VOID)GetProcAddress(hIJL, "ijlRead");
	lpfnIJLWrite			= (IJL_VOID)GetProcAddress(hIJL, "ijlWrite");
	lpfnIJLErrorStr			= (IJL_VOID)GetProcAddress(hIJL, "ijlErrorStr");

	if (!lpfnIJLGetLibVersion ||
		!lpfnIJLInit ||
		!lpfnIJLFree ||
		!lpfnIJLRead ||
		!lpfnIJLWrite ||
		!lpfnIJLErrorStr )
	{
		DebugLog("Failed to get IJL.dat export addresses.\n");
		WriteLog("IJL hook installation failed.\n");
		return false;
	}
	WriteLog("IJL hook installation successful.\n");
	return true;
}

void CIJLHook::SetSSQuality(int quality)
{
	ssQuality = quality;
}

int CIJLHook::GetSSQuality(void)
{
	return ssQuality;
}

//-------------------------------------------
// IJL.dll exports

NAKED void ijlGetLibVersion()
{
	__asm	jmp lpfnIJLGetLibVersion
}

NAKED void ijlInit()
{
	__asm	jmp lpfnIJLInit
}

NAKED void ijlFree()
{
	__asm	jmp lpfnIJLFree
}

NAKED void ijlRead()
{
	__asm	jmp lpfnIJLRead
}

NAKED void ijlWrite()
{
	__asm {
		PUSH	EBP
		MOV		EBP, ESP
		MOV		EAX, DWORD PTR SS:[EBP+8h]
		MOV		ECX, ssQuality
		MOV		DWORD PTR DS:[EAX+50h], ECX
		MOV		EDX, DWORD PTR SS:[EBP+0Ch]
		PUSH	EDX
		MOV		EAX, DWORD PTR SS:[EBP+08h]
		PUSH	EAX
		CALL	lpfnIJLWrite
		POP		EBP
		RETN
	}
}

NAKED void ijlErrorStr()
{
	__asm	jmp lpfnIJLErrorStr
}