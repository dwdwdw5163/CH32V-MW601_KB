#ifndef _USB_H_
#define _USB_H_


#define RAWHID_USAGE_PAGE 0xFFC0
#define RAWHID_USAGE 0x0C00

/*!< hidraw in endpoint */
#define HIDRAW_IN_EP 0x81
#define HIDRAW_IN_EP_SIZE 64
#define HIDRAW_IN_INTERVAL 1

/*!< hidraw out endpoint */
#define HIDRAW_OUT_EP 0x02
#define HIDRAW_OUT_EP_SIZE 64
#define HIDRAW_OUT_EP_INTERVAL 1

#define USBD_VID 0xFEED
#define USBD_PID 0x6064
#define USBD_MAX_POWER 500
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_HID_CONFIG_DESC_SIZ (9 + 9 + 9 + 7 + 7)

/*!< custom hid report descriptor size */
#define HID_CUSTOM_REPORT_DESC_SIZE 89

#define HID_STATE_IDLE 0
#define HID_STATE_BUSY 1

void hid_custom_init(void);
void keyboard_send_report(void);


#endif
