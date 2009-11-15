

typedef struct _USER_POWER_POLICY{
    ULONG                   Revision;       // 1


    // "system idle" detection
    POWER_ACTION_POLICY     IdleAc;
    POWER_ACTION_POLICY     IdleDc;
    ULONG                   IdleTimeoutAc;
    ULONG                   IdleTimeoutDc;
    UCHAR                   IdleSensitivityAc;
    UCHAR                   IdleSensitivityDc;
    
    // Throttling Policy
    UCHAR                   ThrottlePolicyAc;
    UCHAR                   ThrottlePolicyDc;

    // meaning of power action "sleep"
    SYSTEM_POWER_STATE      MaxSleepAc;
    SYSTEM_POWER_STATE      MaxSleepDc;

    // For future use
    ULONG                   Reserved[2];

    // video policies
    ULONG                   VideoTimeoutAc;
    ULONG                   VideoTimeoutDc;

    // hard disk policies
    ULONG                   SpindownTimeoutAc;
    ULONG                   SpindownTimeoutDc;

    // processor policies
    BOOLEAN                 OptimizeForPowerAc;
    BOOLEAN                 OptimizeForPowerDc;
    UCHAR                   FanThrottleToleranceAc;
    UCHAR                   FanThrottleToleranceDc;
    UCHAR                   ForcedThrottleAc;
    UCHAR                   ForcedThrottleDc;
    
} USER_POWER_POLICY, *PUSER_POWER_POLICY;

// Registry storage structures for the POWER_POLICY data. There are three
// structures, MACHINE_POWER_POLICY, MACHINE_PROCESSOR_POWER_POLICY and USER_POWER_POLICY. the
// MACHINE_POWER_POLICY stores per machine data for which there is no UI.
// USER_POWER_POLICY stores the per user data.

typedef struct _MACHINE_POWER_POLICY{
    ULONG                   Revision;       // 1

    // meaning of power action "sleep"
    SYSTEM_POWER_STATE      MinSleepAc;
    SYSTEM_POWER_STATE      MinSleepDc;
    SYSTEM_POWER_STATE      ReducedLatencySleepAc;
    SYSTEM_POWER_STATE      ReducedLatencySleepDc;

    // parameters for dozing
    ULONG                   DozeTimeoutAc;
    ULONG                   DozeTimeoutDc;
    ULONG                   DozeS4TimeoutAc;
    ULONG                   DozeS4TimeoutDc;

    // processor policies
    UCHAR                   MinThrottleAc;
    UCHAR                   MinThrottleDc;
    UCHAR                   pad1[2];
    POWER_ACTION_POLICY     OverThrottledAc;
    POWER_ACTION_POLICY     OverThrottledDc;

} MACHINE_POWER_POLICY, *PMACHINE_POWER_POLICY;

typedef struct _POWER_POLICY{
    USER_POWER_POLICY       user;
    MACHINE_POWER_POLICY    mach;
} POWER_POLICY, *PPOWER_POLICY;
