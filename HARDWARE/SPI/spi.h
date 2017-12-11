#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//SPI��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/16
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

extern SPI_HandleTypeDef SPI5_Handler;  //SPI5���
extern SPI_HandleTypeDef SPI2_Handler;  //SPI2���
extern DMA_HandleTypeDef   DMASPIRx_Handler;        //DMA���

extern void (*spi2_rx_callback)(void);

void SPI5_Init(void);
void SPI5_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI5_ReadWriteByte(u8 TxData);
//void BCI_DMA_Init(u32 mem0addr,u32 mem1addr,u16 memsize,u32 memblen,u32 meminc);
void BCI_DMA_Init(u8 mem0addr,u8 mem1addr,u8 memsize);
void ADS1299_DMA_Start(void);
void ADS1299_DMA_STOP(void);
void SPI2_Init(void);
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI2_ReadWriteByte(u8 TxData);
#endif
