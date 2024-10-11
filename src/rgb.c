#include "rgb.h"
#include "debug.h"





#define LIST_SIZE(list) (sizeof(list) / sizeof(list[0]))
#define hex2rgb(c) (((c) >> 16) & 0xff), (((c) >> 8) & 0xff), ((c) & 0xff)

#define CODE_0 (3)
#define CODE_1 (7)
#define RESET_LEN (60)
#define TIM_DMA_CH1_CH DMA1_Channel5
#define COLOR_BUFFER_LEN (((Pixel_NUM) * (3 * 8)) + RESET_LEN)

uint16_t color_buf[COLOR_BUFFER_LEN] = {0};
uint32_t rgb_i = 0, rgb_j = 0;
uint32_t color = 0x0f0f00;


void TIM1_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure = {0};
  TIM_OCInitTypeDef TIM_OCInitStructure = {0};
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;
  TIM_TimeBaseInitStructure.TIM_Prescaler = SystemCoreClock / 8000000 - 1;
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
  TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);
  TIM_Cmd(TIM1, ENABLE);
}



void DMA1_Init(void) {
  DMA_InitTypeDef DMA_InitStructure = {0};

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(DMA1_Channel5);
  DMA_Cmd(DMA1_Channel5, DISABLE);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM1->CH1CVR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)color_buf;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = COLOR_BUFFER_LEN;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);

  DMA_Cmd(DMA1_Channel5, DISABLE);
}



void setPixelColor(uint16_t id, uint8_t r, uint8_t g, uint8_t b) {
  int i = 0, j = id * 24u;
  if (id >= Pixel_NUM) {
    return;
  }

  for (i = 0; i < 8; i++) {
    if (g & (0x80 >> i)) {
      color_buf[j] = CODE_1;
    } else {
      color_buf[j] = CODE_0;
    }
    j++;
  }
  for (i = 0; i < 8; i++) {
    if (r & (0x80 >> i)) {
      color_buf[j] = CODE_1;
    } else {
      color_buf[j] = CODE_0;
    }
    j++;
  }
  for (i = 0; i < 8; i++) {
    if (b & (0x80 >> i)) {
      color_buf[j] = CODE_1;
    } else {
      color_buf[j] = CODE_0;
    }
    j++;
  }
}

#define MAX_STEP (200)
/*********************************************************************
 * @fn      interpolateColors
 *
 * @brief   Set the pixel color of an LED
 *
 * @param   color1 - The color at the beginning
 *          color2 - The color at the ending
 *          step   -
 *
 *
 * @return  color
 */
uint32_t interpolateColors(uint32_t color1, uint32_t color2, uint8_t step) {
  uint8_t r1 = (color1 >> 16) & 0xFF;
  uint8_t g1 = (color1 >> 8) & 0xFF;
  uint8_t b1 = color1 & 0xFF;

  uint8_t r2 = (color2 >> 16) & 0xFF;
  uint8_t g2 = (color2 >> 8) & 0xFF;
  uint8_t b2 = color2 & 0xFF;

  float tmp1 = (r2 - r1) * (step / (float)MAX_STEP);
  float tmp2 = (g2 - g1) * (step / (float)MAX_STEP);
  float tmp3 = (b2 - b1) * (step / (float)MAX_STEP);

  uint8_t r = (uint8_t)(r1 + tmp1);
  uint8_t g = (uint8_t)(g1 + tmp2);
  uint8_t b = (uint8_t)(b1 + tmp3);

  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

uint32_t color_list[] = {
    0xff0000, 0x00ff00, 0x0000ff, 0xffff00, 0xff00ff, 0x00ffff, 0xffffff,
    //        0x000000,
};



void RGB_Init(void) {
  uint16_t i = 0;
  // Turn off all LEDs
  for (i = 0; i < Pixel_NUM; i++) {
    setPixelColor(i, 0, 0x0f, 0);
  }

  TIM1_Init();
  DMA1_Init();
  DMA_Cmd(DMA1_Channel5, ENABLE);
}

void DMA1_Channel5_IRQHandler(void)
{
    if (DMA_GetFlagStatus( DMA1_FLAG_TC5)) {
        TIM_Cmd( TIM1, DISABLE);
        DMA_Cmd(  DMA1_Channel5, DISABLE);
        DMA_ClearFlag( DMA1_FLAG_TC5);
    }
}



