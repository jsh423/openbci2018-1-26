#ifndef __ADS1299_H
#define __ADS1299_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 

//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//////////////////////////////////////////////////////////////////////////////////////////////////////////


#define	ADS1299_CS0 		PCout(9)  		//ADS1299的片选信号
#define ADS1299_CS1			PCout(10)
#define ADS1299_CS2			PBout(5)
#define ADS1299_CS3			PBout(6)
#define ADS1299_RST2		PBout(7)
#define ADS1299_RST3		PBout(8)

#define ADS1299_RST1			PCout(11)
#define ADS1299_PWDN1			PCout(12)
#define ADS1299_PWDN2			PBout(9)
#define ADS1299_PWDN3			PDout(3)

#define ADS1299_RST0		PCout(7)
#define ADS1299_START   PCout(6)
#define ADS1299_PWDN0	PCout(8)
#define LED0 PBout(1)   //LED0
#define ADS1299_ID 0X3E									   	   

//void NRF24L01_Init(void);//初始化
void ADS1299_Init(void);
void ADS1299_WREG(u8 address,u8 value);
u8 ADS1299_PREG(u8 reg);
u8 ADS1299_PREGS(void);
void ADS1299_RDATA(u8 *p);
void ADS1299_IT(void);
u8 ADS1299_Check(void);
void ADS1299_CHANGE_CHANEL(u8 n,u8 sw);
void Recev_Data(void);
//void led_task(void *pdata);
//extern u8 adc_buf2[20][27];
extern void *Sem_Task_LED2;



//SPI Command Definition Byte Assignments (Datasheet, p35)
#define _WAKEUP 0x02 // Wake-up from standby mode
#define _STANDBY 0x04 // Enter Standby mode
#define _RESET 0x06 // Reset the device registers to default
#define _START 0x08 // Start and restart (synchronize) conversions
#define _STOP 0x0A // Stop conversion
#define _RDATAC 0x10 // Enable Read Data Continuous mode (default mode at power-up)
#define _SDATAC 0x11 // Stop Read Data Continuous mode
#define _RDATA 0x12 // Read data by command; supports multiple read back


//Register Addresses
#define ID 0x00
#define CONFIG1 0x01
#define CONFIG2 0x02
#define CONFIG3 0x03
#define LOFF 0x04
#define CH1SET 0x05
#define CH2SET 0x06
#define CH3SET 0x07
#define CH4SET 0x08
#define CH5SET 0x09
#define CH6SET 0x0A
#define CH7SET 0x0B
#define CH8SET 0x0C
#define BIAS_SENSP 0x0D
#define BIAS_SENSN 0x0E
#define LOFF_SENSP 0x0F
#define LOFF_SENSN 0x10
#define LOFF_FLIP 0x11
#define LOFF_STATP 0x12
#define LOFF_STATN 0x13
#define GPIO 0x14
#define MISC1 0x15
#define MISC2 0x16
#define CONFIG4 0x17

#endif


//#endif
