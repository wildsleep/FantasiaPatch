#include "HookIJL.h"
#include "filesystem.h"
using namespace Platform;

HMODULE g_IJL; //ijl15.dat

typedef void (*IJL_VOID)(void);

IJL_VOID lpfnIJLGetLibVersion = NULL;
IJL_VOID lpfnIJLInit = NULL;
IJL_VOID lpfnIJLFree = NULL;
IJL_VOID lpfnIJLRead = NULL;
IJL_VOID lpfnIJLWrite = NULL;
IJL_VOID lpfnIJLErrorStr = NULL;

DWORD ssQuality;

void ReleaseIJL()
{
	if(g_IJL)
	{
		FreeLibrary(g_IJL);
	}
}

bool HookIJL()
{
	wstring ijlDat=GetCurPath();
	ijlDat+=L"\\ijl15.dat";
	
	g_IJL=NULL;
	g_IJL = LoadLibrary(ijlDat.c_str());
	if (g_IJL == NULL)
	{
		WriteLog("Failed to hook IJL.dat.\n");
		return false;
	}

	lpfnIJLGetLibVersion = (IJL_VOID)
		GetProcAddress(g_IJL, "ijlGetLibVersion");
	lpfnIJLInit = (IJL_VOID)
		GetProcAddress(g_IJL, "ijlInit");
	lpfnIJLFree = (IJL_VOID)
		GetProcAddress(g_IJL, "ijlFree");
	lpfnIJLRead = (IJL_VOID)
		GetProcAddress(g_IJL, "ijlRead");
	lpfnIJLWrite = (IJL_VOID)
		GetProcAddress(g_IJL, "ijlWrite");
	lpfnIJLErrorStr = (IJL_VOID)
		GetProcAddress(g_IJL, "ijlErrorStr");

	return true;
}

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