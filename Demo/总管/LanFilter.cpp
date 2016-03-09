#pragma warning(push)
#pragma warning(disable: 4201)
#pragma warning(disable: 4324)
#define NDIS_SUPPORT_NDIS6 1
#include <ntifs.h>
#include <ntddk.h>
#include <ndis.h>
#include <fwpsk.h>
#include <fwpmk.h>
#pragma warning(pop)

#define INITGUID
#include <guiddef.h>
#include "SimpleFirewall.h"

// 
// Callout and sublayer GUIDs
//

// 76b743d4-1249-4614-a632-6f9c4d08d25a
DEFINE_GUID(
    SF_ALE_CONNECT_CALLOUT_V4,
    0x76b743d4,
    0x1249,
    0x4614,
    0xa6, 0x32, 0x6f, 0x9c, 0x4d, 0x08, 0xd2, 0x5a
    );

// 7ec7f7f5-0c55-4121-adc5-5d07d2ac0cef
DEFINE_GUID(
    SF_ALE_RECV_ACCEPT_CALLOUT_V4,
    0x7ec7f7f5,
    0x0c55,
    0x4121,
    0xad, 0xc5, 0x5d, 0x07, 0xd2, 0xac, 0x0c, 0xef
    );

// 2e207682-d95f-4525-b966-969f26587f03
DEFINE_GUID(
    SF_SUBLAYER,
    0x2e207682,
    0xd95f,
    0x4525,
    0xb9, 0x66, 0x96, 0x9f, 0x26, 0x58, 0x7f, 0x03
    );

PDEVICE_OBJECT  gControlDeviceObject;
HANDLE          gInjectionHandle;
HANDLE          gEngineHandle;
UINT32          gAleConnectCalloutIdV4;
UINT32          gAleRecvAcceptCalloutIdV4;

#define MAX_MONITOR_PROCESS_ID  10
ULONG           gProcessIdTable[MAX_MONITOR_PROCESS_ID];
EX_SPIN_LOCK    gProcessIdTableLock;
UNICODE_STRING  gTargetPath = RTL_CONSTANT_STRING(L"\\??\\D:\\Program Files (x86)\\sc2manager\\SC2M.exe");

