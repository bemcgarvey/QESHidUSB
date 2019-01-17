#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include <libudev.h>
#include <stdint.h>
#include <QString>
#include "QHidWatcher.h"

class Worker : public QThread {
public:
    Worker(uint16_t pid, uint16_t vid, QHidWatcher *parent);
    ~Worker();
    void Stop() {stop = true;}
protected:
    void run();
signals:
    void action(QString type);
private:
    QString pidStr;
    QString vidStr;
    bool stop;
    udev_monitor *mon;
    udev *pudev;
    QHidWatcher *watcher;
};

#endif // WORKER_H
