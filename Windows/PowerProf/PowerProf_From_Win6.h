#pragma once 

#if (NTDDI_VERSION >= NTDDI_VISTA)

//#define DEFINE_POWER_DATA_ACCESSOR
#ifdef DEFINE_POWER_DATA_ACCESSOR
//
// Enum which defines which field inside of a
// power setting is being accessed.
//
typedef enum _POWER_DATA_ACCESSOR {
        
        //
        // Used by read/write and enumeration engines
        //
        ACCESS_AC_POWER_SETTING_INDEX = 0,
        ACCESS_DC_POWER_SETTING_INDEX,
        ACCESS_FRIENDLY_NAME,
        ACCESS_DESCRIPTION,
        ACCESS_POSSIBLE_POWER_SETTING,
        ACCESS_POSSIBLE_POWER_SETTING_FRIENDLY_NAME,
        ACCESS_POSSIBLE_POWER_SETTING_DESCRIPTION,
        ACCESS_DEFAULT_AC_POWER_SETTING,
        ACCESS_DEFAULT_DC_POWER_SETTING,
        ACCESS_POSSIBLE_VALUE_MIN,
        ACCESS_POSSIBLE_VALUE_MAX,
        ACCESS_POSSIBLE_VALUE_INCREMENT,
        ACCESS_POSSIBLE_VALUE_UNITS,
        ACCESS_ICON_RESOURCE,
        ACCESS_DEFAULT_SECURITY_DESCRIPTOR,
        ACCESS_ATTRIBUTES,
        
        //
        // Used by enumeration engine.
        //
        ACCESS_SCHEME,
        ACCESS_SUBGROUP,
        ACCESS_INDIVIDUAL_SETTING,
        
        //
        // Used by access check
        //
        ACCESS_ACTIVE_SCHEME,
        ACCESS_CREATE_SCHEME
        
} POWER_DATA_ACCESSOR, *PPOWER_DATA_ACCESSOR;
#endif //#if (NTDDI_VERSION >= NTDDI_VISTA)

#endif //#ifdef DEFINE_POWER_DATA_ACCESSOR

//Avoid MSVC warning "warning C4005: 'DEFINE_GUID': Makro-Neudefinition"
//(it is also defined in "guiddef.h"
#ifndef DEFINE_GUID
  //// from Windows 6.1 Platform SDK's "winnt.h" :
  //#ifdef INITGUID
  #define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
          EXTERN_C const GUID DECLSPEC_SELECTANY name \
                  = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#else
  //#include <guiddef.h>
  //g++ treats file names case sensitive. "Guiddef.h" should be
  //adequate for g++ "Guiddef.h" and MS compiler ("guiddef.h").
  ////MinGW already has the defines in objbase.h etc.
  #if !defined(__MINGW32__) && (__GNUC__<4)
    #include <Guiddef.h>
  #endif
#endif //#ifndef DEFINE_GUID
//#else
//Use C comment, else compiler warning: multi-line comment because of "\" at
// line end.
/*#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
//    EXTERN_C const GUID FAR name
//#endif // INITGUID */

//// Specifies the subgroup which will contain all of the processor
//// settings for a single policy.
//DEFINE_GUID( GUID_PROCESSOR_SETTINGS_SUBGROUP, 0x54533251, 0x82BE, 0x4824, 0x96, 0xC1, 0x47, 0xB6, 0x0B, 0x74, 0x0D, 0x00 );
//
//// Specifies a percentage (between 0 and 100) that the processor frequency
//// should never go above.  For example, if this value is set to 80, then
//// the processor frequency will never be throttled above 80 percent of its
//// maximum frequency by the system.
//DEFINE_GUID( GUID_PROCESSOR_THROTTLE_MAXIMUM, 0xBC5038F7, 0x23E0, 0x4960, 0x96, 0xDA, 0x33, 0xAB, 0xAF, 0x59, 0x35, 0xEC );
//
//// Specifies a percentage (between 0 and 100) that the processor frequency
//// should not drop below.  For example, if this value is set to 50, then the
//// processor frequency will never be throttled below 50 percent of its
//// maximum frequency by the system.
//DEFINE_GUID( GUID_PROCESSOR_THROTTLE_MINIMUM, 0x893DEE8E, 0x2BEF, 0x41E0, 0x89, 0xC6, 0xB5, 0x5D, 0x09, 0x29, 0x96, 0x4C );

