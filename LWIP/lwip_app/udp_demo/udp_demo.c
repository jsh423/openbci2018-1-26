#include "udp_demo.h" 
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h" 
#include "ads1299.h"
#include "tca6424a.h"
#include "tcp_server_demo.h" 
//////////////////////////////////////////////////////////////////////////////////	 

//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
//UDP接收数据缓冲区
//u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP接收数据缓冲区 
//UDP发送数据内容
//u8 *tcp_server_sendbuf;//="Apollo STM32F4/F7 UDP demo send data\r\n";

//UDP 测试全局状态标记变量
//bit7:没有用到
//bit6:0,没有收到数据;1,收到数据了.
//bit5:0,没有连接上;1,连接上了.
//bit4~0:保留
u8 udp_demo_flag;

 //u32 len;
u8 sendsetbuf1[5]={0xa0,3,0x11,00,0xc0};
u8 sendfirbuf1[5]={0xa0,3,0x12,0,0xc0};
u8 setnorthbuf1[5]={0xa0,3,0x13,0,0xc0};
u8 restestbuf1[68];//={0xa0,66,0x33,0,0,0,0,0
//设置远端IP地址
void udp_demo_set_remoteip(void)
{
	u8 *tbuf;
	//u16 xoff;
	u8 key;
//	LCD_Clear(WHITE);
//	POINT_COLOR=RED;
//	LCD_ShowString(30,30,200,16,16,"Apollo STM32F4/F7");
//	LCD_ShowString(30,50,200,16,16,"UDP Test");
//	LCD_ShowString(30,70,200,16,16,"Remote IP Set");  
//	LCD_ShowString(30,90,200,16,16,"KEY0:+  KEY2:-");  
//	LCD_ShowString(30,110,200,16,16,"KEY_UP:OK");  
	tbuf=mymalloc(SRAMIN,100);	//申请内存
	if(tbuf==NULL)return;
	//前三个IP保持和DHCP得到的IP一致
	lwipdev.remoteip[0]=lwipdev.ip[0];
	lwipdev.remoteip[1]=lwipdev.ip[1];
	lwipdev.remoteip[2]=lwipdev.ip[2]; 
	//sprintf((char*)tbuf,"Remote IP:%d.%d.%d.",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2]);//远端IP
	//LCD_ShowString(30,150,210,16,16,tbuf); 
	//POINT_COLOR=BLUE;
	//xoff=strlen((char*)tbuf)*8+30;
	//LCD_ShowxNum(xoff,150,lwipdev.remoteip[3],3,16,0); 
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)break;
		else if(key)
		{
			if(key==KEY0_PRES)lwipdev.remoteip[3]++;//IP增加
			if(key==KEY2_PRES)lwipdev.remoteip[3]--;//IP减少
			//LCD_ShowxNum(xoff,150,lwipdev.remoteip[3],3,16,0X80);//显示新IP
		}
	}
	myfree(SRAMIN,tbuf); 
} 

