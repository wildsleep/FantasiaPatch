#include "Main.h"

#include "FileSystem.h"
#include "HookIJL.h"
#include "EagleNTPatch.h"
#include "patchfunctions\Patch.h"

using namespace Platform;

#define IJL_VERSION "R45 v2 dev 002"

//=====================================================================================================================================================
wstring	iniFile;
//=====================================================================================================================================================

bool APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	try
	{
		switch (ul_reason_for_call)
		{
		case DLL_PROCESS_ATTACH:
			{
				//DisableThreadLibraryCalls( (HMODULE)hModule );
				DeleteFile(L".\\ijl_patchlog.txt");
				iniFile=GetCurPath();
				iniFile+=L"\\ijl15.ini";
                WriteLog("Mabinogi Fantasia Patch for %s\n", IJL_VERSION);
                WriteLog("Initialization successful.\n");
				if(!HookIJL())
				{
					WriteLog("HookIJL() operation failed.\n");
					return(false);
				}
				EagleNTPatchLoad();
				CPatch::Install();				
				WriteLog("Mabinogi Fantasia Patch successfully completed.\n");
				break;
			}

		case DLL_THREAD_ATTACH:break;
		case DLL_THREAD_DETACH:break;
		case DLL_PROCESS_DETACH:
			{
				ReleaseIJL();
				EagleNTPatchUnload();
				WriteLog("Mabinogi Fantasia Patch unloaded.\n");
				break;
			}
		}
		return true;
	}
	catch(...)
	{
		WriteLog("Mabinogi Fantasia Patch initialization failed.");
	}
}

void WriteLog( const char* format, ... )
{
	va_list	valist;
	FILE *fp;
	char buf1[32],buf2[32];

#if _MSC_VER >= 1400 // .NET2005
	if( fopen_s( &fp, ".\\ijl_patchlog.txt", "a" ) )
		return;
	_strdate_s( buf1, 32 );
	_strtime_s( buf2, 32 );
#else
	fp = fopen( ".\\patchlog.txt", "a" );
	if( fp == NULL )
		return;
	_strdate( buf1 );
	_strtime( buf2 );
#endif

	fprintf( fp, "[%s %s] - ", buf1, buf2 );

	va_start( valist, format );
	vfprintf( fp, format, valist );
	va_end( valist );
	fclose(fp);

}

void DebugLog( const char* format, ... )
{
	uint usingDebug = GetPrivateProfileInt(_T("PATCH"), _T("Debug"), 0, iniFile.c_str());
	if (usingDebug) {
		va_list	valist;
		FILE *fp;
		char buf1[32],buf2[32];

#if _MSC_VER >= 1400 // .NET2005
		if( fopen_s( &fp, ".\\ijl_patchlog.txt", "a" ) )
			return;
		_strdate_s( buf1, 32 );
		_strtime_s( buf2, 32 );
#else
		fp = fopen( ".\\patchlog.txt", "a" );
		if( fp == NULL )
			return;
		_strdate( buf1 );
		_strtime( buf2 );
#endif

		fprintf( fp, "[%s %s] - ", buf1, buf2 );

		va_start( valist, format );
		vfprintf( fp, format, valist );
		va_end( valist );
		fclose(fp);
	}
}


// ==========================================================================
// Write to the process memory.
//	lpAddress:	address to write to
//	lpBuffer:	data to write into memory
//	nSize:		size of data to write
// ==========================================================================
void WriteMem(LPVOID lpAddress, LPVOID lpBuffer, SIZE_T nSize)
{
	DWORD OldProtect, OldProtect2;
	VirtualProtect(lpAddress, nSize, PAGE_READWRITE, &OldProtect);
	memcpy(lpAddress, lpBuffer, nSize);
	VirtualProtect(lpAddress, nSize, OldProtect, &OldProtect2);
}

// ==========================================================================
// Read from the process memory.
//	lpAddress:	address to read from
//	lpBuffer:	buffer to read data into
//	nSize:		size of data to read
// ==========================================================================
void ReadMem(LPVOID lpAddress, LPVOID lpBuffer, SIZE_T nSize)
{
	DWORD OldProtect, OldProtect2;
	VirtualProtect(lpAddress, nSize, PAGE_READWRITE, &OldProtect);
	memcpy(lpBuffer, lpAddress, nSize);
	VirtualProtect(lpAddress, nSize, OldProtect, &OldProtect2);
}