#ifndef QHIDWATCHER_H
#define QHIDWATCHER_H

#include "qeshidusb_global.h"
#include <QWidget>
#include <QString>
#include <QThread>
#include <stdint.h>


#define MY_VID             0x4d63

class QESHIDUSBSHARED_EXPORT QHidWatcher : public QWidget
{
    Q_OBJECT
public:
    explicit QHidWatcher(uint16_t pid, uint16_t vid = MY_VID, QWidget *parent = nullptr);
    ~QHidWatcher(void);
signals:
    void connected(void);
    void removed(void);

#ifdef Q_OS_WIN
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif


private:
    QString pid;
    QString vid;

#ifdef Q_OS_WIN
    void *DeviceNotificationHandle;
#endif
#ifdef Q_OS_LINUX
    QThread *worker;
#endif
};

#endif // QHIDWATCHER_H