////相应变量的内存申
//void Data_Process_udp(void)
//{
//	//u32 len;
//	u8 res;
//	u16 Res;
//	if(udp_demo_recvbuf[0]==0xa0)
//	{
//		switch(udp_demo_recvbuf[1])
//		{
//		case 0x1:  //数据采集启动
//			res=udp_demo_recvbuf[2];
//			if(res==1) ADS1299_Command(_START);
//				//ADS1299_START=1;//ADS1299芯片启动
//		else if(res==0) ADS1299_Command(_STOP);
//			//ADS1299_START=0;
//		break;
//		case 0x11: //采样率设置
//			//u8 buf[5];
//			res=udp_demo_recvbuf[2];
//			if(res!=sps) 
//			{
//				Set_Sps(res);
//				sps=res;
//			}
//			sendsetbuf1[2]=0x11;
//			sendsetbuf1[3]=0x00;
//			len=sendsetbuf1[1]+2;
//			tcp_server_sendbuf=sendsetbuf1;
//			udp_demo_flag|=1<<7;
//			parameter[6]=res;
//			//if(res==0x06) 
//		break;
//		
//		case 0x12://陷波设置
//			tcp_server_sendbuf=sendsetbuf1;
//			len=sendsetbuf1[1]+2;
//			udp_demo_flag|=1<<7;
//		break;
//		
//		case 0x13: //滤波设置
//			tcp_server_sendbuf=setnorthbuf1;
//			len=setnorthbuf1[1]+2;
//			udp_demo_flag|=1<<7;
//		break;
//		
//		case 0x21://设备参数查询
//			udp_demo_flag|=1<<7;
//			tcp_server_sendbuf=parameter;
//			len=parameter[1]+2;
//		break;
//		case 0x31://单个通道阻抗测试
//			ADS1299_Command(_STOP);
//			ADS1299_START=0;
//			Adg731_Write_Rg(0x00);
//			TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,0X800001);
//			//AD5933_Start();
//		//	Res=AD5933_RES();
//			TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,0X000000);
//			Adg731_Write_Rg(0x80);
//			restestbuf1[0]=0xa0;
//			restestbuf1[1]=4;
//			restestbuf1[2]=0x31;
//			restestbuf1[3]=(u8)(Res>>8);
//			restestbuf1[4]=(u8)(Res);
//			restestbuf1[5]=0xc0;
//			res=udp_demo_recvbuf[2];
//			len=restestbuf1[1]+2;
//			udp_demo_flag|=1<<7;
//			tcp_server_sendbuf=restestbuf1;
//		
//		break;
//		case 0x32://N个通道阻抗同时测试
//			ADS1299_Command(_STOP);
//			ADS1299_START=0;
//			Adg731_Write_Rg(0x00);
//			TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,0X800001);
//		//	AD5933_Start();
//		//	Res=AD5933_RES();
//			TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,0X000000);
//			Adg731_Write_Rg(0x80);
//			restestbuf1[0]=0xa0;
//			restestbuf1[1]=66;
//			restestbuf1[2]=0x32;
//			restestbuf1[3]=(u8)(Res>>8);
//			restestbuf1[4]=(u8)(Res);
//			restestbuf1[67]=0xc0;
//			len=restestbuf1[1]+2;
//			udp_demo_flag|=1<<7;
//			tcp_server_sendbuf=restestbuf1;
//		break;
//		default :
//		break;
//		}
//	}
//}


