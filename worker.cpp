#include "worker.h"

Worker::Worker(uint16_t pid, uint16_t vid, QHidWatcher *watcher) {
    this->watcher = watcher;
    pidStr = QString::asprintf(":%04X", pid);
    vidStr = QString::asprintf(":%04X", vid);
    stop = false;
    //qDebug("%s %s", pidStr.toStdString().c_str(), vidStr.toStdString().c_str());
    pudev = udev_new();
    mon = udev_monitor_new_from_netlink(pudev, "udev");
    if (mon == NULL) {
        qDebug("%s", "Can't open monitor");
    }
    udev_monitor_filter_add_match_subsystem_devtype(mon, "hidraw", NULL);
    udev_monitor_enable_receiving(mon);
}

Worker::~Worker() {
    udev_monitor_unref(mon);
    udev_unref(pudev);
}

void Worker::run() {
    setPriority(QThread::LowestPriority);
    while (!stop) {
        udev_device *dev = udev_monitor_receive_device(mon);
        if (dev) {
            //qDebug("%s", udev_device_get_action(dev));
            //qDebug("%s", udev_device_get_devpath(dev));
            QString path = udev_device_get_devpath(dev);
            QString action = udev_device_get_action(dev);
            if (path.contains(vidStr) && path.contains(pidStr)) {
                if (action == "add") {
                    emit watcher->connected();
                } else if (action == "remove") {
                    emit watcher->removed();
                }
            }
            udev_device_unref(dev);
        }
        msleep(100);
    }
}
