#include "FileSystem.h"

namespace Platform
{
	//------------------------------------------------------------------------------------------------------------------------------------------------------------
	wstring GetCurPath()   
	{   
		wstring buff,path;
		wchar_t b[MAX_PATH];
		GetModuleFileNameW(NULL,b,MAX_PATH); 
		buff=b;
		path=buff.substr(0,buff.rfind(L'\\'));
		return(path);  
	}

	string GetCurPathA()
	{
		string buff,path;
		char b[MAX_PATH];
		GetModuleFileNameA(NULL,b,MAX_PATH);
		buff=b;
		path=buff.substr(0,buff.rfind('\\'));
		return(path);
	}

	wstring GetSysDir()
	{
		wchar_t buff[255];
		int i=GetSystemDirectory(buff,255);
		if(i!=0)
		{
			return(buff);
		}
		else
		{
			return(L"");
		}

	}
	//-------------------------------------------------------------------------------------------------------------------------------------------------------
	



};