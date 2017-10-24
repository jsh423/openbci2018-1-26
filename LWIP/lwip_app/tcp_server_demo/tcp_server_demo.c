#include "tcp_server_demo.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "led.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "ads1299.h"
#include "malloc.h"
#include "lcd.h"
//#include "dma.h"
#include "delay.h"
#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F开发板
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
 

u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP客户端接收数据缓冲区
u8 *tcp_server_sendbuf;//="Apollo STM32F4/F7 NETCONN TCP Server send data\r\n";	
u8 tcp_server_flag;								//TCP服务器数据发送标志位
u8 Num;

//TCP客户端任务
#define OPENBCI_PRIO		6
//任务堆栈大小
#define OPENBCI_STK_SIZE	300
//任务堆栈
OS_STK OPENBCI_TASK_STK[OPENBCI_STK_SIZE];
//任务函数
void openbci_task(void *arg);

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
	return 1;
}
//写一个FIFO
//buf:数据缓存区首地址
//返回值:0,写入成功;
//      1,写入失败
u8 netcam_fifo_write(u8 *buf)
{
	u16 i;
	u16 temp=netcamfifowrpos;	//记录当前写位置
	
//	if(netcamfifowrpos>=NETCAM_FIFO_NUM)	netcamfifowrpos=0;//归零  
//	else
//	{
		for(i=0;i<NETCAM_LINE_SIZE;i++)	netcamfifobuf[netcamfifowrpos*NETCAM_LINE_SIZE+i]=buf[i];//拷贝数据
		netcamfifowrpos++;			//写位置加1
		if(netcamfifowrpos<20)return 0;
		else
		{
			netcamfifowrpos=0;
			return temp+1;
		}
	//}
}
#if 0
u8 netcam_fifo_write(u8 *buf)
{
	u16 i;
	u16 temp=netcamfifowrpos;	//记录当前写位置
	netcamfifowrpos++;			//写位置加1
	if(netcamfifowrpos>=NETCAM_FIFO_NUM)netcamfifowrpos=0;//归零  
	if(netcamfifordpos==netcamfifowrpos)
	{
		netcamfifowrpos=temp;	//还原原来的写位置,此次写入失败
		//printf("shit:%d\r\n",temp);
		return 1;	
	}
	for(i=0;i<NETCAM_LINE_SIZE;i++)netcamfifobuf[netcamfifowrpos][i]=buf[i];//拷贝数据
	return 0;
}   
#endif

//相应变量的内存申请
//返回值:0 成功；其他 失败
u8 netmem_malloc(void)
{
    u16 t=0;
    netcam_line_buf0=mymalloc(SRAMIN,NETCAM_LINE_SIZE);
	netcam_line_buf1=mymalloc(SRAMIN,NETCAM_LINE_SIZE);

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
    u16 t=0;
    myfree(SRAMIN,netcam_line_buf0);
    myfree(SRAMIN,netcam_line_buf1);
    //释放FIFO的内存
    //for(t=0;t<NETCAM_FIFO_NUM;t++) 
	//{
        myfree(SRAMEX,netcamfifobuf);
	//}  
}

