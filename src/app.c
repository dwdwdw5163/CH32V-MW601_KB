#include "app.h"
#include "ads8688.h"
#include "debug.h"
#include "key.h"

#include "rgb.h"
#include "usb.h"
#include "usbd_core.h"
#include "usbd_hid.h"
#include <string.h>
#include <sys/_stdint.h>

#define HID_BUFFER_LENGTH 64
#define DEFAULT_ADVANCED_KEY_MODE KEY_ANALOG_RAPID_MODE
// #define DEFAULT_ADVANCED_KEY_MODE KEY_ANALOG_NORMAL_MODE
#define DEFAULT_TRIGGER_DISTANCE 0.005
#define DEFAULT_RELEASE_DISTANCE 0.005
#define DEFAULT_UPPER_DEADZONE 0.02
#define DEFAULT_LOWER_DEADZONE 0.25
#define DEFAULT_SCHMITT_PARAMETER 0.01

AdvancedKey g_keyboard_advanced_keys[64] = {
    {.key.id = 1},  {.key.id = 3},  {.key.id = 5},  {.key.id = 7},
    {.key.id = 9},  {.key.id = 11}, {.key.id = 13}, {.key.id = 56},
    {.key.id = 2},  {.key.id = 4},  {.key.id = 6},  {.key.id = 8},
    {.key.id = 10}, {.key.id = 12}, {.key.id = 14}, {.key.id = 57},
    {.key.id = 15}, {.key.id = 17}, {.key.id = 19}, {.key.id = 21},
    {.key.id = 23}, {.key.id = 25}, {.key.id = 27}, {.key.id = 58},
    {.key.id = 16}, {.key.id = 18}, {.key.id = 20}, {.key.id = 22},
    {.key.id = 24}, {.key.id = 26}, {.key.id = 28}, {.key.id = 59},
    {.key.id = 29}, {.key.id = 31}, {.key.id = 33}, {.key.id = 35},
    {.key.id = 37}, {.key.id = 39}, {.key.id = 41}, {.key.id = 60},
    {.key.id = 30}, {.key.id = 32}, {.key.id = 34}, {.key.id = 36},
    {.key.id = 38}, {.key.id = 40}, {.key.id = 54}, {.key.id = 61},
    {.key.id = 42}, {.key.id = 44}, {.key.id = 46}, {.key.id = 48},
    {.key.id = 50}, {.key.id = 52}, {.key.id = 55}, {.key.id = 62},
    {.key.id = 43}, {.key.id = 45}, {.key.id = 47}, {.key.id = 49},
    {.key.id = 51}, {.key.id = 53}, {.key.id = 64}, {.key.id = 63}};

uint16_t ads_data[64];
uint32_t cnt = 0;
uint32_t channel = 0;
uint32_t key_index = 0;
uint32_t kb_state = 0;
uint32_t led_status = 0;

void (*SysMemBootJump)(void);
__IO uint32_t BootAddr = 0x1FFF8000; /* BootLoader 地址 */

