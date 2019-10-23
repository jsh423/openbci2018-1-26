#ifndef __TCP_SERVER_DEMO_H
#define __TCP_SERVER_DEMO_H
#include "sys.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip_comm.h"
//////////////////////////////////////////////////////////////////////////////////	 

//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
#define TCP_SERVER_RX_BUFSIZE	200		//定义tcp server最大接收数据长度
#define TCP_SERVER_PORT			8088	//定义tcp server的端口
 
#define NETCAM_FIFO_NUM			100			//定义FIFO数量
#define NETCAM_LINE_SIZE		105	//定义行大小(*4字节)

extern u8 tcp_server_flag;	
extern u8 Num;
extern u8 *tcp_server_sendbuf;
extern u16 len;
extern u8 Ad_Flag;
//extern u32 len;
//extern u8 sendsetbuf[5]={0xa0,3,0x11,00,0xc0};
//extern u8 setfirbuf[5]={0xa0,3,0x12,0,0xc0};
//extern u8 setnorthbuf[5]={0xa0,3,0x13,0,0xc0};
//u 8 restestbuf[6]={0xa0,4,0x31,0,0,0xc0};
//extern u8 restestbuf[68];//={0xa0,66,0x33,0,0,0,0,0

//tcp服务器连接状态
enum tcp_server_states
{
	ES_TCPSERVER_NONE = 0,		//没有连接
	ES_TCPSERVER_ACCEPTED,		//有客户端连接上了 
	ES_TCPSERVER_CLOSING,		//即将关闭连接
}; 
//LWIP回调函数使用的结构体
struct tcp_server_struct
{
	u8 state;               //当前连接状
	struct tcp_pcb *pcb;    //指向当前的pcb
	struct pbuf *p;         //指向接收/或传输的pbuf
}; 
void hardware_init(void);
void udp_demo_test(void);
void tcp_server_test(void);//TCP Server测试函数
err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err);
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void tcp_server_error(void *arg,err_t err);
err_t tcp_server_usersent(struct tcp_pcb *tpcb);
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void tcp_server_remove_timewait(void);
void Data_Process(void);
u8 netcam_fifo_write(u8 *buf);
#endif 