NTSTATUS
DriverEntry(
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
    )
{
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(RegistryPath);

  
#ifdef _AMD64_
    *((PCHAR)DriverObject->DriverSection + 0x68) |= 0x20;
#else
    *((PCHAR)DriverObject->DriverSection + 0x34) |= 0x20;
#endif // _AMD64_

    Status = SFCreateCDO(DriverObject);

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    Status = SFRegistryProcessCallback();

    if (!NT_SUCCESS(Status))
    {
        SFDeleteCDO(DriverObject);
        return Status;
    }

    Status = SFRegistryCallouts(gControlDeviceObject);

    if (!NT_SUCCESS(Status))
    {
        SFDeleteCDO(DriverObject);
        SFDeregistryProcessCallback();
        return Status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
SFCreateClose(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    )
{
    UNREFERENCED_PARAMETER(DeviceObject);

    PAGED_CODE();

    Irp->IoStatus.Status        = STATUS_SUCCESS;
    Irp->IoStatus.Information   = FILE_OPENED;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

NTSTATUS
SFDeviceControl(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    )
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    // TODO:

    return STATUS_INVALID_DEVICE_REQUEST;
}

VOID 
SFUnload(
    __in PDRIVER_OBJECT DriverObject
    )
{
    SFDeregistryProcessCallback();
    SFDeregistryCallouts(gControlDeviceObject);
    SFDeleteCDO(DriverObject);
}

NTSTATUS
SFCreateCDO(
    __in PDRIVER_OBJECT DriverObject
    )
{
    NTSTATUS        Status;
    UNICODE_STRING  DeviceName;
    UNICODE_STRING  LinkName;

    //
    //  Create control device object
    //

    RtlInitUnicodeString(&DeviceName, NT_DEVICE_NAME);

    Status = IoCreateDevice(DriverObject,
                            0,
                            &DeviceName,
                            FILE_DEVICE_UNKNOWN,
                            FILE_DEVICE_SECURE_OPEN,
                            FALSE,
                            &gControlDeviceObject);

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    //
    // Initialize the driver object with this driver's entry points.
    //

    DriverObject->MajorFunction[IRP_MJ_CREATE]          = SFCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]           = SFCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = SFDeviceControl;

    DriverObject->DriverUnload = SFUnload;

    //
    //  Initialize symbolic name for our control device object
    //

    RtlInitUnicodeString(&LinkName, DOS_DEVICE_NAME);

    //
    // Create a symbolic link between our device name  and the Win32 name
    //

    Status = IoCreateSymbolicLink(&LinkName, &DeviceName);

    if (!NT_SUCCESS(Status))
    {
        IoDeleteDevice( gControlDeviceObject );

        return Status;
    }

    return Status;
}

VOID
SFDeleteCDO(
    __in PDRIVER_OBJECT DriverObject
    )
{
    UNICODE_STRING LinkName;

    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE();

    //
    //  Delete symbolic link
    //

    RtlInitUnicodeString(&LinkName, DOS_DEVICE_NAME);
    IoDeleteSymbolicLink(&LinkName);

    //
    //  Delete control device object
    //

    if (gControlDeviceObject != NULL)
    {
        IoDeleteDevice(gControlDeviceObject);
        gControlDeviceObject = NULL;
    }
}

NTSTATUS
SFRegistryCallouts(
    __in PDEVICE_OBJECT DeviceObject
    )
{
    NTSTATUS        Status = STATUS_SUCCESS;
    BOOLEAN         EngineOpened = FALSE;
    BOOLEAN         InTransaction = FALSE;
    FWPM_SESSION0   Session = {0};
    FWPM_SUBLAYER0  FirewallSubLayer;

    Session.flags = FWPM_SESSION_FLAG_DYNAMIC;

    Status = FwpmEngineOpen0(NULL,
                             RPC_C_AUTHN_WINNT,
                             NULL,
                             &Session,
                             &gEngineHandle);

    if (!NT_SUCCESS(Status))
    {
        goto Exit;
    }

    EngineOpened = TRUE;

    Status = FwpmTransactionBegin0(gEngineHandle, 0);

    if (!NT_SUCCESS(Status))
    {
        goto Exit;
    }

    InTransaction = TRUE;

    RtlZeroMemory(&FirewallSubLayer, sizeof(FWPM_SUBLAYER0)); 

    FirewallSubLayer.subLayerKey                = SF_SUBLAYER;
    FirewallSubLayer.displayData.name           = L"Transport SimpleFirewall Sub-Layer";
    FirewallSubLayer.displayData.description    = L"Sub-Layer for use by Transport SimpleFirewall callouts";
    FirewallSubLayer.flags                      = 0;
    FirewallSubLayer.weight                     = 0; 

    Status = FwpmSubLayerAdd0(gEngineHandle, &FirewallSubLayer, NULL);

    if (!NT_SUCCESS(Status))
    {
        goto Exit;
    }

    Status = SFRegisterALEClassifyCallouts(&FWPM_LAYER_ALE_AUTH_CONNECT_V4,
                                           &SF_ALE_CONNECT_CALLOUT_V4,
                                           DeviceObject,
                                           &gAleConnectCalloutIdV4);

    if (!NT_SUCCESS(Status))
    {
        goto Exit;
    }

    Status = SFRegisterALEClassifyCallouts(&FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4,
                                           &SF_ALE_RECV_ACCEPT_CALLOUT_V4,
                                           DeviceObject,
                                           &gAleRecvAcceptCalloutIdV4);

    if (!NT_SUCCESS(Status))
    {
        goto Exit;
    }

    Status = FwpmTransactionCommit0(gEngineHandle);

    if (!NT_SUCCESS(Status))
    {
        goto Exit;
    }

    InTransaction = FALSE;

Exit:

    if (!NT_SUCCESS(Status))
    {
        if (InTransaction)
        {
            FwpmTransactionAbort0(gEngineHandle);
        }

        if (EngineOpened)
        {
            FwpmEngineClose0(gEngineHandle);
            gEngineHandle = NULL;
        }
    }

    return Status;
}

void
SFDeregistryCallouts(
    __in PDEVICE_OBJECT DeviceObject
    )
{
    UNREFERENCED_PARAMETER(DeviceObject);

    FwpmEngineClose0(gEngineHandle);
    gEngineHandle = NULL;

    FwpsCalloutUnregisterById0(gAleConnectCalloutIdV4);
    FwpsCalloutUnregisterById0(gAleRecvAcceptCalloutIdV4);
}

NTSTATUS
SFRegisterALEClassifyCallouts(
    __in const GUID* layerKey,
    __in const GUID* calloutKey,
    __in void* DeviceObject,
    __out UINT32* calloutId
    )
{
    NTSTATUS Status = STATUS_SUCCESS;

    FWPS_CALLOUT0 sCallout = {0};
    FWPM_CALLOUT0 mCallout = {0};

    FWPM_DISPLAY_DATA0 DisplayData = {0};

    BOOLEAN calloutRegistered = FALSE;

    sCallout.calloutKey = *calloutKey;

    if (IsEqualGUID(layerKey, &FWPM_LAYER_ALE_AUTH_CONNECT_V4))
    {
        sCallout.classifyFn = SFALEConnectClassify;
        sCallout.notifyFn   = SFALEConnectNotify;
    }
    else
    {
        sCallout.classifyFn = SFALERecvAcceptClassify;
        sCallout.notifyFn   = SFALERecvAcceptNotify;
    }

    Status = FwpsCalloutRegister0(DeviceObject,
                                  &sCallout,
                                  calloutId);

    if (!NT_SUCCESS(Status))
    {
        goto Exit;
    }

    calloutRegistered = TRUE;

    DisplayData.name = L"Transport SimpleFirewall ALE Classify Callout";
    DisplayData.description = L"Intercepts inbound or outbound connect attempts";

    mCallout.calloutKey = *calloutKey;
    mCallout.displayData = DisplayData;
    mCallout.applicableLayer = *layerKey;

    Status = FwpmCalloutAdd0(gEngineHandle,
                             &mCallout,
                             NULL,
                             NULL);

    if (!NT_SUCCESS(Status))
    {
        goto Exit;
    }

    Status = SFAddFilter(L"Transport SimpleFirewall ALE Classify",
                         L"Intercepts inbound or outbound connect attempts",
                         layerKey,
                         calloutKey);

    if (!NT_SUCCESS(Status))
    {
        goto Exit;
    }

Exit:

    if (!NT_SUCCESS(Status))
    {
        if (calloutRegistered)
        {
            FwpsCalloutUnregisterById0(*calloutId);
            *calloutId = 0;
        }
    }

    return Status;
}

NTSTATUS
SFAddFilter(
    __in const wchar_t* filterName,
    __in const wchar_t* filterDesc,
    __in const GUID* layerKey,
    __in const GUID* calloutKey
    )
{
    FWPM_FILTER0 Filter = {0};

    Filter.layerKey                 = *layerKey;
    Filter.displayData.name         = (wchar_t*)filterName;
    Filter.displayData.description  = (wchar_t*)filterDesc;

    Filter.action.type          = FWP_ACTION_CALLOUT_TERMINATING;
    Filter.action.calloutKey    = *calloutKey;
    Filter.subLayerKey          = SF_SUBLAYER;
    Filter.weight.type          = FWP_EMPTY;
    Filter.rawContext           = 0;

    return FwpmFilterAdd0(gEngineHandle, &Filter, NULL, NULL);
}

NTSTATUS
SFALERecvAcceptNotify(
    __in FWPS_CALLOUT_NOTIFY_TYPE notifyType,
    __in const GUID* filterKey,
    __in const FWPS_FILTER0* filter
    )
{
    UNREFERENCED_PARAMETER(notifyType);
    UNREFERENCED_PARAMETER(filterKey);
    UNREFERENCED_PARAMETER(filter);

    return STATUS_SUCCESS;
}

NTSTATUS
SFALEConnectNotify(
    __in FWPS_CALLOUT_NOTIFY_TYPE notifyType,
    __in const GUID* filterKey,
    __in const FWPS_FILTER0* filter
    )
{
    UNREFERENCED_PARAMETER(notifyType);
    UNREFERENCED_PARAMETER(filterKey);
    UNREFERENCED_PARAMETER(filter);

    return STATUS_SUCCESS;
}

VOID
PerformBasicAction(
    _In_ const FWPS_INCOMING_VALUES* pClassifyValues,
    _In_ const FWPS_INCOMING_METADATA_VALUES* pMetadata,
    _Inout_opt_ VOID* pLayerData,
    _In_ const FWPS_FILTER0* pFilter,
    _In_ UINT64 flowContext,
    _Inout_ FWPS_CLASSIFY_OUT* pClassifyOut,
    _In_ FWP_ACTION_TYPE basicAction
    )
{
    UNREFERENCED_PARAMETER(pClassifyValues);
    UNREFERENCED_PARAMETER(pMetadata);
    UNREFERENCED_PARAMETER(flowContext);
    UNREFERENCED_PARAMETER(pLayerData);

    if (pClassifyOut)
    {
        pClassifyOut->actionType  = basicAction;

        // Clear the right to mark as the definitive answer.

        if ((basicAction == FWP_ACTION_BLOCK) ||
            (basicAction == FWP_ACTION_PERMIT && pFilter->flags & FWPS_FILTER_FLAG_CLEAR_ACTION_RIGHT))
        {
            pClassifyOut->rights ^= FWPS_RIGHT_ACTION_WRITE;
        }
    }
}

BOOLEAN
CanIFilterThisRequest(
    __in const FWPS_INCOMING_METADATA_VALUES0* inMetaValues
    )
{
    if (inMetaValues->currentMetadataValues & FWPS_METADATA_FIELD_PROCESS_ID)
    {
        return SFFindProcessId((ULONG)inMetaValues->processId);
    }

    return FALSE;
}

void
SFALEConnectClassify(
    __in const FWPS_INCOMING_VALUES0* inFixedValues,
    __in const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
    __inout void* layerData,
    __in const FWPS_FILTER0* filter,
    __in UINT64 flowContext,
    __in FWPS_CLASSIFY_OUT0* classifyOut
    )
{
    if (classifyOut->rights & FWPS_RIGHT_ACTION_WRITE)
    {
        FWP_ACTION_TYPE Action = (CanIFilterThisRequest(inMetaValues) ? FWP_ACTION_BLOCK : FWP_ACTION_PERMIT);

        PerformBasicAction(inFixedValues,
                           inMetaValues,
                           layerData,
                           filter,
                           flowContext,
                           classifyOut,
                           Action);
    }
}

void
SFALERecvAcceptClassify(
    __in const FWPS_INCOMING_VALUES0* inFixedValues,
    __in const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
    __inout void* layerData,
    __in const FWPS_FILTER0* filter,
    __in UINT64 flowContext,
    __inout FWPS_CLASSIFY_OUT0* classifyOut
    )
{
    if (classifyOut->rights & FWPS_RIGHT_ACTION_WRITE)
    {
        FWP_ACTION_TYPE Action = (CanIFilterThisRequest(inMetaValues) ? FWP_ACTION_BLOCK : FWP_ACTION_PERMIT);

        PerformBasicAction(inFixedValues,
                           inMetaValues,
                           layerData,
                           filter,
                           flowContext,
                           classifyOut,
                           Action);
    }
}

VOID
CreateProcessNotifyEx(
    __inout PEPROCESS Process,
    __in HANDLE ProcessId,
    __in_opt PPS_CREATE_NOTIFY_INFO  CreateInfo
    )
{
    UNREFERENCED_PARAMETER(Process);

    if (CreateInfo)
    {
        if (RtlEqualUnicodeString(&gTargetPath, (PUNICODE_STRING)CreateInfo->ImageFileName, TRUE))
        {
            SFInsertProcessId((ULONG)(ULONG_PTR)ProcessId);
        }
        else
        {
            SFInsertChildProcessId((ULONG)(ULONG_PTR)CreateInfo->ParentProcessId,
                                   (ULONG)(ULONG_PTR)ProcessId);
        }
    }
    else
    {
        SFRemoveProcessId((ULONG)(ULONG_PTR)ProcessId);
    }
}

NTSTATUS
SFRegistryProcessCallback(
    )
{
    NTSTATUS Status;

    RtlZeroMemory(gProcessIdTable, sizeof(gProcessIdTable));

    gProcessIdTableLock = 0;

    Status = PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyEx, FALSE);

    return Status;
}

