#include "Addr.h"
#include "FileSystem.h"

static LPBYTE DllAddresses[DllAddress_size];

LPBYTE CAddr::Addr( DllAddress addrNumber )
{
	if ( DllAddresses[addrNumber] != NULL )
		return DllAddresses[addrNumber];
	if ( CacheAddress( addrNumber ))
		return DllAddresses[addrNumber];
	return NULL;
}

bool CAddr::PreCacheAddresses( void )
{
	bool result = true;
	for (int i=0; i<DllAddress_size; i++)
		result &= CacheAddress( (DllAddress)i );

	if (result)
		WriteLog("Caching of addresses successful.\n");
	else
		WriteLog("Warning: failed to cache some addresses.\n");

	return result;
}

bool CAddr::CacheAddress( DllAddress addrNumber )
{
	wchar_t *dllfile;
	char *dllfunction;
	char *fullname;

	switch (addrNumber) {

		// kernel32.dll

		case kernel32_CreateRemoteThread:
			dllfile = L"kernel32.dll";
			fullname = "CRT";
			dllfunction = "CreateRemoteThread";
			break;

        // kernelbase.dll

		case kernelbase_CreateRemoteThreadEx:
			dllfile = L"KERNELBASE.dll";
			fullname = "CRTX";
			dllfunction = "CreateRemoteThreadEx";
			break;

		// esl.dll

		case CStringTUni_Constr:
			dllfile = L"esl.dll"; 
			fullname = "esl::CStringT::CStringT(void)";
			dllfunction = "??0?$CStringT@_WVunicode_string_trait@esl@@Vunicode_string_implement@2@@esl@@QAE@XZ";
			break;
		case CStringTUni_Equals:
			dllfile = L"esl.dll";
			fullname = "esl::CStringT::operator=(wchar_t *)";
			dllfunction = "??4?$CStringT@_WVunicode_string_trait@esl@@Vunicode_string_implement@2@@esl@@QAEAAV01@PB_W@Z";
			break;
		case CFileSystem_SetLookUpOrder:
			dllfile = L"esl.dll";
			fullname = "esl::CFileSystem::SetLookUpOrder";
			dllfunction = "?SetLookUpOrder@CFileSystem@esl@@QAEXW4EFileLookUpOrder@esl_constant@2@@Z";
			break;
		case CFormatter_Insert_ulong:
			dllfile = L"esl.dll";
			fullname = "esl::CFormatter::operator<<(ulong)";
			dllfunction = "??6CFormatter@esl@@QAEAAV01@K@Z";
			break;
		case CFormatter_Insert_ushort:
			dllfile = L"esl.dll";
			fullname = "esl::CFormatter::operator<<(ushort)";
			dllfunction = "??6CFormatter@esl@@QAEAAV01@G@Z";
			break;
		case CFormatter_Insert_float:
			dllfile = L"esl.dll";
			fullname = "esl::CFormatter::operator<<(float)";
			dllfunction = "??6CFormatter@esl@@QAEAAV01@M@Z";
			break;
		case CFormatter_Insert_pwchar:
			dllfile = L"esl.dll";
			fullname = "esl::CFormatter::operator<<(wchar_t const *)";
			dllfunction = "??6CFormatter@esl@@QAEAAV01@PB_W@Z";
			break;

		// pleione.dll

		case CGameOptionMgr_ReadFSAA:
			dllfile = L"pleione.dll";
			fullname = "pleione::CGameOptionMgr::ReadFSAA";
			dllfunction = "?ReadFSAA@CGameOptionMgr@pleione@@QAE?AW4EFSAA@pleione_constant@2@XZ";
			break;
		case CGameOptionMgr_ReadFSAAQuality:
			dllfile = L"pleione.dll";
			fullname = "pleione::CGameOptionMgr::ReadFSAAQuality";
			dllfunction = "?ReadFSAAQuality@CGameOptionMgr@pleione@@QAEKXZ";
			break;
		case CPleione_CleanUp:
			dllfile = L"pleione.dll";
			fullname = "pleione::CPleione::CleanUp";
			dllfunction = "?CleanUp@CPleione@pleione@@QAE_NXZ";
			break;

		// renderer2.dll

		case CAtmosphere_SetSkyTime:
			dllfile = L"renderer2.dll";
			fullname = "pleione::CAtmosphere::SetSkyTime";
			dllfunction = "?SetSkyTime@CAtmosphere@pleione@@QAEXM@Z";
			break;
		case CCameraControl_SetCamera:
			dllfile = L"renderer2.dll";
			fullname = "pleione::CCameraControl::SetCamera";
			dllfunction = "?SetCamera@CCameraControl@pleione@@QAEXPAVCScene@2@PAVITerrain@2@KFF@Z";
			break;
		case CDefaultCompiler_Compile:
			dllfile = L"Renderer2.dll";
			fullname = "pleione::CDefaultCompiler::Compile";
			dllfunction = "?Compile@CDefaultCompiler@pleione@@UAEPAVCCompiledText@2@ABV?$CStringT@_WVunicode_string_trait@esl@@Vunicode_string_implement@2@@esl@@ABUtext_option@2@@Z";
			break;
		case CDefaultTagCompiler_Compile:
			dllfile = L"Renderer2.dll";
			fullname = "pleione::CDefaultTagCompiler::Compile";
			dllfunction = "?Compile@CDefaultTagCompiler@pleione@@UAEPAVCCompiledText@2@ABV?$CStringT@_WVunicode_string_trait@esl@@Vunicode_string_implement@2@@esl@@ABUtext_option@2@@Z";
			break;
		case CHTMLCompiler_Compile:
			dllfile = L"Renderer2.dll";
			fullname = "pleione::CHTMLCompiler::Compile";
			dllfunction = "?Compile@CHTMLCompiler@pleione@@UAEPAVCCompiledText@2@ABV?$CStringT@_WVunicode_string_trait@esl@@Vunicode_string_implement@2@@esl@@ABUtext_option@2@@Z";
			break;

		// standard.dll

		case CDungeonRegion_SetFog:
			dllfile = L"standard.dll";
			fullname = "core::CDungeonRegion::SetFog";
			dllfunction = "?SetFog@CDungeonRegion@core@@QAEX_N@Z";
			break;
		case IParameterBase2_GetCombatPower:
			dllfile = L"standard.dll";
			fullname = "core::IParameterBase2::GetCombatPower";
			dllfunction = "?GetCombatPower@IParameterBase2@core@@QBEMXZ";
			break;
		case IItem_GetInterfaceDurability:
			dllfile = L"standard.dll";
			fullname = "core::IItem::GetInterfaceDurability";
			dllfunction = "?GetInterfaceDurability@IItem@core@@QBEKXZ";
			break;
		case IItem_GetInterfaceDurabilityMax:
			dllfile = L"standard.dll";
			fullname = "core::IItem::GetInterfaceDurabilityMax";
			dllfunction = "?GetInterfaceDurabilityMax@IItem@core@@QBEKXZ";
			break;
		case IItem_GetColor:
			dllfile = L"standard.dll";
			fullname = "core::IItem::GetColor";
			dllfunction = "?GetColor@IItem@core@@QBEKK@Z";
			break;
		case ICharacter_IsElf:
			dllfile = L"standard.dll";
			fullname = "core::ICharacter::IsElf";
			dllfunction = "?IsElf@ICharacter@core@@QBE_NXZ";
			break;
		case ICharacter_IsGiant:
			dllfile = L"standard.dll";
			fullname = "core::ICharacter::IsGiant";
			dllfunction = "?IsGiant@ICharacter@core@@QBE_NXZ";
			break;
		case ICharacter_IsPet:
			dllfile = L"standard.dll";
			fullname = "core::ICharacter::IsPet";
			dllfunction = "?IsPet@ICharacter@core@@QBE_NXZ";
			break;
		case ICharacter_IsNPC:
			dllfile = L"standard.dll";
			fullname = "core::ICharacter::IsNPC";
			dllfunction = "?IsNPC@ICharacter@core@@QBE_NXZ";
			break;
		case ICharacter_IsGoodNPC:
			dllfile = L"standard.dll";
			fullname = "core::ICharacter::IsGoodNPC";
			dllfunction = "?IsGoodNPC@ICharacter@core@@QBE_NXZ";
			break;
		case IParameterBase2_GetExploLevel:
			dllfile = L"standard.dll";
			fullname = "core::IParameterBase2::GetExploLevel";
			dllfunction = "?GetExploLevel@IParameterBase2@core@@QBEGXZ";
			break;
		case IParameter_GetExploExpPercent:
			dllfile = L"standard.dll";
			fullname = "core::IParameter::GetExploExpPercent";
			dllfunction = "?GetExploExpPercent@IParameter@core@@QBEMXZ";
			break;
		default:
			return false;
	}

	HMODULE dllmodule = GetModuleHandle( dllfile );
	LPBYTE address = (LPBYTE)GetProcAddress( dllmodule, dllfunction );
	if(address==NULL) {
		DebugLog("Error: Could not find %s in %ls.\n", fullname, dllfile);
		DllAddresses[addrNumber] = NULL;
		return false;
	}
	DebugLog("Success: Found %s in %ls.\n", fullname, dllfile);
	DllAddresses[addrNumber] = address;
	return true;
}