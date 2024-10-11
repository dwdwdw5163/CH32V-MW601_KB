#include "usb.h"
#include "debug.h"
#include "key.h"
#include "keymap.h"
#include "usbd_core.h"
#include "usbd_hid.h"

/*!< global descriptor */
static const uint8_t hid_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID,
                               0x0002, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01,
                               USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    /************** Descriptor of Custom interface *****************/
    0x09,                          /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type
                                    */
    0x00,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x02,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x00,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00, /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,    /* iInterface: Index of string descriptor */
    /******************** Descriptor of Custom HID ********************/
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01, 0x00,              /* bCountryCode: Hardware target country */
    0x01, /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22, /* bDescriptorType */
    HID_CUSTOM_REPORT_DESC_SIZE, /* wItemLength: Total length of Report
                                    descriptor */
    0x00,
    /******************** Descriptor of Custom in endpoint ********************/
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HIDRAW_IN_EP,                 /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    WBVAL(HIDRAW_IN_EP_SIZE),     /* wMaxPacketSize: 4 Byte max */
    HIDRAW_IN_INTERVAL,           /* bInterval: Polling Interval */
    /******************** Descriptor of Custom out endpoint
     ********************/
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HIDRAW_OUT_EP,                /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    WBVAL(HIDRAW_OUT_EP_SIZE),    /* wMaxPacketSize: 4 Byte max */
    HIDRAW_OUT_EP_INTERVAL,       /* bInterval: Polling Interval */
    /* 73 */
    /*
     * string0 descriptor
     */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /*
     * string1 descriptor
     */
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    /*
     * string2 descriptor
     */
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'H', 0x00,                  /* wcChar10 */
    'I', 0x00,                  /* wcChar11 */
    'D', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    /*
     * string3 descriptor
     */
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    /*
     * device qualifier descriptor
     */
    0x0a, USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x40, 0x01, 0x00,
#endif
    0x00};

/*!< custom hid report descriptor */
static const uint8_t hid_custom_report_desc[HID_CUSTOM_REPORT_DESC_SIZE] = {
    /* USER CODE BEGIN 0 */
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x06, // USAGE (Keyboard)
    0xa1, 0x01, // COLLECTION (Application)
    0x85, 0x01, //     REPORT_ID (1)
    // bitmap of modifiers
    0x75, 0x01, //   Report Size (1),
    0x95, 0x08, //   Report Count (8),
    0x05, 0x07, //   Usage Page (Key Codes),
    0x19, 0xE0, //   Usage Minimum (224),
    0x29, 0xE7, //   Usage Maximum (231),
    0x15, 0x00, //   Logical Minimum (0),
    0x25, 0x01, //   Logical Maximum (1),
    0x81, 0x02, //   Input (Data, Variable, Absolute), ;Modifier byte
    // bitmap of keys
    0x95, 0x78, //   Report Count (120),
    0x75, 0x01, //   Report Size (1),
    0x15, 0x00, //   Logical Minimum (0),
    0x25, 0x01, //   Logical Maximum(1),
    0x05, 0x07, //   Usage Page (Key Codes),
    0x19, 0x00, //   Usage Minimum (0),
    0x29, 0x77, //   Usage Maximum (),
    0x81, 0x02, //   Input (Data, Variable, Absolute),
    // LED output report
    0x95, 0x05, //   Report Count (5),
    0x75, 0x01, //   Report Size (1),
    0x05, 0x08, //   Usage Page (LEDs),
    0x19, 0x01, //   Usage Minimum (1),
    0x29, 0x05, //   Usage Maximum (5),
    0x91, 0x02, //   Output (Data, Variable, Absolute),
    0x95, 0x01, //   Report Count (1),
    0x75, 0x03, //   Report Size (3),
    0x91, 0x03, //   Output (Constant),
    0xC0,
    //	RAW HID
    0x06, 0xC0, 0xFF,             // 30
    0x0A, 0x00, 0x0C, 0xA1, 0x01, // Collection 0x01
    0x85, 0x02,                   // REPORT_ID (2)
    0x75, 0x08,                   // report size = 8 bits
    0x15, 0x00,                   // logical minimum = 0
    0x26, 0xFF, 0x00,             // logical maximum = 255
    0x95, 17,                     // report count TX
    0x09, 0x01,                   // usage
    0x81, 0x02,                   // Input (array)
    0x95, 17,                     // report count RX
    0x09, 0x02,                   // usage
    0x91, 0x02,                   // Output (array)
    0xC0,                         /*     END_COLLECTION	       89 Bytes      */ 
};

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[HIDRAW_OUT_EP_SIZE];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t send_buffer[HIDRAW_IN_EP_SIZE];

