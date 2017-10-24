#ifndef __TCP_SERVER_DEMO_H
#define __TCP_SERVER_DEMO_H
#include "sys.h"
#include "includes.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//NETCONN API��̷�ʽ��TCP���������Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/2/24
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   

#define NETCAM_FIFO_NUM			60			//����FIFO����
#define NETCAM_LINE_SIZE		99	//�����д�С(*4�ֽ�)
 
#define TCP_SERVER_RX_BUFSIZE	2000		//����tcp server���������ݳ���
#define TCP_SERVER_PORT			8088	//����tcp server�Ķ˿�
#define LWIP_SEND_DATA			0X80	//���������ݷ���

extern u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
extern u8 tcp_server_flag;			//TCP���������ݷ��ͱ�־λ
extern u8 Num;

INT8U openbci_init(void);		//openbci�����ʼ��(����TCP�������߳�)
u8 netcam_fifo_read(u8 **buf);
u8 netcam_fifo_write(u8 *buf);
u8 netmem_malloc(void);
void netmem_free(void);
//void openbci_init(void);
#endif

