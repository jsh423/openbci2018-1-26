#include "tcp_server_demo.h" 
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h"  
#include "ads1299.h"
#include "adg731.h"
#include "tca6424a.h"
#include "ad5933.h"
//////////////////////////////////////////////////////////////////////////////////	 

//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
//TCP Server接收数据缓冲区
u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	
//TCP服务器发送数据内容
u8 *tcp_server_sendbuf;//="Apollo STM32F4/F7 TCP Server send data\r\n";


//TCP Server 测试全局状态标记变量
//bit7:0,没有数据要发送;1,有数据要发送
//bit6:0,没有收到数据;1,收到数据了.
//bit5:0,没有客户端连接上;1,有客户端连接上了.
//bit4~0:保留
u8 tcp_server_flag;	 
 u8 Num;
 u32 len;
u8 sendsetbuf[5]={0xa0,3,0x11,00,0xc0};
u8 setfirbuf[5]={0xa0,3,0x12,0,0xc0};
u8 setnorthbuf[5]={0xa0,3,0x13,0,0xc0};
//u8 restestbuf[6]={0xa0,4,0x31,0,0,0xc0};
u8 restestbuf[68];//={0xa0,66,0x33,0,0,0,0,0

 struct netconn *newconn=0;		            //NET CAM TCP网络连接结构体指针 
u8 *netcam_line_buf0;					    //定义行缓存0  
u8 *netcam_line_buf1;					    //定义行缓存1   

//NET CAM FIFO
vu16 netcamfifordpos=0;					    //FIFO读位置
vu16 netcamfifowrpos=0;					    //FIFO写位置
u8 *netcamfifobuf;	    //定义NETCAM_FIFO_SIZE个接收FIFO	



//读取FIFO
//buf:数据缓存区首地址
//返回值:0,没有数据可读;
//      1,读到了1个数据块
u8 netcam_fifo_read(u8 **buf)
{
//	if(netcamfifordpos==netcamfifowrpos)return 0;
//	netcamfifordpos++;		//读位置加1
//	if(netcamfifordpos>=NETCAM_FIFO_NUM)netcamfifordpos=0;//归零 
	*buf=netcamfifobuf;
//	netcam_line_buf0[0]=0xa0;
//	netcam_line_buf0[NETCAM_LINE_SIZE-1]=0xc0;
//	if(Index++<0xff) netcam_line_buf0[1]=Index;
//	else Index=0;
//	*buf=netcam_line_buf0;
	return 1;
}
#if 1
//写一个FIFO
//buf:数据缓存区首地址
//返回值:0,写入成功;
//      1,写入失败
u8 netcam_fifo_write(u8 *buf)
{
	u16 i;
	u16 temp=netcamfifowrpos;	//记录当前写位置
	u16 res;
	
	//if(netcamfifowrpos>=NETCAM_FIFO_NUM)	netcamfifowrpos=0;//归零  
//	else
//	{
	//netcamfifobuf[netcamfifowrpos*NETCAM_LINE_SIZE+i]=buf[i];//拷贝数据
	//for(i=0;i<NETCAM_LINE_SIZE;i++)	netcamfifobuf[i]=buf[i];//拷贝数据
		//for(i=0;i<NETCAM_LINE_SIZE;i++)	netcamfifobuf[netcamfifowrpos*NETCAM_LINE_SIZE+i]=buf[i];//拷贝数据
		if(openvibeflag) res=NETCAM_LINE_SIZE-2;
		else res=NETCAM_LINE_SIZE;
	
		memcpy(&netcamfifobuf[netcamfifowrpos*res],buf,res);
//		else
//		{
//			memcpy(&netcamfifobuf[netcamfifowrpos*NETCAM_LINE_SIZE],buf,NETCAM_LINE_SIZE);
//		}
		netcamfifowrpos++;			//写位置加1
		if(netcamfifowrpos<NETCAM_FIFO_NUM)return 0;
		else
		{
			netcam_fifo_read(&tcp_server_sendbuf);
			len=netcamfifowrpos*res;
			netcamfifowrpos=0;
			//tcp_server_flag|=(1<<7);//有数据要发送	//netcam_line_buf1写入FIFO 
			return temp+1;
		}
	//}
}
#endif

#if 0
u8 netcam_fifo_write(u8 *buf)
{
	u16 i;
	u16 temp=netcamfifowrpos;	//记录当前写位置
//	netcamfifowrpos++;			//写位置加1
//	if(netcamfifowrpos>=NETCAM_FIFO_NUM)netcamfifowrpos=0;//归零  
//	if(netcamfifordpos==netcamfifowrpos)
//	{
//		netcamfifowrpos=temp;	//还原原来的写位置,此次写入失败
//		//printf("shit:%d\r\n",temp);
//		return 1;	
//	}
	for(i=0;i<NETCAM_LINE_SIZE-3;i++)netcam_line_buf0[i+2]=buf[i];//拷贝数据
	return 0;
}   
#endif

