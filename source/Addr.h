#pragma once

#include "Main.h"

enum DllAddress {
	kernel32_CreateRemoteThread,
    kernelbase_CreateRemoteThreadEx,
	CStringTUni_Constr,
	CStringTUni_Equals,
	CFileSystem_SetLookUpOrder,
	CFormatter_Insert_ulong,
	CFormatter_Insert_ushort,
	CFormatter_Insert_float,
	CFormatter_Insert_pwchar,
	CGameOptionMgr_ReadFSAA,
	CGameOptionMgr_ReadFSAAQuality,
	CPleione_CleanUp,
	CAtmosphere_SetSkyTime,
	CCameraControl_SetCamera,
	CDungeonRegion_SetFog,
	IParameterBase2_GetCombatPower,
	IItem_GetInterfaceDurability,
	IItem_GetInterfaceDurabilityMax,
	IItem_GetColor,
	ICharacter_IsElf,
	ICharacter_IsGiant,
	ICharacter_IsPet,
	ICharacter_IsNPC,
	ICharacter_IsGoodNPC,
	CDefaultCompiler_Compile,
	CDefaultTagCompiler_Compile,
	CHTMLCompiler_Compile,
	IParameterBase2_GetExploLevel,
	IParameter_GetExploExpPercent,
	DllAddress_size // equal to the number of enum entries previously
};

class CAddr
{
public:
	static LPBYTE Addr( DllAddress addrNumber );
	static bool PreCacheAddresses( void );
private:
	static bool CacheAddress( DllAddress addrNumber );
};