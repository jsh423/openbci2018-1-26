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
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F������
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
 

u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	//TCP�ͻ��˽������ݻ�����
u8 *tcp_server_sendbuf;//="Apollo STM32F4/F7 NETCONN TCP Server send data\r\n";	
u8 tcp_server_flag;								//TCP���������ݷ��ͱ�־λ
u8 Num;

//TCP�ͻ�������
#define OPENBCI_PRIO		6
//�����ջ��С
#define OPENBCI_STK_SIZE	300
//�����ջ
OS_STK OPENBCI_TASK_STK[OPENBCI_STK_SIZE];
//������
void openbci_task(void *arg);

struct netconn *newconn=0;		            //NET CAM TCP�������ӽṹ��ָ�� 
u8 *netcam_line_buf0;					    //�����л���0  
u8 *netcam_line_buf1;					    //�����л���1   

//NET CAM FIFO
vu16 netcamfifordpos=0;					    //FIFO��λ��
vu16 netcamfifowrpos=0;					    //FIFOдλ��
u8 *netcamfifobuf;	    //����NETCAM_FIFO_SIZE������FIFO	

//��ȡFIFO
//buf:���ݻ������׵�ַ
//����ֵ:0,û�����ݿɶ�;
//      1,������1�����ݿ�
u8 netcam_fifo_read(u8 **buf)
{
//	if(netcamfifordpos==netcamfifowrpos)return 0;
//	netcamfifordpos++;		//��λ�ü�1
//	if(netcamfifordpos>=NETCAM_FIFO_NUM)netcamfifordpos=0;//���� 
	*buf=netcamfifobuf;
	return 1;
}
//дһ��FIFO
//buf:���ݻ������׵�ַ
//����ֵ:0,д��ɹ�;
//      1,д��ʧ��
u8 netcam_fifo_write(u8 *buf)
{
	u16 i;
	u16 temp=netcamfifowrpos;	//��¼��ǰдλ��
	
//	if(netcamfifowrpos>=NETCAM_FIFO_NUM)	netcamfifowrpos=0;//����  
//	else
//	{
		for(i=0;i<NETCAM_LINE_SIZE;i++)	netcamfifobuf[netcamfifowrpos*NETCAM_LINE_SIZE+i]=buf[i];//��������
		netcamfifowrpos++;			//дλ�ü�1
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
	u16 temp=netcamfifowrpos;	//��¼��ǰдλ��
	netcamfifowrpos++;			//дλ�ü�1
	if(netcamfifowrpos>=NETCAM_FIFO_NUM)netcamfifowrpos=0;//����  
	if(netcamfifordpos==netcamfifowrpos)
	{
		netcamfifowrpos=temp;	//��ԭԭ����дλ��,�˴�д��ʧ��
		//printf("shit:%d\r\n",temp);
		return 1;	
	}
	for(i=0;i<NETCAM_LINE_SIZE;i++)netcamfifobuf[netcamfifowrpos][i]=buf[i];//��������
	return 0;
}   
#endif

//��Ӧ�������ڴ�����
//����ֵ:0 �ɹ������� ʧ��
u8 netmem_malloc(void)
{
    u16 t=0;
    netcam_line_buf0=mymalloc(SRAMIN,NETCAM_LINE_SIZE);
	netcam_line_buf1=mymalloc(SRAMIN,NETCAM_LINE_SIZE);

    //��FIFO�����ڴ�
	netcamfifobuf=mymalloc(SRAMIN,NETCAM_LINE_SIZE*NETCAM_FIFO_NUM);
//    for(t=0;t<NETCAM_FIFO_NUM;t++) 
//	{
//		netcamfifobuf[t]=mymalloc(SRAMEX,NETCAM_LINE_SIZE);
//	} 
		if(!netcamfifobuf)//||!netcam_line_buf0)//�ڴ�����ʧ��  
    {
        myfree(SRAMIN,netcamfifobuf);
		//netmem_free();//�ͷ��ڴ�
        return 1;
    }
    return 0;
//    if(!netcamfifobuf[NETCAM_FIFO_NUM-1]||!netcam_line_buf1||!netcam_line_buf0)//�ڴ�����ʧ��  
//    {
//        netmem_free();//�ͷ��ڴ�
//        return 1;
//    }
//    return 0;
}

//��Ӧ�������ڴ��ͷ�
void netmem_free(void)
{
    u16 t=0;
    myfree(SRAMIN,netcam_line_buf0);
    myfree(SRAMIN,netcam_line_buf1);
    //�ͷ�FIFO���ڴ�
    //for(t=0;t<NETCAM_FIFO_NUM;t++) 
	//{
        myfree(SRAMEX,netcamfifobuf);
	//}  
}