//相应变量的内存申请
//返回值:0 成功；其他 失败
u8 netmem_malloc(void)
{
//    u16 t=0;
    //netcam_line_buf0=mymalloc(SRAMIN,NETCAM_LINE_SIZE);
	//netcam_line_buf1=mymalloc(SRAMIN,NETCAM_LINE_SIZE);

    //给FIFO申请内存
	netcamfifobuf=mymalloc(SRAMIN,NETCAM_LINE_SIZE*NETCAM_FIFO_NUM);
//    for(t=0;t<NETCAM_FIFO_NUM;t++) 
//	{
//		netcamfifobuf[t]=mymalloc(SRAMEX,NETCAM_LINE_SIZE);
//	} 
		if(!netcamfifobuf)//||!netcam_line_buf0)//内存申请失败  
    {
        myfree(SRAMIN,netcamfifobuf);
		//netmem_free();//释放内存
        return 1;
    }
    return 0;
//    if(!netcamfifobuf[NETCAM_FIFO_NUM-1]||!netcam_line_buf1||!netcam_line_buf0)//内存申请失败  
//    {
//        netmem_free();//释放内存
//        return 1;
//    }
//    return 0;
}

//相应变量的内存释放
void netmem_free(void)
{
   // u16 t=0;
   // myfree(SRAMIN,netcam_line_buf0);
    //myfree(SRAMIN,netcam_line_buf1);
    //释放FIFO的内存
    //for(t=0;t<NETCAM_FIFO_NUM;t++) 
	//{
        myfree(SRAMIN,netcamfifobuf);
	//}  
}
 //ADS1299数据接收回调函数
static void spi2_rx_dma_callback(void)
{    
	//if(DMA1_Stream3->CR&(1<<19))netcam_fifo_write(netcam_line_buf0);	//netcam_line_buf0写入FIFO
	//else netcam_fifo_write(netcam_line_buf1);	
	//netcam_fifo_write(adc_buf2);
	//tcp_server_flag|=(1<<7);//有数据要发送
}
//openbci初始化
void hardware_init(void)
{
	
	
	
	
	//while(1) 									//初始化tcp_server(创建tcp_server程)
	{
		//LCD_ShowString(30,110,200,20,16,"ADS1299 Check failed! "); 
		//delay_ms(500);
		//LCD_Fill(30,110,230,170,WHITE);
		if(ADS1299_Check()) delay_ms(100);
		//else break;
		
	}
	//LCD_ShowString(30,150,200,20,16,"ads1299 check Success!  "); 	//tcp服务器创建成功
	
	// spi2_rx_callback=spi2_rx_dma_callback;//接收数据回调函数
    // BCI_DMA_Init(netcam_line_buf0[0],netcam_line_buf1[0],NETCAM_LINE_SIZE); 
	//ADS1299_DMA_Config(DMA1_Stream3,DMA_CHANNEL_0);//初始化DMA
	//ADS1299_DMA_Start();
	
	ADS1299_IT();		//中断配置
}
 