////UDP测试
//void udp_demo_test(void)
//{
// 	err_t err;
//	struct udp_pcb *udppcb;  	//定义一个TCP服务器控制块
//	struct ip_addr rmtipaddr;  	//远端ip地址
// 	
//	u8 *tbuf;
// 	u8 key;
//	u8 res=0;		
//	u8 t=0; 
// 	
//	//udp_demo_set_remoteip();//先选择IP
////	LCD_Clear(WHITE);	//清屏
////	POINT_COLOR=RED; 	//红色字体
////	LCD_ShowString(30,30,200,16,16,"Apollo STM32F4/F7");
////	LCD_ShowString(30,50,200,16,16,"UDP Test");
////	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");  
////	LCD_ShowString(30,90,200,16,16,"KEY0:Send data");  
////	LCD_ShowString(30,110,200,16,16,"KEY_UP:Quit");  
//	tbuf=mymalloc(SRAMIN,200);	//申请内存
//	if(tbuf==NULL)return ;		//内存申请失败了,直接退出
////	sprintf((char*)tbuf,"Local IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//服务器IP
////	LCD_ShowString(30,130,210,16,16,tbuf);  
////	sprintf((char*)tbuf,"Remote IP:%d.%d.%d.%d",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//远端IP
////	LCD_ShowString(30,150,210,16,16,tbuf);  
////	sprintf((char*)tbuf,"Remote Port:%d",UDP_DEMO_PORT);//客户端端口号
////	LCD_ShowString(30,170,210,16,16,tbuf);
////	POINT_COLOR=BLUE;
////	LCD_ShowString(30,190,210,16,16,"STATUS:Disconnected"); 
//	udppcb=udp_new();
//	netmem_malloc();
//	if(udppcb)//创建成功
//	{ 
//		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
//		err=udp_connect(udppcb,&rmtipaddr,UDP_DEMO_PORT);//UDP客户端连接到指定IP地址和端口号的服务器
//		if(err==ERR_OK)
//		{
//			err=udp_bind(udppcb,IP_ADDR_ANY,UDP_DEMO_PORT);//绑定本地IP地址与端口号
//			if(err==ERR_OK)	//绑定完成
//			{
//				udp_recv(udppcb,udp_demo_recv,NULL);//注册接收回调函数 
////				//LCD_ShowString(30,190,210,16,16,"STATUS:Connected   ");//标记连接上了(UDP是非可靠连接,这里仅仅表示本地UDP已经准备好)
////				POINT_COLOR=RED;
////				LCD_ShowString(30,210,lcddev.width-30,lcddev.height-190,16,"Receive Data:");//提示消息		
////				POINT_COLOR=BLUE;//蓝色字体
//			}else res=1;
//		}else res=1;		
//	}else res=1;
//	while(res==0)
//	{
//		//key=KEY_Scan(0);
//		//if(key==WKUP_PRES)break;
//		//if(key==KEY0_PRES)//KEY0按下了,发送数据
//		if(udp_demo_flag&1<<7)//有数据要发送)//KEY0按下了,发送数据
//		{
//			udp_demo_senddata(udppcb);
//		}
//		if(udp_demo_flag&1<<6)//是否收到数据?
//		{
//			//LCD_Fill(30,230,lcddev.width-1,lcddev.height-1,WHITE);//清上一次数据
//			Data_Process_udp();
//			//LCD_ShowString(30,230,lcddev.width-30,lcddev.height-230,16,udp_demo_recvbuf);//显示接收到的数据			
//			udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
//		} 
//		lwip_periodic_handle();
//		delay_ms(2);
//		t++;
//		if(t==200)
//		{
//			t=0;
//			LED0=!LED0;
//		}
//	}
//	udp_demo_connection_close(udppcb); 
//	myfree(SRAMIN,tbuf);
//	netmem_free();
//} 

////UDP服务器回调函数
//void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
//{
//	u32 data_len = 0;
//	struct pbuf *q;
//	if(p!=NULL)	//接收到不为空的数据时
//	{
//		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //数据接收缓冲区清零
//		for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
//		{
//			//判断要拷贝到UDP_DEMO_RX_BUFSIZE中的数据是否大于UDP_DEMO_RX_BUFSIZE的剩余空间，如果大于
//			//的话就只拷贝UDP_DEMO_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
//			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//拷贝数据
//			else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
//			data_len += q->len;  	
//			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
//		}
//		upcb->remote_ip=*addr; 				//记录远程主机的IP地址
//		upcb->remote_port=port;  			//记录远程主机的端口号
//		lwipdev.remoteip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
//		lwipdev.remoteip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
//		lwipdev.remoteip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
//		lwipdev.remoteip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1 
//		udp_demo_flag|=1<<6;	//标记接收到数据了
//		pbuf_free(p);//释放内存
//	}else
//	{
//		udp_disconnect(upcb); 
//	} 
//} 
////UDP服务器发送数据
//void udp_demo_senddata(struct udp_pcb *upcb)
//{
//	struct pbuf *ptr;
//	ptr=pbuf_alloc(PBUF_TRANSPORT,len,PBUF_POOL); //申请内存
//	if(ptr)
//	{
//		ptr->payload=(void*)tcp_server_sendbuf; 
//		udp_send(upcb,ptr);	//udp发送数据 
//		pbuf_free(ptr);//释放内存
//		  udp_demo_flag&=~(1<<7);	    //清除数据发送标志
//	} 
//} 
////关闭tcp连接
//void udp_demo_connection_close(struct udp_pcb *upcb)
//{
//	udp_disconnect(upcb); 
//	udp_remove(upcb);		//断开UDP连接 
//}

