//ADS1299数据接收回调函数
static void spi2_rx_dma_callback(void)
{    
	if(DMA1_Stream3->CR&(1<<19))netcam_fifo_write(netcam_line_buf0);	//netcam_line_buf0写入FIFO
	else netcam_fifo_write(netcam_line_buf1);							//netcam_line_buf1写入FIFO 
}
//openbci初始化
void hardware_init(void)
{
	netmem_malloc();
	
	ADS1299_Init();
	
	while(ADS1299_Check()) 									//初始化tcp_server(创建tcp_server程)
	{
		LCD_ShowString(30,110,200,20,16,"ADS1299 Check failed! "); 
		delay_ms(500);
		LCD_Fill(30,110,230,170,WHITE);
		delay_ms(500);
	}
	LCD_ShowString(30,150,200,20,16,"ads1299 check Success!  "); 	//tcp服务器创建成功
	
	// spi2_rx_callback=spi2_rx_dma_callback;//接收数据回调函数
     //BCI_DMA_Init((u32)netcam_line_buf0,0,NETCAM_LINE_SIZE,DMA_MDATAALIGN_BYTE,DMA_MINC_ENABLE); 
	//ADS1299_DMA_Config(DMA1_Stream3,DMA_CHANNEL_0);//初始化DMA
	//ADS1299_Start();
	ADS1299_IT();		//中断配置
}
//openbci任务
static void openbci_task(void *arg)
{
	OS_CPU_SR cpu_sr;
	u32 data_len = 0;
	struct pbuf *q;
	err_t err,recv_err;
	u8 remot_addr[4];
	struct netconn *conn, *newconn;
	static ip_addr_t ipaddr;
	static u16_t 			port;
	u8 res;
	
	LWIP_UNUSED_ARG(arg);

	conn = netconn_new(NETCONN_TCP);  //创建一个TCP链接
	netconn_bind(conn,IP_ADDR_ANY,TCP_SERVER_PORT);  //绑定端口 8号端口
	netconn_listen(conn);  		//进入监听模式
	conn->recv_timeout = 10;  	//禁止阻塞线程 等待10ms
	
	
	while (1) 
	{
		err = netconn_accept(conn,&newconn);  //接收连接请求
		if(err==ERR_OK)newconn->recv_timeout = 10;

		if (err == ERR_OK)    //处理新连接的数据
		{ 
			struct netbuf *recvbuf;

			netconn_getaddr(newconn,&ipaddr,&port,0); //获取远端IP地址和端口号
			
			remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
			remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
			remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
			remot_addr[0] = (uint8_t)(ipaddr.addr);
			printf("主机%d.%d.%d.%d连接上服务器,主机端口号为:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);			
			hardware_init();
			netcamfifowrpos=0;
			//Sem_Task_LED2 = OSSemCreate(0);
			//tcp_server_sendbuf=&adc_buf2[0][0];
			while(1)
			{
				
//				if(__HAL_DMA_GET_FLAG(&SPI2RxDMA_Handler,DMA_FLAG_TCIF3_7))//等待DMA2_Steam7传输完成
//                {
//                    __HAL_DMA_CLEAR_FLAG(&SPI2RxDMA_Handler,DMA_FLAG_TCIF3_7);//清除DMA2_Steam7传输完成标志
//                    HAL_SPI_DMAStop(&SPI2_Handler);      //传输完成以后关闭串口DMA
//					break; 
//                }
				//OSSemPend(Sem_Task_LED2,0,&err);  // 等待信号量
		
		//netcam_fifo_write(adc_buf2);
				//Recev_Data();	
				LED0=!LED0;
				//res=netcam_fifo_read(&tcp_server_sendbuf);
				if((tcp_server_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //有数据要发送
				//if(res)
				{
					netcam_fifo_read(&tcp_server_sendbuf);
					err = netconn_write(newconn ,tcp_server_sendbuf,NETCAM_LINE_SIZE*Num,NETCONN_COPY); //发送tcp_server_sendbuf中的数据
					if(err != ERR_OK)
					{
						printf("发送失败\r\n");
					}
					tcp_server_flag &= ~LWIP_SEND_DATA;
				}
//				LED0=!LED0;
				if((recv_err = netconn_recv(newconn,&recvbuf)) == ERR_OK)  	//接收到数据
				{		
					OS_ENTER_CRITICAL(); //关中断
					memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //数据接收缓冲区清零
					for(q=recvbuf->p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
					{
						//判断要拷贝到TCP_SERVER_RX_BUFSIZE中的数据是否大于TCP_SERVER_RX_BUFSIZE的剩余空间，如果大于
						//的话就只拷贝TCP_SERVER_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
						if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//拷贝数据
						else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_SERVER_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
					}
					OS_EXIT_CRITICAL();  //开中断
					data_len=0;  //复制完成后data_len要清零。	
					printf("%s\r\n",tcp_server_recvbuf);  //通过串口发送接收到的数据
					netbuf_delete(recvbuf);
				}
				
				else if(recv_err == ERR_CLSD)  //关闭连接
				{
					netmem_free();
					netconn_close(newconn);
					netconn_delete(newconn);
					printf("主机:%d.%d.%d.%d断开与服务器的连接\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					break;
				}
				//LED0=!LED0;
				//OSTimeDlyHMSM(0,0,0,1);  //延时500ms
			}
			 //myfree(SRAMIN,netcamfifobuf);
		}
	}
}


//创建OPENBCI服务器线程
//返回值:0 服务器创建成功
//		其他 服务器创建失败
INT8U openbci_init(void)
{
	INT8U res;
	OS_CPU_SR cpu_sr;
	
	OS_ENTER_CRITICAL();	//关中断
	res = OSTaskCreate(openbci_task,(void*)0,(OS_STK*)&OPENBCI_TASK_STK[OPENBCI_STK_SIZE-1],OPENBCI_PRIO); //创建TCP服务器线程
	OS_EXIT_CRITICAL();		//开中断
	
	return res;
}


