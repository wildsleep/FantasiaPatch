#pragma once
#include "Main.h"
#include "Singleton.h"

typedef void (*IJL_VOID)(void);

class CIJLHook
{
public:
	CIJLHook() { hIJL = NULL; }
	~CIJLHook();
	bool Hook();
	void SetSSQuality(int quality);
	int GetSSQuality(void);
private:
	HMODULE hIJL;
};
typedef Singleton<CIJLHook> SngIJLHook;

// IJL.dll exports
void ijlGetLibVersion();
void ijlInit();
void ijlFree();
void ijlRead();
void ijlWrite();
void ijlErrorStr();