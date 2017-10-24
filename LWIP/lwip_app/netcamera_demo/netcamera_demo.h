#ifndef __TCP_SERVER_DEMO_H
#define __TCP_SERVER_DEMO_H
#include "sys.h"
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//网络摄像头程序   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/2/24
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	  

#define NETCAM_FIFO_NUM			100			//定义FIFO数量
#define NETCAM_LINE_SIZE		4096		//定义行大小(*4字节)

INT8U netcamera_init(void);		            //网络摄像头初始化(创建网络摄像头线程
u8 netcam_fifo_read(u32 **buf);
u8 netcam_fifo_write(u32 *buf);
u8 netmem_malloc(void);
void netmem_free(void);
u8 camera_init(void);
void camera_deinit(void);
void netcam_resolu_set(u8 resol);

#endif

