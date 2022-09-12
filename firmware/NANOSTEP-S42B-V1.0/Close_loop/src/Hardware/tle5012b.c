#include "tle5012b.h"



void TLE5012B_Init()
{
    // Configure chip select pin
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = NSS_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(NSS_GPIO_Port, &GPIO_InitStruct);

    // Angle range 360°
    WriteValue(WRITE_MOD2_VALUE, MOD2_VALUE);
}


bool CheckHealth()
{
  // Read status register
  uint16_t state = ReadState();
 
  // Check for Magnitude out of limit error
  if(state&0x0080)
  {

  }
  return false;
}

void delay_us(int delay)
{
  for(int i=0;i<delay;i++)
    __ASM volatile ("NOP");
}

void bufferData()//Required when we are reading buffered data
{
    NSS_L;
    delay_us(1);
    NSS_H;
}

//Minimum wait that is working is 10. Using 200 for some reserve. 
#define max_wait 200
uint16_t ReadValue(uint16_t RegAdd)
{
    static uint16_t data;
    int loop;

    NSS_L;
    SPI_TX_PP;
    for(loop=0;loop<max_wait && LL_SPI_IsActiveFlag_TXE(SPI1)==0;loop++);
    if(loop==max_wait)goto load_failed; 

    LL_SPI_TransmitData16(SPI1,RegAdd);
    for(loop=0;loop<max_wait && LL_SPI_IsActiveFlag_RXNE(SPI1)==0;loop++);
    if(loop==max_wait)goto load_failed; 

    LL_SPI_ReceiveData16(SPI1);//read old data first and discard
    SPI_TX_OD;
    for(loop=0;loop<max_wait && LL_SPI_IsActiveFlag_TXE(SPI1)==0;loop++);
    if(loop==max_wait)goto load_failed; 

    delay_us(1);//130ns delay
    LL_SPI_TransmitData16(SPI1,0xFFFF);//Sending clock signal 
    for(loop=0;loop<max_wait && LL_SPI_IsActiveFlag_RXNE(SPI1)==0;loop++);
    if(loop==max_wait)goto load_failed; 

    data=LL_SPI_ReceiveData16(SPI1)&0x7FFF;

load_failed:    //If load failed - return old data
    NSS_H;
    return data;
}

uint16_t ReadValue_16bit(uint16_t RegAdd)
{
    static uint16_t data;

    NSS_L;
    SPI_TX_PP;
    while(LL_SPI_IsActiveFlag_TXE(SPI1)==0);

    LL_SPI_TransmitData16(SPI1,RegAdd);
    while(LL_SPI_IsActiveFlag_RXNE(SPI1)==0);

    LL_SPI_ReceiveData16(SPI1);//read old data first and discard
    SPI_TX_OD;
    while(LL_SPI_IsActiveFlag_TXE(SPI1)==0);

    delay_us(1);//130ns delay
    LL_SPI_TransmitData16(SPI1,0xFFFF);//Sending clock signal 
    while(LL_SPI_IsActiveFlag_RXNE(SPI1)==0);

    data=LL_SPI_ReceiveData16(SPI1);
    NSS_H;
    return data;
}



void WriteValue(uint16_t RegAdd,uint16_t RegValue)
{
  NSS_L;
  while(LL_SPI_IsActiveFlag_TXE(SPI1)==0);
  LL_SPI_TransmitData16(SPI1,RegAdd);
  while(LL_SPI_IsActiveFlag_RXNE(SPI1)==0);
  LL_SPI_ReceiveData16(SPI1);
  while(LL_SPI_IsActiveFlag_TXE(SPI1)==0);
  LL_SPI_TransmitData16(SPI1,RegValue);
  while(LL_SPI_IsActiveFlag_RXNE(SPI1)==0);
  LL_SPI_ReceiveData16(SPI1);
  NSS_H;
}


uint16_t ReadState(void)
{
  return (ReadValue(READ_STATUS));
}


uint16_t ReadAngle(void)
{
  return (ReadValue(READ_ANGLE_VALUE));
}