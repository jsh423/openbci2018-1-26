#ifndef __TCP_SERVER_DEMO_H
#define __TCP_SERVER_DEMO_H
#include "sys.h"
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//NETCONN API编程方式的TCP服务器测试代码	   
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

#define NETCAM_FIFO_NUM			60			//定义FIFO数量
#define NETCAM_LINE_SIZE		99	//定义行大小(*4字节)
 
#define TCP_SERVER_RX_BUFSIZE	2000		//定义tcp server最大接收数据长度
#define TCP_SERVER_PORT			8088	//定义tcp server的端口
#define LWIP_SEND_DATA			0X80	//定义有数据发送

extern u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
extern u8 tcp_server_flag;			//TCP服务器数据发送标志位
extern u8 Num;

INT8U openbci_init(void);		//openbci任务初始化(创建TCP服务器线程)
u8 netcam_fifo_read(u8 **buf);
u8 netcam_fifo_write(u8 *buf);
u8 netmem_malloc(void);
void netmem_free(void);
//void openbci_init(void);
#endif

