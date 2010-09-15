#include "Addr.h"

static LPBYTE DllAddresses[DllAddress_size];

LPBYTE CAddr::Addr( DllAddress addrNumber )
{
	if ( DllAddresses[addrNumber] != NULL )
		return DllAddresses[addrNumber];
	if ( CacheAddress( addrNumber ))
		return DllAddresses[addrNumber];
	return NULL;
}

bool CAddr::CacheAddress( DllAddress addrNumber )
{
	wchar_t *dllfile;
	char *dllfunction;
	char *fullname;

	switch (addrNumber) {
		case CStringTUni_Constr:
			dllfile = _T("esl.dll");
			fullname = "esl::CStringT::CStringT";
			dllfunction = "??0?$CStringT@_WVunicode_string_trait@esl@@Vunicode_string_implement@2@@esl@@QAE@XZ";
			break;
		case CStringTUni_Equals:
			dllfile = _T("esl.dll");
			fullname = "esl::CStringT::operator=";
			dllfunction = "??4?$CStringT@_WVunicode_string_trait@esl@@Vunicode_string_implement@2@@esl@@QAEAAV01@PB_W@Z";
			break;
		case CFileSystem_SetLookUpOrder:
			dllfile = _T("esl.dll");
			fullname = "esl::CFileSystem::SetLookUpOrder";
			dllfunction = "?SetLookUpOrder@CFileSystem@esl@@QAEXW4EFileLookUpOrder@esl_constant@2@@Z";
			break;
		case CFormatter_Insert:
			dllfile = _T("esl.dll");
			fullname = "esl::CFormatter::operator<<";
			dllfunction = "??6CFormatter@esl@@QAEAAV01@K@Z";
			break;
		case CGameOptionMgr_ReadFSAA:
			dllfile = _T("pleione.dll");
			fullname = "pleione::CGameOptionMgr::ReadFSAA";
			dllfunction = "?ReadFSAA@CGameOptionMgr@pleione@@QAE?AW4EFSAA@pleione_constant@2@XZ";
			break;
		case CGameOptionMgr_ReadFSAAQuality:
			dllfile = _T("pleione.dll");
			fullname = "pleione::CGameOptionMgr::ReadFSAAQuality";
			dllfunction = "?ReadFSAAQuality@CGameOptionMgr@pleione@@QAEKXZ";
			break;
		case CPleione_CleanUp:
			dllfile = _T("pleione.dll");
			fullname = "pleione::CPleione::CleanUp";
			dllfunction = "?CleanUp@CPleione@pleione@@QAE_NXZ";
			break;
		case CAtmosphere_SetSkyTime:
			dllfile = _T("renderer2.dll");
			fullname = "pleione::CAtmosphere::SetSkyTime";
			dllfunction = "?SetSkyTime@CAtmosphere@pleione@@QAEXM@Z";
			break;
		case CCameraControl_SetCamera:
			dllfile = _T("renderer2.dll");
			fullname = "pleione::CCameraControl::SetCamera";
			dllfunction = "?SetCamera@CCameraControl@pleione@@QAEXPAVCScene@2@PAVITerrain@2@KFF@Z";
			break;
		case CDungeonRegion_SetFog:
			dllfile = _T("standard.dll");
			fullname = "core::CDungeonRegion::SetFog";
			dllfunction = "?SetFog@CDungeonRegion@core@@QAEX_N@Z";
			break;
		case IParameterBase2_GetCombatPower:
			dllfile = _T("standard.dll");
			fullname = "core::IParameterBase2::GetCombatPower";
			dllfunction = "?GetCombatPower@IParameterBase2@core@@QBEMXZ";
			break;
		case IItem_GetInterfaceDurability:
			dllfile = _T("standard.dll");
			fullname = "core::IItem::GetInterfaceDurability";
			dllfunction = "?GetInterfaceDurability@IItem@core@@QBEKXZ";
			break;
		case IItem_GetInterfaceDurabilityMax:
			dllfile = _T("standard.dll");
			fullname = "core::IItem::GetInterfaceDurabilityMax";
			dllfunction = "?GetInterfaceDurabilityMax@IItem@core@@QBEKXZ";
			break;
		case ICharacter_IsElf:
			dllfile = _T("standard.dll");
			fullname = "core::ICharacter::IsElf";
			dllfunction = "?IsElf@ICharacter@core@@QBE_NXZ";
			break;
		case ICharacter_IsGiant:
			dllfile = _T("standard.dll");
			fullname = "core::ICharacter::IsGiant";
			dllfunction = "?IsGiant@ICharacter@core@@QBE_NXZ";
			break;
		case ICharacter_IsPet:
			dllfile = _T("standard.dll");
			fullname = "core::ICharacter::IsPet";
			dllfunction = "?IsPet@ICharacter@core@@QBE_NXZ";
			break;
		case ICharacter_IsNPC:
			dllfile = _T("standard.dll");
			fullname = "core::ICharacter::IsNPC";
			dllfunction = "?IsNPC@ICharacter@core@@QBE_NXZ";
			break;
		case ICharacter_IsGoodNPC:
			dllfile = _T("standard.dll");
			fullname = "core::ICharacter::IsGoodNPC";
			dllfunction = "?IsGoodNPC@ICharacter@core@@QBE_NXZ";
			break;
		case CDefaultCompiler_Compile:
			dllfile = _T("Renderer2.dll");
			fullname = "pleione::CDefaultCompiler::Compile";
			dllfunction = "?Compile@CDefaultCompiler@pleione@@UAEPAVCCompiledText@2@ABV?$CStringT@_WVunicode_string_trait@esl@@Vunicode_string_implement@2@@esl@@ABUtext_option@2@@Z";
			break;
		case CDefaultTagCompiler_Compile:
			dllfile = _T("Renderer2.dll");
			fullname = "pleione::CDefaultTagCompiler::Compile";
			dllfunction = "?Compile@CDefaultTagCompiler@pleione@@UAEPAVCCompiledText@2@ABV?$CStringT@_WVunicode_string_trait@esl@@Vunicode_string_implement@2@@esl@@ABUtext_option@2@@Z";
			break;
		case CHTMLCompiler_Compile:
			dllfile = _T("Renderer2.dll");
			fullname = "pleione::CHTMLCompiler::Compile";
			dllfunction = "?Compile@CHTMLCompiler@pleione@@UAEPAVCCompiledText@2@ABV?$CStringT@_WVunicode_string_trait@esl@@Vunicode_string_implement@2@@esl@@ABUtext_option@2@@Z";
			break;
		default:
			return false;
	}

	LPBYTE address = (LPBYTE)GetProcAddress( GetModuleHandle( dllfile ), dllfunction );
	if(address==NULL) {
		DebugLog("Error: Could not find %s in %ls.\n", fullname, dllfile);
		DllAddresses[addrNumber] = NULL;
		return false;
	}
	DebugLog("Success: Found %s in %ls.\n", fullname, dllfile);
	DllAddresses[addrNumber] = address;
	return true;
}