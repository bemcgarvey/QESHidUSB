#include "HidUSBLink.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#include <SetupAPI.h>
#include <hidsdi.h>


HidUSBLink::HidUSBLink(size_t reportSize, bool includeReportNum)  {
    handle = INVALID_HANDLE_VALUE;
    if (reportSize <= 64)
        this->reportSize = reportSize;
    else
        reportSize = 64;
    this->includeReportNum = includeReportNum;
}

HidUSBLink::~HidUSBLink() {
    if (handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
    }
}

void HidUSBLink::Open(uint16_t pid, uint16_t vid) {
    HDEVINFO deviceInfo = INVALID_HANDLE_VALUE;
    GUID guid;
    SP_DEVICE_INTERFACE_DATA deviceInfoData;
    DWORD i = 0;

    HidD_GetHidGuid(&guid);
    deviceInfo = SetupDiGetClassDevs(&guid, NULL, NULL,
                                     (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));
    if (deviceInfo == INVALID_HANDLE_VALUE) {
        return;
    }
    deviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    while (
           SetupDiEnumDeviceInterfaces(deviceInfo, 0, &guid, i, &deviceInfoData)) {
        DWORD requiredSize;
        SetupDiGetDeviceInterfaceDetail(deviceInfo, &deviceInfoData, NULL, 0,
                                        &requiredSize, NULL);
        PSP_DEVICE_INTERFACE_DETAIL_DATA functionClassDeviceData;
        functionClassDeviceData =
                (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
        functionClassDeviceData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        if (SetupDiGetDeviceInterfaceDetail(deviceInfo, &deviceInfoData,
                                            functionClassDeviceData, requiredSize,
                                            &requiredSize, NULL)) {
            HANDLE hidDevice;
            hidDevice = CreateFile(functionClassDeviceData->DevicePath,
                                   GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                   OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
            if (hidDevice != INVALID_HANDLE_VALUE) {
                HIDD_ATTRIBUTES attributes;
                attributes.Size = sizeof(HIDD_ATTRIBUTES);
                HidD_GetAttributes(hidDevice, &attributes);
                if (pid == attributes.ProductID && vid == attributes.VendorID) {
                    handle = hidDevice;
                    free(functionClassDeviceData);
                    break;
                } else {
                    CloseHandle(hidDevice);
                }
            }
            free(functionClassDeviceData);
        }
        ++i;
    }
    if (deviceInfo != INVALID_HANDLE_VALUE) {
        SetupDiDestroyDeviceInfoList(deviceInfo);
    }
}

bool HidUSBLink::SendReport(BYTE *buffer, int reportNum) {
    DWORD len;
    HANDLE hEventObject;
    OVERLAPPED HIDOverlapped;
    int status;
    BYTE temp[65];
    temp[0] = static_cast<BYTE>(reportNum);
    memcpy(&temp[1], buffer, reportSize);
    hEventObject = CreateEvent((LPSECURITY_ATTRIBUTES)NULL, FALSE, TRUE, L"");
    HIDOverlapped.hEvent = hEventObject;
    HIDOverlapped.Offset = 0;
    HIDOverlapped.OffsetHigh = 0;
    WriteFile(handle, temp, reportSize + 1, &len, &HIDOverlapped);
    status = WaitForSingleObject(hEventObject, 100);
    if (status == WAIT_OBJECT_0) {
        return true;
    } else {
        CancelIo(handle);
        return false;
    }
}

bool HidUSBLink::GetReport(BYTE *buffer) {
    DWORD len;
    HANDLE hEventObject;
    OVERLAPPED HIDOverlapped;
    int status;
    BYTE temp[65];

    hEventObject = CreateEvent((LPSECURITY_ATTRIBUTES)NULL, FALSE, TRUE, L"");
    HIDOverlapped.hEvent = hEventObject;
    HIDOverlapped.Offset = 0;
    HIDOverlapped.OffsetHigh = 0;

    if (includeReportNum)
        status = ReadFile(handle, buffer, reportSize + 1, &len, &HIDOverlapped);
    else
        status = ReadFile(handle, temp, reportSize + 1, &len, &HIDOverlapped);
    status = WaitForSingleObject(hEventObject, 100);
    if (status == WAIT_OBJECT_0) {
        if (!includeReportNum) {
            memcpy(buffer, &temp[1], reportSize);
        }
        return true;
    } else {
        CancelIo(handle);
        return false;
    }
}

bool HidUSBLink::Connected(void) { return handle != INVALID_HANDLE_VALUE; }

void HidUSBLink::Close(void) {
    if (handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
    }
}

#endif

#ifdef Q_OS_LINUX
#include "hidapi.h"
#include <string.h>
HidUSBLink::HidUSBLink(size_t reportSize, bool includeReportNum) {
    handle = nullptr;
    this->reportSize = reportSize;
    this->includeReportNum = includeReportNum;
}

HidUSBLink::~HidUSBLink() {
    if (handle) {
        Close();
    }
    hid_exit();
}

void HidUSBLink::Open(uint16_t pid, uint16_t vid) {
    handle = hid_open(vid, pid, NULL);
}

bool HidUSBLink::SendReport(uint8_t *buffer, int reportNum) {
    unsigned int result;

    uint8_t temp[65];
    if (!includeReportNum) {
        temp[0] = 0;
    } else {
        temp[0] = reportNum;
    }
    memcpy(&temp[1], buffer, reportSize);
    result = hid_write(reinterpret_cast<hid_device *>(handle), temp, reportSize + 1);
    if (result == reportSize + 1) {
        return true;
    } else {
        return false;
    }
}


bool HidUSBLink::GetReport(uint8_t *buffer) {
    unsigned int result;
    result = hid_read_timeout(reinterpret_cast<hid_device *>(handle), buffer, reportSize, 100);
    if (result == reportSize) {
        return true;
    } else {
        return false;
    }
}

bool HidUSBLink::Connected(void) {
    return handle != nullptr;
}

void HidUSBLink::Close(void) {
    if (handle != nullptr) {
        hid_close(reinterpret_cast<hid_device *>(handle));
    }
    handle = nullptr;
}
#endif
