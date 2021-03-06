#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//SPI驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/16
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

extern SPI_HandleTypeDef SPI3_Handler;  //SPI5句柄
extern SPI_HandleTypeDef SPI2_Handler;  //SPI2句柄
extern DMA_HandleTypeDef   DMASPIRx_Handler;        //DMA句柄

extern void (*spi2_rx_callback)(void);

void SPI3_Init(void);
void SPI3_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI3_WriteByte(u8 TxData);
//void BCI_DMA_Init(u32 mem0addr,u32 mem1addr,u16 memsize,u32 memblen,u32 meminc);
void BCI_DMA_Init(u8 mem0addr,u8 mem1addr,u8 memsize);
void ADS1299_DMA_Start(void);
void ADS1299_DMA_STOP(void);
void SPI2_Init(void);
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI2_ReadWriteByte(u8 TxData);
#endif
