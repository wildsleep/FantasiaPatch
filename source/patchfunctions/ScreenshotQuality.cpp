#include "patch.h"

static const wchar_t *patchname = _T("ScreenshotQuality");

bool CPatch::ScreenshotQuality(void)
{
	ssQuality = ReadINI_Int(patchname, 1, 100, 75);
	return true;
}

// The remainder of this mod is implemented in HookIJL.cpp -> ijlWrite()