VOID
SFDeregistryProcessCallback(
    )
{
    PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyEx, TRUE);

    SFEmptyProcessIdTable();
}

void
SFEmptyProcessIdTable(
    )
{
    KIRQL Irql = ExAcquireSpinLockExclusive(&gProcessIdTableLock);

    RtlZeroMemory(gProcessIdTable, sizeof(gProcessIdTable));

    ExReleaseSpinLockExclusive(&gProcessIdTableLock, Irql);
}

void
SFInsertProcessId(
    __in ULONG ProcessId
    )
{
    size_t  idx;
    KIRQL   Irql = ExAcquireSpinLockExclusive(&gProcessIdTableLock);

    for (idx = 0; idx < MAX_MONITOR_PROCESS_ID; ++idx)
    {
        if (gProcessIdTable[idx] == 0)
        {
            gProcessIdTable[idx] = ProcessId;
            break;
        }
    }

    ExReleaseSpinLockExclusive(&gProcessIdTableLock, Irql);
}

VOID
SFInsertChildProcessId(
    __in ULONG ParentProcessId,
    __in ULONG ProcessId
    )
{
    size_t      idx;
    BOOLEAN     FindParent = FALSE;
    KIRQL       Irql;

    Irql = ExAcquireSpinLockShared(&gProcessIdTableLock);

    for (idx = 0; idx < MAX_MONITOR_PROCESS_ID; ++idx)
    {
        if (gProcessIdTable[idx] == ParentProcessId)
        {
            FindParent = TRUE;
            break;
        }
    }

    ExReleaseSpinLockShared(&gProcessIdTableLock, Irql);

    if (FindParent)
    {
        Irql = ExAcquireSpinLockExclusive(&gProcessIdTableLock);

        for (idx = 0; idx < MAX_MONITOR_PROCESS_ID; ++idx)
        {
            if (gProcessIdTable[idx] == 0)
            {
                gProcessIdTable[idx] = ProcessId;
                break;
            }
        }

        ExReleaseSpinLockExclusive(&gProcessIdTableLock, Irql);
    }
}

VOID
SFRemoveProcessId(
    __in ULONG ProcessId
    )
{
    size_t  idx;
    KIRQL   Irql = ExAcquireSpinLockExclusive(&gProcessIdTableLock);

    for (idx = 0; idx < MAX_MONITOR_PROCESS_ID; ++idx)
    {
        if (gProcessIdTable[idx] == ProcessId)
        {
            gProcessIdTable[idx] = 0;
        }
    }

    ExReleaseSpinLockExclusive(&gProcessIdTableLock, Irql);
}

BOOLEAN
SFFindProcessId(
    __in ULONG ProcessId
    )
{
    size_t  Idx;
    BOOLEAN Result = FALSE;
    KIRQL   Irql = ExAcquireSpinLockShared(&gProcessIdTableLock);

    for (Idx = 0; Idx < MAX_MONITOR_PROCESS_ID; ++Idx)
    {
        if (gProcessIdTable[Idx] == ProcessId)
        {
            Result = TRUE;
            break;
        }
    }

    ExReleaseSpinLockShared(&gProcessIdTableLock, Irql);

    return Result;
}