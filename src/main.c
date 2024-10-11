#include "app.h"
#include "ch32v30x.h"
#include "ch32v30x_rcc.h"
#include "core_riscv.h"
#include "debug.h"

#include "ads8688.h"
#include "rgb.h"

#include "usb.h"
#include "usbd_core.h"
#include <stdint.h>
#include <stdio.h>



void usb_dc_low_level_init(void) {
    RCC_USBCLK48MConfig(RCC_USBCLK48MCLKSource_USBPHY);
    RCC_USBHSPLLCLKConfig(RCC_HSBHSPLLCLKSource_HSE);
    RCC_USBHSConfig(RCC_USBPLL_Div2);
    RCC_USBHSPLLCKREFCLKConfig(RCC_USBHSPLLCKREFCLK_4M);
    RCC_USBHSPHYPLLALIVEcmd(ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBHS, ENABLE);
    NVIC_EnableIRQ(USBHS_IRQn);

    Delay_Us(100);
}

int main(void) {
  // System Setup
  SystemCoreClockUpdate();
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  Delay_Init();

  

  ADS8688_Init();
  hid_custom_init();
  RGB_Init();
  // cdc_acm_init();
  //  Wait until configured
  while (!usb_device_is_configured()) {
  }

  /* while (!dtr_enable) { */
  /* } */

  printf("SystemClk:%d\r\n", SystemCoreClock);

  // App Run
  run();
}

/* __attribute__((used)) int _write(int fd, char *buf, int size) { */
/*   if (dtr_enable) { */
/*     ep_tx_busy_flag = true; */
/*     usbd_ep_start_write(CDC_IN_EP, buf, size); */
/*     while (ep_tx_busy_flag) { */
/*     } */
/*   } */

/*   return size; */
/* } */
