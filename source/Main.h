#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <process.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "wchar.h"
#include <tchar.h>
#include <basetsd.h>
#include <stdarg.h>
#include "Aclapi.h"
#include "AccCtrl.h"

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <iterator>
#include <stack>
#include <fstream>

#include <assert.h>

using namespace std;

#define NAKED __declspec(naked)

typedef unsigned int uint;
typedef unsigned long ulong;
typedef __int64 int64;
typedef wchar_t wchar;

void WriteLog( const char* format, ... );
void DebugLog( const char* format, ... );

void WriteMem(LPVOID lpAddress, LPVOID lpBuffer, SIZE_T nSize);
void ReadMem(LPVOID lpAddress, LPVOID lpBuffer, SIZE_T nSize);