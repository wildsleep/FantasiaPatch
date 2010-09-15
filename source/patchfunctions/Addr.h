#pragma once

#include "..\main.h"

enum DllAddress {
	CStringTUni_Constr,
	CStringTUni_Equals,
	CFileSystem_SetLookUpOrder,
	CFormatter_Insert,
	CGameOptionMgr_ReadFSAA,
	CGameOptionMgr_ReadFSAAQuality,
	CPleione_CleanUp,
	CAtmosphere_SetSkyTime,
	CCameraControl_SetCamera,
	CDungeonRegion_SetFog,
	IParameterBase2_GetCombatPower,
	IItem_GetInterfaceDurability,
	IItem_GetInterfaceDurabilityMax,
	ICharacter_IsElf,
	ICharacter_IsGiant,
	ICharacter_IsPet,
	ICharacter_IsNPC,
	ICharacter_IsGoodNPC,
	CDefaultCompiler_Compile,
	CDefaultTagCompiler_Compile,
	CHTMLCompiler_Compile,
	DllAddress_size // equal to the number of enum entries previously
};

class CAddr
{
public:
	static LPBYTE Addr( DllAddress addrNumber );
private:
	static bool CacheAddress( DllAddress addrNumber );
};