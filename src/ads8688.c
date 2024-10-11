#include "ads8688.h"


#define USE_SOFT_SPI

#define CS_HIGH() GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define CS_LOW() GPIO_ResetBits(GPIOA, GPIO_Pin_4)

#define SCK_HIGH() GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define SCK_LOW() GPIO_ResetBits(GPIOA, GPIO_Pin_5)

#define MOSI_HIGH() GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define MOSI_LOW() GPIO_ResetBits(GPIOA, GPIO_Pin_7)

#define MISO() GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)

void Hard_SPI1_Init() {
  GPIO_InitTypeDef GPIO_InitStructure = {0};
  SPI_InitTypeDef SPI_InitStructure = {0};

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; // CS
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  SPI_Cmd(SPI1, ENABLE);
}

void Soft_SPI1_Init() {
  GPIO_InitTypeDef GPIO_InitStructure = {0};

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; // CS
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // SCK
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA, GPIO_Pin_5);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // MISO
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; // MOSI
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TMUX_Init() {
  GPIO_InitTypeDef GPIO_InitStructure = {0};

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);   // EN
  GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET); // A0
  GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_RESET); // A1
  GPIO_WriteBit(GPIOB, GPIO_Pin_2, Bit_RESET); // A2
}

void TMUX_Switch(uint32_t channel) {
  GPIO_WriteBit(GPIOB, GPIO_Pin_0, channel & 0x01); // A0
  GPIO_WriteBit(GPIOB, GPIO_Pin_1, channel & 0x02); // A1
  GPIO_WriteBit(GPIOB, GPIO_Pin_2, channel & 0x04); // A2
}

uint16_t Soft_SPI_TransmitReceive(uint16_t tx_data) {
  uint16_t rx_data = 0;

  CS_LOW();

  for (int i = 0; i < 16; i++) {
    SCK_HIGH();
    if (tx_data & 0x8000) {
      MOSI_HIGH();
    } else {
      MOSI_LOW();
    }
    tx_data <<= 1;
    //Delay_Us(1);
    SCK_LOW();
  }
  //Delay_Us(1);
  for (int i = 0; i < 16; i++) {
    SCK_HIGH();
    rx_data <<= 1;
    if (MISO()) {
      rx_data++;
    }
    //Delay_Us(1);
    SCK_LOW();
  }

  CS_HIGH();

  return rx_data;
}

uint8_t Hard_SPI1_ReadWriteByte(uint8_t tx_data) {
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    ;
  SPI_I2S_SendData(SPI1, tx_data);
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    ;
  return SPI_I2S_ReceiveData(SPI1);
}

uint16_t Hard_SPI_TransmitReceive(uint16_t tx_data) {
  uint16_t rx_data = 0;
  uint8_t *p = (uint8_t *)&tx_data;
  CS_LOW();
  Hard_SPI1_ReadWriteByte(p[1]);
  Hard_SPI1_ReadWriteByte(p[0]);
  rx_data |= Hard_SPI1_ReadWriteByte(0xff) << 8;
  rx_data |= Hard_SPI1_ReadWriteByte(0xff);

  CS_HIGH();

  return rx_data;
}

// after the read, data contains the data from the addressed register
uint16_t ADS_Prog_Read(uint8_t addr) {
  uint16_t tx_data = ((addr << 1 & 0xfe) << 8) | 0x00;
  uint16_t rx_data = 0;

#ifdef USE_SOFT_SPI
  rx_data = Soft_SPI_TransmitReceive(tx_data);
#else
  rx_data = Hard_SPI_TransmitReceive(tx_data);
#endif
  return rx_data;
}

// after the write, data should contain the data (byte) written to the addressed
// register (check equality for evaluation)
uint16_t ADS_Prog_Write(uint8_t addr, uint8_t data) {
  uint16_t tx_data = ((addr << 1 | 0x01) << 8) | data;
  uint16_t rx_data = 0;

#ifdef USE_SOFT_SPI
  rx_data = Soft_SPI_TransmitReceive(tx_data);
#else
  rx_data = Hard_SPI_TransmitReceive(tx_data);
#endif

  return rx_data;
}

uint16_t ADS_Cmd_Write(uint8_t cmd) {
  uint16_t tx_data = (cmd << 8) | 0x00;
  uint16_t rx_data = 0;

#ifdef USE_SOFT_SPI
  rx_data = Soft_SPI_TransmitReceive(tx_data);
#else
  rx_data = Hard_SPI_TransmitReceive(tx_data);
#endif

  return rx_data;
}

void ADS_Read_All_Raw(uint16_t * data) {
  for (int i = 0; i < CHNS_NUM_READ; i++) {

    data[i] = ADS_Cmd_Write(CONT);
  }
}

void ADS8688_Init(void) {
  uint16_t ret;

#ifdef USE_SOFT_SPI
  Soft_SPI1_Init();
#else
  Hard_SPI1_Init();
#endif

  TMUX_Init();
  // ADS8688 Reset
  GPIO_WriteBit(GPIOA, GPIO_Pin_3, Bit_RESET);
  Delay_Ms(10);
  GPIO_WriteBit(GPIOA, GPIO_Pin_3, Bit_SET);

  ret = ADS_Cmd_Write(RST);
  printf("RST: %x\r\n", ret);
  Delay_Ms(10);

  ret = ADS_Cmd_Write(CONT);
  printf("CONT: %x\r\n", ret);
  Delay_Ms(10);
  ret = ADS_Prog_Write(AUTO_SEQ_EN, 0xff);
  ret = ADS_Prog_Read(AUTO_SEQ_EN);
  printf("AUTO SEQ EN: %x\r\n", ret);
  Delay_Ms(10);
  ret = ADS_Prog_Write(FEATURE_SELECT, 0x03);
  ret = ADS_Prog_Read(FEATURE_SELECT);
  printf("FEATURE_SELECT: %x\r\n", ret);
  Delay_Ms(10);
  ADS_Prog_Write(CHN_0_RANGE, 0x06);
  ADS_Prog_Write(CHN_1_RANGE, 0x06);
  ADS_Prog_Write(CHN_2_RANGE, 0x06);
  ADS_Prog_Write(CHN_3_RANGE, 0x06);
  ADS_Prog_Write(CHN_4_RANGE, 0x06);
  ADS_Prog_Write(CHN_5_RANGE, 0x06);
  ADS_Prog_Write(CHN_6_RANGE, 0x06);
  ADS_Prog_Write(CHN_7_RANGE, 0x06);
  ret = ADS_Cmd_Write(AUTO_RST);
  printf("AUTO_RST: %x\r\n", ret);
}