//ADS1299���ݽ��ջص�����
static void spi2_rx_dma_callback(void)
{    
	if(DMA1_Stream3->CR&(1<<19))netcam_fifo_write(netcam_line_buf0);	//netcam_line_buf0д��FIFO
	else netcam_fifo_write(netcam_line_buf1);							//netcam_line_buf1д��FIFO 
}
//openbci��ʼ��
void hardware_init(void)
{
	netmem_malloc();
	
	ADS1299_Init();
	
	while(ADS1299_Check()) 									//��ʼ��tcp_server(����tcp_server��)
	{
		LCD_ShowString(30,110,200,20,16,"ADS1299 Check failed! "); 
		delay_ms(500);
		LCD_Fill(30,110,230,170,WHITE);
		delay_ms(500);
	}
	LCD_ShowString(30,150,200,20,16,"ads1299 check Success!  "); 	//tcp�����������ɹ�
	
	// spi2_rx_callback=spi2_rx_dma_callback;//�������ݻص�����
     //BCI_DMA_Init((u32)netcam_line_buf0,0,NETCAM_LINE_SIZE,DMA_MDATAALIGN_BYTE,DMA_MINC_ENABLE); 
	//ADS1299_DMA_Config(DMA1_Stream3,DMA_CHANNEL_0);//��ʼ��DMA
	//ADS1299_Start();
	ADS1299_IT();		//�ж�����
}
//openbci����
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

	conn = netconn_new(NETCONN_TCP);  //����һ��TCP����
	netconn_bind(conn,IP_ADDR_ANY,TCP_SERVER_PORT);  //�󶨶˿� 8�Ŷ˿�
	netconn_listen(conn);  		//�������ģʽ
	conn->recv_timeout = 10;  	//��ֹ�����߳� �ȴ�10ms
	
	
	while (1) 
	{
		err = netconn_accept(conn,&newconn);  //������������
		if(err==ERR_OK)newconn->recv_timeout = 10;

		if (err == ERR_OK)    //���������ӵ�����
		{ 
			struct netbuf *recvbuf;

			netconn_getaddr(newconn,&ipaddr,&port,0); //��ȡԶ��IP��ַ�Ͷ˿ں�
			
			remot_addr[3] = (uint8_t)(ipaddr.addr >> 24); 
			remot_addr[2] = (uint8_t)(ipaddr.addr>> 16);
			remot_addr[1] = (uint8_t)(ipaddr.addr >> 8);
			remot_addr[0] = (uint8_t)(ipaddr.addr);
			printf("����%d.%d.%d.%d�����Ϸ�����,�����˿ں�Ϊ:%d\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3],port);			
			hardware_init();
			netcamfifowrpos=0;
			//Sem_Task_LED2 = OSSemCreate(0);
			//tcp_server_sendbuf=&adc_buf2[0][0];
			while(1)
			{
				
//				if(__HAL_DMA_GET_FLAG(&SPI2RxDMA_Handler,DMA_FLAG_TCIF3_7))//�ȴ�DMA2_Steam7�������
//                {
//                    __HAL_DMA_CLEAR_FLAG(&SPI2RxDMA_Handler,DMA_FLAG_TCIF3_7);//���DMA2_Steam7������ɱ�־
//                    HAL_SPI_DMAStop(&SPI2_Handler);      //��������Ժ�رմ���DMA
//					break; 
//                }
				//OSSemPend(Sem_Task_LED2,0,&err);  // �ȴ��ź���
		
		//netcam_fifo_write(adc_buf2);
				//Recev_Data();	
				LED0=!LED0;
				//res=netcam_fifo_read(&tcp_server_sendbuf);
				if((tcp_server_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				//if(res)
				{
					netcam_fifo_read(&tcp_server_sendbuf);
					err = netconn_write(newconn ,tcp_server_sendbuf,NETCAM_LINE_SIZE*Num,NETCONN_COPY); //����tcp_server_sendbuf�е�����
					if(err != ERR_OK)
					{
						printf("����ʧ��\r\n");
					}
					tcp_server_flag &= ~LWIP_SEND_DATA;
				}
//				LED0=!LED0;
				if((recv_err = netconn_recv(newconn,&recvbuf)) == ERR_OK)  	//���յ�����
				{		
					OS_ENTER_CRITICAL(); //���ж�
					memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//��������
						else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > TCP_SERVER_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					OS_EXIT_CRITICAL();  //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣	
					printf("%s\r\n",tcp_server_recvbuf);  //ͨ�����ڷ��ͽ��յ�������
					netbuf_delete(recvbuf);
				}
				
				else if(recv_err == ERR_CLSD)  //�ر�����
				{
					netmem_free();
					netconn_close(newconn);
					netconn_delete(newconn);
					printf("����:%d.%d.%d.%d�Ͽ��������������\r\n",remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
					break;
				}
				//LED0=!LED0;
				//OSTimeDlyHMSM(0,0,0,1);  //��ʱ500ms
			}
			 //myfree(SRAMIN,netcamfifobuf);
		}
	}
}


//����OPENBCI�������߳�
//����ֵ:0 �����������ɹ�
//		���� ����������ʧ��
INT8U openbci_init(void)
{
	INT8U res;
	OS_CPU_SR cpu_sr;
	
	OS_ENTER_CRITICAL();	//���ж�
	res = OSTaskCreate(openbci_task,(void*)0,(OS_STK*)&OPENBCI_TASK_STK[OPENBCI_STK_SIZE-1],OPENBCI_PRIO); //����TCP�������߳�
	OS_EXIT_CRITICAL();		//���ж�
	
	return res;
}


