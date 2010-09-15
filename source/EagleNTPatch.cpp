#include "EagleNTPatch.h"

bool SetRegistryPermissions(LPTSTR pObjectName,
							LPTSTR pTrusteeName,
							ACCESS_MODE AccessMode)
{
	bool return_val = true;
	PACL pOldAcl;
	PACL pNewAcl;
	PSECURITY_DESCRIPTOR pSecurityDescriptor;
	EXPLICIT_ACCESS ExplicitAccess;

	int result;
	result = GetNamedSecurityInfo(
		pObjectName,
		SE_REGISTRY_KEY,
		DACL_SECURITY_INFORMATION,
		NULL,
		NULL,
		&pOldAcl,
		NULL,
		&pSecurityDescriptor);
	if (result != ERROR_SUCCESS) {
		WriteLog("ERROR: GetNamedSecurityInfo call failed.\n");
		return_val = false;
		goto cleanup;
	}
	
	BuildExplicitAccessWithName(
		&ExplicitAccess,
		pTrusteeName,
		KEY_ALL_ACCESS,
		AccessMode,
		SUB_CONTAINERS_AND_OBJECTS_INHERIT);
	result = SetEntriesInAcl(
		1,
		&ExplicitAccess,
		pOldAcl,
		&pNewAcl);
	if (result != ERROR_SUCCESS) {
		WriteLog("ERROR: SetEntriesInAcl call failed.\n");
		return_val = false;
		goto cleanup;
	}

	result = SetNamedSecurityInfo(
		pObjectName,
		SE_REGISTRY_KEY,
		DACL_SECURITY_INFORMATION,
		NULL,
		NULL,
		pNewAcl,
		NULL);
	if (result != ERROR_SUCCESS) {
		WriteLog("ERROR: SetNamedSecurityInfo call failed.\n");
		return_val = false;
		goto cleanup;
	}
	
cleanup:
	if (pSecurityDescriptor)
		LocalFree(pSecurityDescriptor);
	if (pOldAcl)
		LocalFree(pOldAcl);
	if (pNewAcl)
		LocalFree(pNewAcl);
	return return_val;
}

bool EagleNTPatchLoad(void)
{
	bool result = true;
	result &= SetRegistryPermissions(
		_T("MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EagleNT"),
		_T("EVERYONE"),
		DENY_ACCESS);
	result &= SetRegistryPermissions(
		_T("MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EagleNT"),
		_T("ADMINISTRATORS"),
		DENY_ACCESS);
	result &= SetRegistryPermissions(
		_T("MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EagleNT"),
		_T("SYSTEM"),
		DENY_ACCESS);
	result &= SetRegistryPermissions(
		_T("MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EagleNT"),
		_T("CREATOR OWNER"),
		DENY_ACCESS);
	if (result) {
		WriteLog("Registry patch succeeded.\n");
		return true;
	} else {
		WriteLog("Registry patch failed.\n");
		return false;
	}
}

bool EagleNTPatchUnload(void)
{
	bool result = true;
	result &= SetRegistryPermissions(
		_T("MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EagleNT"),
		_T("EVERYONE"),
		SET_ACCESS);
	result &= SetRegistryPermissions(
		_T("MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EagleNT"),
		_T("ADMINISTRATORS"),
		SET_ACCESS);
	result &= SetRegistryPermissions(
		_T("MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EagleNT"),
		_T("SYSTEM"),
		SET_ACCESS);
	result &= SetRegistryPermissions(
		_T("MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EagleNT"),
		_T("CREATOR OWNER"),
		SET_ACCESS);
	if (result) {
		WriteLog("Registry patch unload succeeded.\n");
		return true;
	} else {
		WriteLog("Registry patch unload failed.\n");
		return false;
	}
}