/*!< hid state ! Data can be sent only when state is idle  */
uint8_t custom_state;

void usbd_event_handler(uint8_t event) {
  switch (event) {
  case USBD_EVENT_RESET:
    break;
  case USBD_EVENT_CONNECTED:
    break;
  case USBD_EVENT_DISCONNECTED:
    break;
  case USBD_EVENT_RESUME:
    break;
  case USBD_EVENT_SUSPEND:
    break;
  case USBD_EVENT_CONFIGURED:
    /* setup first out ep read transfer */
    usbd_ep_start_read(HIDRAW_OUT_EP, read_buffer, HIDRAW_OUT_EP_SIZE);
    break;
  case USBD_EVENT_SET_REMOTE_WAKEUP:
    break;
  case USBD_EVENT_CLR_REMOTE_WAKEUP:
    break;

  default:
    break;
  }
}

static void usbd_hid_custom_in_callback(uint8_t ep, uint32_t nbytes) {
  // USB_LOG_RAW("actual in len:%d\r\n", nbytes);
  custom_state = HID_STATE_IDLE;
}

static void usbd_hid_custom_out_callback(uint8_t ep, uint32_t nbytes) {
  // USB_LOG_RAW("actual out len:%d\r\n", nbytes);
  usbd_ep_start_read(HIDRAW_OUT_EP, read_buffer, HIDRAW_OUT_EP_SIZE);
  if(read_buffer[0]==1) {
    extern uint32_t led_status;
    led_status = read_buffer[1];
  }
  //read_buffer[0] = 0x02; /* IN: report id */
  //usbd_ep_start_write(HIDRAW_IN_EP, read_buffer, nbytes);
}

static struct usbd_endpoint custom_in_ep = {
    .ep_cb = usbd_hid_custom_in_callback, .ep_addr = HIDRAW_IN_EP};

static struct usbd_endpoint custom_out_ep = {
    .ep_cb = usbd_hid_custom_out_callback, .ep_addr = HIDRAW_OUT_EP};

/* function ------------------------------------------------------------------*/
/**
 * @brief            hid custom init
 * @pre              none
 * @param[in]        none
 * @retval           none
 */
struct usbd_interface intf0;

void hid_custom_init(void) {
  usbd_desc_register(hid_descriptor);
  usbd_add_interface(usbd_hid_init_intf(&intf0, hid_custom_report_desc,
                                        HID_CUSTOM_REPORT_DESC_SIZE));
  usbd_add_endpoint(&custom_in_ep);
  usbd_add_endpoint(&custom_out_ep);

  usbd_initialize();
  
}

void keyboard_send_report(void) {
  // keyboard_post_process();
  int16_t index, bitIndex, keycode, layer;
  extern AdvancedKey g_keyboard_advanced_keys[64];
  layer = g_keyboard_advanced_keys[47].key.report_state ? 1 : 0; // Fn key

  memset(send_buffer, 0, HIDRAW_IN_EP_SIZE);
  send_buffer[0] = 1;
  for (int i = 0; i < 64; i++) {
    keycode = g_default_keymap[layer][g_keyboard_advanced_keys[i].key.id - 1];
    index = (int16_t)(keycode / 8 + 1); // +1 for modifier
    bitIndex = (int16_t)(keycode % 8);
    if (bitIndex < 0) {
      index -= 1;
      bitIndex += 8;
    } else if (keycode > 100)
      continue;
    if (g_keyboard_advanced_keys[i].key.report_state)
      send_buffer[index + 1] |= 1 << (bitIndex); // +1 for Report-ID
  }

  custom_state = HID_STATE_BUSY;
  int ret = usbd_ep_start_write(HIDRAW_IN_EP, send_buffer, 16 + 1);
  if (ret < 0) {
    return;
  }
  /* while (custom_state == HID_STATE_BUSY) { */
  /* } */
  
}