//TCP Server 测试
void tcp_server_test(void)
{
	err_t err;  
	struct tcp_pcb *tcppcbnew;  	//定义一个TCP服务器控制块
	struct tcp_pcb *tcppcbconn;  	//定义一个TCP服务器控制块
	
	u8 *tbuf;
 	u8 key;
	u8 res=0,res1=0;		
	u16 t=0; 
	
	LED1=1;
	//netmem_free();
	tbuf=mymalloc(SRAMIN,200);	//申请内存
	netmem_malloc();
	if(tbuf==NULL)return ;		//内存申请失败了,直接退出
	//sprintf((char*)tbuf,"Server IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//服务器IP
	//sprintf((char*)tbuf,"Server Port:%d",TCP_SERVER_PORT);//服务器端口号 
	tcppcbnew=tcp_new();	//创建一个新的pcb
	if(tcppcbnew)			//创建成功
	{ 
		err=tcp_bind(tcppcbnew,IP_ADDR_ANY,TCP_SERVER_PORT);	//将本地IP与指定的端口号绑定在一起,IP_ADDR_ANY为绑定本地所有的IP地址
		if(err==ERR_OK)	//绑定完成
		{
			tcppcbconn=tcp_listen(tcppcbnew); 			//设置tcppcb进入监听状态
			tcp_accept(tcppcbconn,tcp_server_accept); 	//初始化LWIP的tcp_accept的回调函数
		}else res=1;  
	}else res=1;
	delay_ms(100);
	hardware_init();
	if((tcp_server_flag&1<<5)&&(res==0))
	{
		res1=1;
	}
	while(res1==1)//如果已经连上
	{
//		while(res1==1)
//		{
		if(tcp_server_flag&1<<7)//有数据要发送)//KEY0按下了,发送数据
		{
			//netcam_fifo_read(&tcp_server_sendbuf);
			//tcp_server_sendbuf=netcamfifobuf;
            tcp_server_usersent(tcppcbnew);//发送数据
		}
		if(tcp_server_flag&1<<6)//是否收到数据?
		{
			Data_Process();
			//LCD_Fill(30,210,lcddev.width-1,lcddev.height-1,WHITE);//清上一次数据
			//LCD_ShowString(30,210,lcddev.width-30,lcddev.height-210,16,tcp_server_recvbuf);//显示接收到的数据	
			//此处对通信数据进行解析
			//包头包尾是否正确
			//命令字判断
			//21：设备参数上传
			//11：采样率设置
			//12:险波设置
			//13：滤波设置
			//01：AD数据采集
			tcp_server_flag&=~(1<<6);//标记数据已经被处理了.
		}
		
		//}
		if((tcp_server_flag&1<<5)==0)
		{
			res1=0;
			//LCD_Fill(30,170,lcddev.width-1,lcddev.height-1,WHITE);//清屏
		}
		lwip_periodic_handle();
		//delay_ms(2);
		LED1=0;
//		t++;
//		if(t==20000)
//		{
//			t=0;
//			
//			LED1=!LED1;
//		} 
	} 
	LED1=1;	
	tcp_server_connection_close(tcppcbnew,0);//关闭TCP Server连接
	tcp_server_connection_close(tcppcbconn,0);//关闭TCP Server连接 
	tcp_server_remove_timewait(); 
	memset(tcppcbnew,0,sizeof(struct tcp_pcb));
	memset(tcppcbconn,0,sizeof(struct tcp_pcb)); 
	myfree(SRAMIN,tbuf);
	netmem_free();
	
} 
//lwIP tcp_accept()的回调函数
err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err)
{
	err_t ret_err;
	struct tcp_server_struct *es; 
 	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);
	tcp_setprio(newpcb,TCP_PRIO_MIN);//设置新创建的pcb优先级
	es=(struct tcp_server_struct*)mem_malloc(sizeof(struct tcp_server_struct)); //分配内存
 	if(es!=NULL) //内存分配成功
	{
		es->state=ES_TCPSERVER_ACCEPTED;  	//接收连接
		es->pcb=newpcb;
		es->p=NULL;
		
		tcp_arg(newpcb,es);
		tcp_recv(newpcb,tcp_server_recv);	//初始化tcp_recv()的回调函数
		tcp_err(newpcb,tcp_server_error); 	//初始化tcp_err()回调函数
		tcp_poll(newpcb,tcp_server_poll,1);	//初始化tcp_poll回调函数
		tcp_sent(newpcb,tcp_server_sent);  	//初始化发送回调函数
		  
		tcp_server_flag|=1<<5;				//标记有客户端连上了
		lwipdev.remoteip[0]=newpcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(newpcb->remote_ip.addr>>8)&0xff;  	//IADDR3
		lwipdev.remoteip[2]=(newpcb->remote_ip.addr>>16)&0xff; 	//IADDR2
		lwipdev.remoteip[3]=(newpcb->remote_ip.addr>>24)&0xff; 	//IADDR1 
		ret_err=ERR_OK;
	}else ret_err=ERR_MEM;
	return ret_err;
}
//lwIP tcp_recv()函数的回调函数
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	err_t ret_err;
	u32 data_len = 0;
	struct pbuf *q;
  	struct tcp_server_struct *es;
	LWIP_ASSERT("arg != NULL",arg != NULL);
	es=(struct tcp_server_struct *)arg;
	if(p==NULL) //从客户端接收到空数据
	{
		LED1=1;
		es->state=ES_TCPSERVER_CLOSING;//需要关闭TCP 连接了
		es->p=p; 
		ret_err=ERR_OK;
	}else if(err!=ERR_OK)	//从客户端接收到一个非空数据,但是由于某种原因err!=ERR_OK
	{
		if(p)pbuf_free(p);	//释放接收pbuf
		ret_err=err;
		LED1=1;
	}else if(es->state==ES_TCPSERVER_ACCEPTED) 	//处于连接状态
	{
		if(p!=NULL)  //当处于连接状态并且接收到的数据不为空时将其打印出来
		{
			memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //数据接收缓冲区清零
			for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
			{
				//判断要拷贝到TCP_SERVER_RX_BUFSIZE中的数据是否大于TCP_SERVER_RX_BUFSIZE的剩余空间，如果大于
				//的话就只拷贝TCP_SERVER_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
				if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//拷贝数据
				else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
				data_len += q->len;  	
				if(data_len > TCP_SERVER_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
			}
			tcp_server_flag|=1<<6;	//标记接收到数据了
			lwipdev.remoteip[0]=tpcb->remote_ip.addr&0xff; 		//IADDR4
			lwipdev.remoteip[1]=(tpcb->remote_ip.addr>>8)&0xff; //IADDR3
			lwipdev.remoteip[2]=(tpcb->remote_ip.addr>>16)&0xff;//IADDR2
			lwipdev.remoteip[3]=(tpcb->remote_ip.addr>>24)&0xff;//IADDR1 
 			tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
			pbuf_free(p);  	//释放内存
			ret_err=ERR_OK;
		}
	}else//服务器关闭了
	{
		tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
		es->p=NULL;
		pbuf_free(p); //释放内存
		ret_err=ERR_OK;
		tcp_server_flag&=~(1<<5);//标记连接断开了
	}
	return ret_err;
}
//lwIP tcp_err函数的回调函数
void tcp_server_error(void *arg,err_t err)
{  
	LWIP_UNUSED_ARG(err);  
	tcp_server_flag&=~(1<<5);//标记连接断开了
	//printf("tcp error:%x\r\n",(u32)arg);
	if(arg!=NULL)mem_free(arg);//释放内存
} 

