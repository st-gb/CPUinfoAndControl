#pragma once //include guard

//Following function pointer types equal the PwrProf API except that they
//are function pointers ("WINAPI *").

typedef DWORD (WINAPI * pfnPowerDeleteScheme) (
  __in_opt  HKEY RootPowerKey,
  __in      const GUID *SchemeGuid
);

typedef DWORD (WINAPI * pfnPowerDuplicateScheme) (
  __in_opt  HKEY RootPowerKey,
  __in      const GUID *SourceSchemeGuid,
  __in      GUID **DestinationSchemeGuid
);

typedef DWORD (WINAPI * pfnPowerEnumerate) (
  __in_opt   HKEY RootPowerKey,
  __in_opt   const GUID *SchemeGuid,
  __in_opt   const GUID *SubGroupOfPowerSettingsGuid,
  __in       POWER_DATA_ACCESSOR AccessFlags,
  __in       ULONG Index,
  __out_opt  UCHAR *Buffer,
  __inout    DWORD *BufferSize
);

typedef DWORD (WINAPI * pfnPowerGetActiveScheme) (
      __in_opt HKEY UserRootPowerKey,
      __deref_out GUID **ActivePolicyGuid
      ) ;

typedef DWORD (WINAPI * pfnPowerReadACValueIndex) (
  __in_opt HKEY RootPowerKey,
  __in_opt CONST GUID *SchemeGuid,
  __in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
  __in_opt CONST GUID *PowerSettingGuid,
  __out LPDWORD AcValueIndex
  ) ;

typedef DWORD ( WINAPI * pfnPowerReadFriendlyName) (
  __in_opt   HKEY RootPowerKey,
  __in_opt   const GUID *SchemeGuid,
  __in_opt   const GUID *SubGroupOfPowerSettingsGuid,
  __in_opt   const GUID *PowerSettingGuid,
  __out_opt  PUCHAR Buffer,
  __inout    LPDWORD BufferSize
);

typedef DWORD ( WINAPI * pfnPowerSetActiveScheme) (
  __in_opt  HKEY UserRootPowerKey,
  __in      const GUID *SchemeGuid
);

typedef DWORD (WINAPI * pfnPowerSettingAccessCheck) (
  __in      POWER_DATA_ACCESSOR AccessFlags,
  __in_opt  const GUID *PowerGuid
);

typedef DWORD (WINAPI * pfnPowerWriteACValueIndex) (
  __in_opt  HKEY RootPowerKey,
  __in      const GUID *SchemeGuid,
  __in_opt  const GUID *SubGroupOfPowerSettingsGuid,
  __in_opt  const GUID *PowerSettingGuid,
  __in      DWORD DcValueIndex
  );
typedef DWORD (WINAPI * pfnPowerWriteFriendlyName) (
  __in_opt  HKEY RootPowerKey,
  __in      const GUID *SchemeGuid,
  __in_opt  const GUID *SubGroupOfPowerSettingsGuid,
  __in_opt  const GUID *PowerSettingGuid,
  __in      UCHAR *Buffer,
  __in      DWORD BufferSize
);