void JumpToBootloader(void) {
  uint32_t i = 0;

  /* 关闭全局中断 */
  __disable_irq();

  /* 关闭滴答定时器，复位到默认值 */
  // SysTick->CTRL = 0;
  // SysTick->LOAD = 0;
  // SysTick->VAL = 0;
  SysTick->CTLR = 0;
  SysTick->CMP = 0;
  SysTick->CNT = 0;
  SysTick->SR = 0;

  /* 设置所有时钟到默认状态，使用HSI时钟 */
  RCC_DeInit();

  /* 关闭所有中断，清除所有中断挂起标志 */
  for (i = 0; i < 8; i++) {
    NVIC->IPSR[i] = 0xFFFFFFFF;
    NVIC->IPRR[i] = 0xFFFFFFFF;
  }

  /* 使能全局中断 */
  __enable_irq();

  /* 跳转到系统BootLoader，首地址是MSP，地址+4是复位中断服务程序地址 */
  SysMemBootJump = (void (*)(void))(*((uint32_t *)(BootAddr + 4)));

  /* 设置主堆栈指针 */
  //__set_MSP(*(uint32_t *)BootAddr);
  //__asm("li  a6, 0x5z000");
  //__asm("jr  a6");

  /* 跳转到系统BootLoader */
  SysMemBootJump();

  /* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
  while (1) {
  }
}

void keyboard_factory_reset() {
  for (uint8_t i = 0; i < 64; i++) {
    g_keyboard_advanced_keys[i].key.report_state = 0;
    g_keyboard_advanced_keys[i].key.state = 0;
    g_keyboard_advanced_keys[i].mode = DEFAULT_ADVANCED_KEY_MODE;
    g_keyboard_advanced_keys[i].trigger_distance = DEFAULT_TRIGGER_DISTANCE;
    g_keyboard_advanced_keys[i].release_distance = DEFAULT_RELEASE_DISTANCE;
    g_keyboard_advanced_keys[i].schmitt_parameter = DEFAULT_SCHMITT_PARAMETER;
    g_keyboard_advanced_keys[i].activation_value = 0.3;
    advanced_key_set_deadzone(g_keyboard_advanced_keys + i,
                              DEFAULT_UPPER_DEADZONE, DEFAULT_LOWER_DEADZONE);
  }
  g_keyboard_advanced_keys[47].mode = KEY_ANALOG_NORMAL_MODE; // Fn
}

void analog_check() {

  channel = cnt % 8;

  TMUX_Switch(channel);
  Delay_Us(50);

  ADS_Read_All_Raw(&ads_data[channel * 8]);

  for (int i = 0; i < 8; i++) {

    uint32_t index = channel * 8 + i;

    float analog_avg = 30000 - ads_data[index]; // because of the sensors

    if (kb_state) {
      if (analog_avg < g_keyboard_advanced_keys[index].lower_bound) {
        advanced_key_set_range(g_keyboard_advanced_keys + index,
                               g_keyboard_advanced_keys[index].upper_bound,
                               analog_avg);
        advanced_key_set_deadzone(
            g_keyboard_advanced_keys + index, 0.02,
            g_keyboard_advanced_keys[index].lower_deadzone);
      }

      advanced_key_update_raw(g_keyboard_advanced_keys + index, analog_avg);

      if (g_keyboard_advanced_keys[index].key.report_state) {
        setPixelColor(g_keyboard_advanced_keys[index].key.id - 1, 0xff, 0xff,
                      0xff);
        // printf("num: %d\r\n", matrix2pos[i+(cnt%8)*8]-1);
      } else {
        setPixelColor(g_keyboard_advanced_keys[index].key.id - 1, 0, 0, 0);
      }
      if (led_status & 0x02) {
        setPixelColor(28, 0xff, 0xff, 0xff); // cap lock
      }
    }
    // printf("%d ", ads_data[i]);
  }
  // printf("\r\n");
  cnt++;
}

void TIM2_Init(uint16_t psc, uint16_t arr) {
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseInitStructure.TIM_Period = arr;
  TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_ARRPreloadConfig(TIM2, ENABLE);
  TIM_Cmd(TIM2, ENABLE);

  NVIC_InitTypeDef NVIC_InitStructure = {0};
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void run(void) {

  for (uint8_t i = 0; i < 64; i++)
    analog_check();

  keyboard_factory_reset();

  for (uint8_t i = 0; i < 64; i++) {
    advanced_key_set_range(g_keyboard_advanced_keys + i, 30000 - ads_data[i],
                           10000);
    key_attach(&g_keyboard_advanced_keys[i].key, KEY_EVENT_UP, NULL);
    key_attach(&g_keyboard_advanced_keys[i].key, KEY_EVENT_DOWN, NULL);

    advanced_key_set_deadzone(g_keyboard_advanced_keys + i,
                              DEFAULT_UPPER_DEADZONE,
                              g_keyboard_advanced_keys[i].lower_deadzone);
  }

  { //ESC
    g_keyboard_advanced_keys[0].trigger_distance = 0.5;
    
    advanced_key_set_range(g_keyboard_advanced_keys, 30000 - ads_data[0],
                           29000);
    key_attach(&g_keyboard_advanced_keys[0].key, KEY_EVENT_UP, NULL);
    key_attach(&g_keyboard_advanced_keys[0].key, KEY_EVENT_DOWN, NULL);

    advanced_key_set_deadzone(g_keyboard_advanced_keys,
                              DEFAULT_UPPER_DEADZONE,
                              g_keyboard_advanced_keys[0].lower_deadzone);
  }

  TIM2_Init(100 - 1, 1440 - 1);

  kb_state = 1;
  // Everything is interrupt driven so just loop here
  while (1) {
    analog_check();
    if (g_keyboard_advanced_keys[47].key.report_state &&
        g_keyboard_advanced_keys[34].key.report_state) {
      JumpToBootloader();
    }
  }
}

void TIM2_IRQHandler(void) {
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  keyboard_send_report();
}
