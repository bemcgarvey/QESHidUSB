#ifndef _HID_USB_LINK_
#define _HID_USB_LINK_

#include "qeshidusb_global.h"
#include <stdint.h>

#define MY_VID             0x4d63

class QESHIDUSBSHARED_EXPORT HidUSBLink
{
public:
    explicit HidUSBLink(size_t reportSize = 64, bool includeReportNum = false);
    HidUSBLink(HidUSBLink &obj) = delete;
    ~HidUSBLink();
    HidUSBLink& operator=(HidUSBLink &obj) = delete;
    void Open(uint16_t pid, uint16_t vid = MY_VID);
    bool SendReport(uint8_t *buffer, int reportNum = 0);
    bool GetReport(uint8_t *buffer);
	bool Connected(void);
	void Close(void);
private:
    size_t reportSize;
    bool includeReportNum;
    void *handle;
};

#endif
