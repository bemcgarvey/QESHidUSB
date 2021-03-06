
#include "QHidWatcher.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#include <SetupAPI.h>
#include <hidsdi.h>
#include <Dbt.h>

QHidWatcher::QHidWatcher(uint16_t pid, uint16_t vid, QWidget *parent) : QWidget(parent) {
    this->vid = QString::asprintf("vid_%04x", vid);
    this->pid = QString::asprintf("pid_%04x", pid);
    HWND hwnd = (HWND)winId();
    DEV_BROADCAST_DEVICEINTERFACE DevBroadcastDeviceInterface;
    GUID HidGuid;
    HidD_GetHidGuid(&HidGuid);
    DevBroadcastDeviceInterface.dbcc_size = sizeof(DevBroadcastDeviceInterface);
    DevBroadcastDeviceInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    DevBroadcastDeviceInterface.dbcc_classguid = HidGuid;
    DeviceNotificationHandle = RegisterDeviceNotification(
        hwnd, &DevBroadcastDeviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE);
};

QHidWatcher::~QHidWatcher(void) {
    UnregisterDeviceNotification(DeviceNotificationHandle);
}


bool QHidWatcher::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    MSG *msg = reinterpret_cast<MSG *>(message);
    if (msg->message == WM_DEVICECHANGE) {
        DEV_BROADCAST_DEVICEINTERFACE *pdevBroadCastHdr;
        if (msg->lParam != 0) {
            pdevBroadCastHdr = (DEV_BROADCAST_DEVICEINTERFACE *)msg->lParam;
            if (pdevBroadCastHdr->dbcc_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                QString path;
                switch (msg->wParam) {
                case DBT_DEVICEARRIVAL:
                    path = path.fromWCharArray(pdevBroadCastHdr->dbcc_name).toLower();
                    if (path.contains(vid) && (path.contains(pid))) {
                        emit connected();
                    }
                    return true;
                case DBT_DEVICEREMOVECOMPLETE:
                    path = path.fromWCharArray(pdevBroadCastHdr->dbcc_name).toLower();
                    if (path.contains(vid) && (path.contains(pid))) {
                        emit removed();
                    }
                    return true;
                }
            }
        }
    }
    return false;
}
#endif

#ifdef Q_OS_LINUX

#include "worker.h"


QHidWatcher::QHidWatcher(uint16_t pid, uint16_t vid, QWidget *parent) : QWidget(parent) {
    worker = new Worker(pid, vid, this);
    dynamic_cast<Worker *>(worker)->start();
}

QHidWatcher::~QHidWatcher(void) {
    dynamic_cast<Worker *>(worker)->Stop();
    worker->wait(1000);
    delete worker;
}

#endif