//LWIP数据发送，用户应用程序调用此函数来发送数据
//tpcb:TCP控制块
//返回值:0，成功；其他，失败
err_t tcp_server_usersent(struct tcp_pcb *tpcb)
{
    err_t ret_err;
	struct tcp_server_struct *es; 
	es=tpcb->callback_arg;
	if(es!=NULL)  //连接处于空闲可以发送数据
	{
//es->p=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)tcp_server_sendbuf),PBUF_POOL);	//申请内存 
       // pbuf_take(es->p,(char*)tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf));	//将tcp_server_sentbuf[]中的数据拷贝到es->p_tx中
		es->p=pbuf_alloc(PBUF_TRANSPORT, len,PBUF_POOL);	//申请内存 
        pbuf_take(es->p,(char*)tcp_server_sendbuf,len);	//将tcp_server_sentbuf[]中的数据拷贝到es->p_tx中
        tcp_server_senddata(tpcb,es);   //将tcp_server_sentbuf[]里面复制给pbuf的数据发送出去
        tcp_server_flag&=~(1<<7);	    //清除数据发送标志
        if(es->p!=NULL)pbuf_free(es->p);//释放内存
		ret_err=ERR_OK;
	}else
	{ 
		LED1=1;
		tcp_abort(tpcb);//终止连接,删除pcb控制块
		ret_err=ERR_ABRT;
		LED1=1;
	}
	return ret_err;
}

