#ifndef _RGB_H_
#define _RGB_H_

#include <stdint.h>

#define Pixel_NUM (64)


void DMA1_Channel5_IRQHandler(void)
    __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM2_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));


void led_example_0(void);
void led_example_1(void);
void led_example_2(void);
void RGB_Init(void);




#endif