//lwIP tcp_poll的回调函数
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_server_struct *es; 
	es=(struct tcp_server_struct *)arg; 
    if(es->state==ES_TCPSERVER_CLOSING)//需要关闭连接?执行关闭操作
    {
        tcp_server_connection_close(tpcb,es);//关闭连接
		//tcp_server_flag&=~(1<<5);//标记连接断开了
		LED1=1;
    }
    ret_err=ERR_OK;
	return ret_err;
} 
//lwIP tcp_sent的回调函数(当从远端主机接收到ACK信号后发送数据)
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	struct tcp_server_struct *es;
	LWIP_UNUSED_ARG(len); 
	es = (struct tcp_server_struct *) arg;
	if(es->p)tcp_server_senddata(tpcb,es);//发送数据
	return ERR_OK;
} 
//此函数用来发送数据
void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
	struct pbuf *ptr;
	u16 plen;
	err_t wr_err=ERR_OK;
	while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb)))
	{
		ptr=es->p;
		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
		if(wr_err==ERR_OK)
		{ 
			plen=ptr->len;
			es->p=ptr->next;			//指向下一个pbuf
			if(es->p)pbuf_ref(es->p);	//pbuf的ref加一
			pbuf_free(ptr);
			tcp_recved(tpcb,plen); 		//更新tcp窗口大小
		}else if(wr_err==ERR_MEM)es->p=ptr;
        tcp_output(tpcb);		        //将发送缓冲队列中的数据立即发送出去
    }
    
} 
//关闭tcp连接
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
	tcp_close(tpcb);
	tcp_arg(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_recv(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);
	if(es)mem_free(es); 
	tcp_server_flag&=~(1<<5);//标记连接断开了
}
extern void tcp_pcb_purge(struct tcp_pcb *pcb);	//在 tcp.c里面 
extern struct tcp_pcb *tcp_active_pcbs;			//在 tcp.c里面 
extern struct tcp_pcb *tcp_tw_pcbs;				//在 tcp.c里面  
//强制删除TCP Server主动断开时的time wait
void tcp_server_remove_timewait(void)
{
	struct tcp_pcb *pcb,*pcb2; 
	u8 t=0;
	while(tcp_active_pcbs!=NULL&&t<200)
	{
		lwip_periodic_handle();	//继续轮询
		t++;
		delay_ms(10);			//等待tcp_active_pcbs为空  
	}
	pcb=tcp_tw_pcbs;
	while(pcb!=NULL)//如果有等待状态的pcbs
	{
		tcp_pcb_purge(pcb); 
		tcp_tw_pcbs=pcb->next;
		pcb2=pcb;
		pcb=pcb->next;
		memp_free(MEMP_TCP_PCB,pcb2);	
	}
}
void Data_Process(void)
{
	u8 res;
	u16 Res;
	if(tcp_server_recvbuf[0]==0xa0)
	{
		switch(tcp_server_recvbuf[1])
		{
		case 0x1:  //数据采集启动
			res=tcp_server_recvbuf[2];
			if(res==1) 
			{
//				ADS1299_RST1=0;
//				ADS1299_RST0=0;
//				ADS1299_RST2=0;
//				ADS1299_RST3=0;
//				delay_ms(5);
//				ADS1299_RST0=1;
//				ADS1299_RST1=1;
//				ADS1299_RST2=1;
//				ADS1299_RST3=1;
//				delay_ms(5);
				ADS1299_START=1;//ADS1299芯片启动
			}
		else if(res==0)
			ADS1299_START=0;
		break;
		case 0x11: //采样率设置
			//u8 buf[5];
			res=tcp_server_recvbuf[2];
			if(res!=sps) 
			{
				Set_Sps(res);
				sps=res;
			}
			sendsetbuf[2]=0x11;
			sendsetbuf[3]=0x00;
			len=sendsetbuf[1]+2;
			tcp_server_sendbuf=sendsetbuf;
			tcp_server_flag|=1<<7;
			parameter[6]=res;
			//if(res==0x06) 
		break;
		
		case 0x12://陷波设置
			tcp_server_sendbuf=setfirbuf;
			len=setfirbuf[1]+2;
			tcp_server_flag|=1<<7;
		break;
		
		case 0x13: //滤波设置
			tcp_server_sendbuf=setnorthbuf;
			len=setnorthbuf[1]+2;
			tcp_server_flag|=1<<7;
		break;
		
		case 0x21://设备参数查询
			tcp_server_flag|=1<<7;
			tcp_server_sendbuf=parameter;
			len=parameter[1]+2;
		break;
		case 0x31://单个通道阻抗测试
			ADS1299_START=0;
			Adg731_Write_Rg(0x00);
			TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,0X800001);
			AD5933_Start();
			Res=AD5933_RES();
			TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,0X000000);
			Adg731_Write_Rg(0x80);
			restestbuf[0]=0xa0;
			restestbuf[1]=4;
			restestbuf[2]=0x31;
			restestbuf[3]=(u8)(Res>>8);
			restestbuf[4]=(u8)(Res);
			restestbuf[5]=0xc0;
			res=tcp_server_recvbuf[2];
			len=restestbuf[1]+2;
			tcp_server_flag|=1<<7;
			tcp_server_sendbuf=restestbuf;
		
		break;
		case 0x32://N个通道阻抗同时测试
			ADS1299_START=0;
			Adg731_Write_Rg(0x00);
			TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,0X800001);
			AD5933_Start();
			Res=AD5933_RES();
			TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,0X000000);
			Adg731_Write_Rg(0x80);
			restestbuf[0]=0xa0;
			restestbuf[1]=66;
			restestbuf[2]=0x32;
			restestbuf[3]=(u8)(Res>>8);
			restestbuf[4]=(u8)(Res);
			restestbuf[67]=0xc0;
			len=restestbuf[1]+2;
			tcp_server_flag|=1<<7;
			tcp_server_sendbuf=restestbuf;
		break;
		default :
		break;
		}
	}
}































