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
#include "iwdg.h"
#include "tca6424a.h"
#include "timer.h"
#include "udp_demo.h" 
//////////////////////////////////////////////////////////////////////////////////	 

//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
//TCP Server�������ݻ�����
u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	
//TCP������������������	
u8 *tcp_server_sendbuf;//="Apollo STM32F4/F7 TCP Server send data\r\n";
//u32 ledbuf[32]={0X38be00,0X78b400,0x387e00,0x787400,0x3c3c00,0x3cb400,0x3a3c,0x7a34,0x39be00,0x79b400,
//				0x687c00,0x785c00,0x6c7c00,0x7c1c00,0x6b3c00,0x7b1b00,0x5c3c00,0x7c2c00,0x707c00,0x743c00,
//				0x723c00,0x789c00,0x687c00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x100};

//u32 ledbuf1[5]={0x008000,0x004000,0x040000,0x020000,0x010000};
				
u32 ledbuf2[8]={0x5FFFFF,0X6FFFFF,0X3FFFFF,0X77FFFF,0X7FF7FF,0X7FDFFF,0X7FEFFF,0X7FFBFF};	
u16 ledbuf3[9]={0x100,0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
u8 tcp_server_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP�������ݻ����� 
//UDP������������
//u8 *tcp_server_sendbuf;//="Apollo STM32F4/F7 UDP demo send data\r\n";

//UDP ����ȫ��״̬��Ǳ���
//bit7:û���õ�
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û��������;1,��������.
//bit4~0:����
extern u8 gain;
u8 tcp_server_flag;
u8 ResTest_Flag;
static u8 Ad_Flag;
u32 Res1[6]={0x78be00,0x787e00,0x7c3e00,0x7a3e00,0x793e00,0x00};
//TCP Server ����ȫ��״̬��Ǳ���
//bit7:0,û������Ҫ����;1,������Ҫ����
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û�пͻ���������;1,�пͻ�����������.
//bit4~0:����
u8 tcp_server_flag;	 
 u8 Num;
u16 len;
u8 sendsetbuf[5]={0xa0,3,0x11,00,0xc0};
u8 setledbuf[5]={0xa0,3,0x41,0,0xc0};
u8 selftesthbuf[5]={0xa0,3,0x14,0,0xc0};

//u8 restestbuf[6]={0xa0,4,0x31,0,0,0xc0};
u8 restestbuf[68];//={0xa0,66,0x33,0,0,0,0,0

 struct netconn *newconn=0;		            //NET CAM TCP�������ӽṹ��ָ�� 
//u8 *netcam_line_buf0;					    //�����л���0  
//u8 *netcam_line_buf1;					    //�����л���1   
//extern u32 lwip_localtime;	         //lwip����ʱ�������,��λ:ms
u16 led_shtime;
static u8 PwmledFlag;
static u8 LedhighFlag;
u16 Pwmledduty;
static u16 Pwmledtimer;
u16 Pwmledtimer1;
u16 Pwmledfre;
static u16 ledcjtimer;
u16 led_pwmtime;			//����̼���ʱ�� ��λs
//NET CAM FIFO
vu16 netcamfifordpos=0;					    //FIFO��λ��
vu16 netcamfifowrpos=0;					    //FIFOдλ��
u8* netcamfifobuf;	    //����NETCAM_FIFO_SIZE������FIFO	
u32 State_Im;
//u8 t=0;

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
//	netcam_line_buf0[0]=0xa0;
//	netcam_line_buf0[NETCAM_LINE_SIZE-1]=0xc0;
//	if(Index++<0xff) netcam_line_buf0[1]=Index;
//	else Index=0;
//	*buf=netcam_line_buf0;
	return 1;
}
#if 1
//дһ��FIFO
//buf:���ݻ������׵�ַ
//����ֵ:0,д��ɹ�;
//      1,д��ʧ��
u8 netcam_fifo_write(u8 *buf)
{
	//u16 i;
	u16 temp=netcamfifowrpos;	//��¼��ǰдλ��
	u16 res;
	
	//if(netcamfifowrpos>=NETCAM_FIFO_NUM)	netcamfifowrpos=0;//����  
//	else
//	{
	//netcamfifobuf[netcamfifowrpos*NETCAM_LINE_SIZE+i]=buf[i];//��������
	//for(i=0;i<NETCAM_LINE_SIZE;i++)	netcamfifobuf[i]=buf[i];//��������
		//for(i=0;i<NETCAM_LINE_SIZE;i++)	netcamfifobuf[netcamfifowrpos*NETCAM_LINE_SIZE+i]=buf[i];//��������
		if(openvibeflag) res=NETCAM_LINE_SIZE-2;
		else res=NETCAM_LINE_SIZE;
	
		//if(netcamfifowrpos<10)
		//{
			memcpy(&netcamfifobuf[netcamfifowrpos*res],buf,res);
//		}
//		else if(netcamfifowrpos<20)
//		{
//			memcpy(&netcam_line_buf0[(netcamfifowrpos-10)*res],buf,res);
//		}
////		else
////		{
//			memcpy(&netcamfifobuf[netcamfifowrpos*NETCAM_LINE_SIZE],buf,NETCAM_LINE_SIZE);
//		}
		netcamfifowrpos++;			//дλ�ü�1
		if(netcamfifowrpos<NETCAM_FIFO_NUM)return 0;
		else
		{
			//netcam_fifo_read(&tcp_server_sendbuf);
			tcp_server_sendbuf=netcamfifobuf;
			len=netcamfifowrpos*res;
			netcamfifowrpos=0;
			//tcp_server_flag|=(1<<7);//������Ҫ����	//netcam_line_buf1д��FIFO 
			return temp+1;
		}
	//}
}
#endif

#if 0
u8 netcam_fifo_write(u8 *buf)
{
	u16 i;
	u16 temp=netcamfifowrpos;	//��¼��ǰдλ��
//	netcamfifowrpos++;			//дλ�ü�1
//	if(netcamfifowrpos>=NETCAM_FIFO_NUM)netcamfifowrpos=0;//����  
//	if(netcamfifordpos==netcamfifowrpos)
//	{
//		netcamfifowrpos=temp;	//��ԭԭ����дλ��,�˴�д��ʧ��
//		//printf("shit:%d\r\n",temp);
//		return 1;	
//	}
	for(i=0;i<NETCAM_LINE_SIZE-3;i++)netcam_line_buf0[i+2]=buf[i];//��������
	return 0;
}   
#endif

//��Ӧ�������ڴ�����
//����ֵ:0 �ɹ������� ʧ��
u8 netmem_malloc(void)
{
//    u16 t=0;
   // netcam_line_buf0=mymalloc(SRAMIN,NETCAM_LINE_SIZE*10);
	//netcam_line_buf1=mymalloc(SRAMIN,NETCAM_LINE_SIZE);

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
   // u16 t=0;
   // myfree(SRAMIN,netcam_line_buf0);
    //myfree(SRAMIN,netcam_line_buf1);
    //�ͷ�FIFO���ڴ�
    //for(t=0;t<NETCAM_FIFO_NUM;t++) 
	//{
        myfree(SRAMIN,netcamfifobuf);
	//}  
}
 //ADS1299���ݽ��ջص�����
//static void spi2_rx_dma_callback(void)
//{    
//	//if(DMA1_Stream3->CR&(1<<19))netcam_fifo_write(netcam_line_buf0);	//netcam_line_buf0д��FIFO
//	//else netcam_fifo_write(netcam_line_buf1);	
//	//netcam_fifo_write(adc_buf2);
//	//tcp_server_flag|=(1<<7);//������Ҫ����
//}
//openbci��ʼ��
void hardware_init(void)
{
	
	
	
	
	while(1) 									//��ʼ��tcp_server(����tcp_server��)
	{
		//LCD_ShowString(30,110,200,20,16,"ADS1299 Check failed! "); 
		//delay_ms(500);
		//LCD_Fill(30,110,230,170,WHITE);
		if(ADS1299_Check()) delay_ms(100);
		else break;
		
	}
	//LCD_ShowString(30,150,200,20,16,"ads1299 check Success!  "); 	//tcp�����������ɹ�
	
	// spi2_rx_callback=spi2_rx_dma_callback;//�������ݻص�����
    // BCI_DMA_Init(netcam_line_buf0[0],netcam_line_buf1[0],NETCAM_LINE_SIZE); 
	//ADS1299_DMA_Config(DMA1_Stream3,DMA_CHANNEL_0);//��ʼ��DMA
	//ADS1299_DMA_Start();
	
	ADS1299_IT();		//�ж�����
}
// #if 0
//////TCP Server ����
//void tcp_server_test(void)
//{
//	err_t err;  
//	struct tcp_pcb *tcppcbnew;  	//����һ��TCP���������ƿ�
//	struct tcp_pcb *tcppcbconn;  	//����һ��TCP���������ƿ�
//	
//	u8 *tbuf;
//// 	u8 key;
//	u8 res=0,res1=0;		
//	u16 t=0; 
//	//u8 i;
//	
//	
//	//netmem_free();
//	tbuf=mymalloc(SRAMIN,200);	//�����ڴ�
//	netmem_malloc();
//	if(tbuf==NULL)return ;		//�ڴ�����ʧ����,ֱ���˳�
//	//sprintf((char*)tbuf,"Server IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//������IP
//	//sprintf((char*)tbuf,"Server Port:%d",TCP_SERVER_PORT);//�������˿ں� 
//	tcppcbnew=tcp_new();	//����һ���µ�pcb
//	if(tcppcbnew)			//�����ɹ�
//	{ 
//		err=tcp_bind(tcppcbnew,IP_ADDR_ANY,TCP_SERVER_PORT);	//������IP��ָ���Ķ˿ںŰ���һ��,IP_ADDR_ANYΪ�󶨱������е�IP��ַ
//		if(err==ERR_OK)	//�����
//		{
//			tcppcbconn=tcp_listen(tcppcbnew); 			//����tcppcb�������״̬
//			tcp_accept(tcppcbconn,tcp_server_accept); 	//��ʼ��LWIP��tcp_accept�Ļص�����
//		}else res=1;  
//	}else res=1;
//	delay_ms(100);
//	//hardware_init();
//	if((tcp_server_flag&1<<5)&&(res==0))
//	{
//		res1=1;
//		//hardware_init();
//	}
//	while(res1==1)//����Ѿ�����
//	{
////		while(res1==1)
////		{
//		if(tcp_server_flag&1<<7)//������Ҫ����)//KEY0������,��������
//		{
//			//netcam_fifo_read(&tcp_server_sendbuf);
//			tcp_server_sendbuf=netcamfifobuf;
//            tcp_server_usersent(tcppcbnew);//��������
//		}
//		if(tcp_server_flag&1<<6)//�Ƿ��յ�����?
//		{
//			Data_Process();
//			//LCD_Fill(30,210,lcddev.width-1,lcddev.height-1,WHITE);//����һ������
//			//LCD_ShowString(30,210,lcddev.width-30,lcddev.height-210,16,tcp_server_recvbuf);//��ʾ���յ�������	
//			//�˴���ͨ�����ݽ��н���
//			//��ͷ��β�Ƿ���ȷ
//			//�������ж�
//			//21���豸�����ϴ�
//			//11������������
//			//12:�ղ�����
//			//13���˲�����
//			//01��AD���ݲɼ�
//			tcp_server_flag&=~(1<<6);//��������Ѿ���������.
//		}
//		
//		//}
//		if((tcp_server_flag&1<<5)==0)
//		{
//			res1=0;
//			break;
//			//LCD_Fill(30,170,lcddev.width-1,lcddev.height-1,WHITE);//����
//		}
//		
//		lwip_periodic_handle();
//		//delay_ms(2);
//		//State_Im=(stat[3]<<24)|(stat[2]<<16)|(stat[1]<<8)|(stat[0]);
//		
//		t++;
////		if(t>1000)
////		{
////			t=0;
////		if(i<8) i++;
////		else i=0;
////		State_Im=(stat[3]<<24)|(stat[2]<<16)|(stat[1]<<8)|(stat[0]);
//////		if(t<4)
//////		{
//////			for(t=0;t<4;t++)
//////		{
//////			for(i=0;i<8;i++)
//////			{
////		if((State_Im&(0x00000001<<i))) TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,ledbuf[i]);
////				//else TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,00);
////			//}
////		//}
////		}	
//			//LED1=!LED1;
//		//} 
//	}   
//	tcp_server_connection_close(tcppcbnew,0);//�ر�TCP Server����
//	tcp_server_connection_close(tcppcbconn,0);//�ر�TCP Server���� 
//	tcp_server_remove_timewait(); 
//	memset(tcppcbnew,0,sizeof(struct tcp_pcb));
//	memset(tcppcbconn,0,sizeof(struct tcp_pcb)); 
//	myfree(SRAMIN,tbuf);
//	netmem_free();
//	
//} 
//#endif
//lwIP tcp_accept()�Ļص�����
//err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err)
//{
//	err_t ret_err;
//	struct tcp_server_struct *es; 
// 	LWIP_UNUSED_ARG(arg);
//	LWIP_UNUSED_ARG(err);
//	tcp_setprio(newpcb,TCP_PRIO_MIN);//�����´�����pcb���ȼ�
//	es=(struct tcp_server_struct*)mem_malloc(sizeof(struct tcp_server_struct)); //�����ڴ�
// 	if(es!=NULL) //�ڴ����ɹ�
//	{
//		es->state=ES_TCPSERVER_ACCEPTED;  	//��������
//		es->pcb=newpcb;
//		es->p=NULL;
//		
//		tcp_arg(newpcb,es);
//		tcp_recv(newpcb,tcp_server_recv);	//��ʼ��tcp_recv()�Ļص�����
//		tcp_err(newpcb,tcp_server_error); 	//��ʼ��tcp_err()�ص�����
//		tcp_poll(newpcb,tcp_server_poll,1);	//��ʼ��tcp_poll�ص�����
//		tcp_sent(newpcb,tcp_server_sent);  	//��ʼ�����ͻص�����
//		  
//		tcp_server_flag|=1<<5;				//����пͻ���������
//		lwipdev.remoteip[0]=newpcb->remote_ip.addr&0xff; 		//IADDR4
//		lwipdev.remoteip[1]=(newpcb->remote_ip.addr>>8)&0xff;  	//IADDR3
//		lwipdev.remoteip[2]=(newpcb->remote_ip.addr>>16)&0xff; 	//IADDR2
//		lwipdev.remoteip[3]=(newpcb->remote_ip.addr>>24)&0xff; 	//IADDR1 
//		ret_err=ERR_OK;
//	}else ret_err=ERR_MEM;
//	return ret_err;
//}
////lwIP tcp_recv()�����Ļص�����
//err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
//{
//	err_t ret_err;
//	u32 data_len = 0;
//	struct pbuf *q;
//  	struct tcp_server_struct *es;
//	LWIP_ASSERT("arg != NULL",arg != NULL);
//	es=(struct tcp_server_struct *)arg;
//	if(p==NULL) //�ӿͻ��˽��յ�������
//	{
//		es->state=ES_TCPSERVER_CLOSING;//��Ҫ�ر�TCP ������
//		es->p=p; 
//		ret_err=ERR_OK;
//	}else if(err!=ERR_OK)	//�ӿͻ��˽��յ�һ���ǿ�����,��������ĳ��ԭ��err!=ERR_OK
//	{
//		if(p)pbuf_free(p);	//�ͷŽ���pbuf
//		ret_err=err;
//	}else if(es->state==ES_TCPSERVER_ACCEPTED) 	//��������״̬
//	{
//		if(p!=NULL)  //����������״̬���ҽ��յ������ݲ�Ϊ��ʱ�����ӡ����
//		{
//			memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //���ݽ��ջ���������
//			for(q=p;q!=NULL;q=q->next)  //����������pbuf����
//			{
//				//�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
//				//�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
//				if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//��������
//				else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
//				data_len += q->len;  	
//				if(data_len > TCP_SERVER_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
//			}
//			tcp_server_flag|=1<<6;	//��ǽ��յ�������
//			lwipdev.remoteip[0]=tpcb->remote_ip.addr&0xff; 		//IADDR4
//			lwipdev.remoteip[1]=(tpcb->remote_ip.addr>>8)&0xff; //IADDR3
//			lwipdev.remoteip[2]=(tpcb->remote_ip.addr>>16)&0xff;//IADDR2
//			lwipdev.remoteip[3]=(tpcb->remote_ip.addr>>24)&0xff;//IADDR1 
// 			tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
//			pbuf_free(p);  	//�ͷ��ڴ�
//			ret_err=ERR_OK;
//		}
//	}else//�������ر���
//	{
//		tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
//		es->p=NULL;
//		pbuf_free(p); //�ͷ��ڴ�
//		ret_err=ERR_OK;
//		tcp_server_flag&=~(1<<5);//������ӶϿ���
//	}
//	return ret_err;
//}
////lwIP tcp_err�����Ļص�����
//void tcp_server_error(void *arg,err_t err)
//{  
//	LWIP_UNUSED_ARG(err);  
//	tcp_server_flag&=~(1<<5);//������ӶϿ���
//	//printf("tcp error:%x\r\n",(u32)arg);
//	if(arg!=NULL)mem_free(arg);//�ͷ��ڴ�
//} 

////LWIP���ݷ��ͣ��û�Ӧ�ó�����ô˺�������������
////tpcb:TCP���ƿ�
////����ֵ:0���ɹ���������ʧ��
//err_t tcp_server_usersent(struct tcp_pcb *tpcb)
//{
//    err_t ret_err;
//	struct tcp_server_struct *es; 
//	es=tpcb->callback_arg;
//	if(es!=NULL)  //���Ӵ��ڿ��п��Է�������
//	{
////es->p=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)tcp_server_sendbuf),PBUF_POOL);	//�����ڴ� 
//       // pbuf_take(es->p,(char*)tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf));	//��tcp_server_sentbuf[]�е����ݿ�����es->p_tx��
//		es->p=pbuf_alloc(PBUF_TRANSPORT, len,PBUF_RAM);	//�����ڴ� 
//        pbuf_take(es->p,(char*)tcp_server_sendbuf,len);	//��tcp_server_sentbuf[]�е����ݿ�����es->p_tx��
//        tcp_server_senddata(tpcb,es);   //��tcp_server_sentbuf[]���渴�Ƹ�pbuf�����ݷ��ͳ�ȥ
//        tcp_server_flag&=~(1<<7);	    //������ݷ��ͱ�־
//        if(es->p!=NULL)pbuf_free(es->p);//�ͷ��ڴ�
//		ret_err=ERR_OK;
//	}else
//	{ 
//		tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
//		ret_err=ERR_ABRT;
//	}
//	return ret_err;
//}

////lwIP tcp_poll�Ļص�����
//err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
//{
//	err_t ret_err;
//	struct tcp_server_struct *es; 
//	es=(struct tcp_server_struct *)arg; 
//    if(es->state==ES_TCPSERVER_CLOSING)//��Ҫ�ر�����?ִ�йرղ���
//    {
//        tcp_server_connection_close(tpcb,es);//�ر�����
//		//tcp_server_flag&=~(1<<5);//������ӶϿ���
//    }
//    ret_err=ERR_OK;
//	return ret_err;
//} 
////lwIP tcp_sent�Ļص�����(����Զ���������յ�ACK�źź�������)
//err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
//{
//	struct tcp_server_struct *es;
//	LWIP_UNUSED_ARG(len); 
//	es = (struct tcp_server_struct *) arg;
//	if(es->p)tcp_server_senddata(tpcb,es);//��������
//	return ERR_OK;
//} 
////�˺���������������
//void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
//{
//	struct pbuf *ptr;
//	u16 plen;
//	err_t wr_err=ERR_OK;
//	while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb)))
//	{
//		ptr=es->p;
//		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
//		if(wr_err==ERR_OK)
//		{ 
//			plen=ptr->len;
//			es->p=ptr->next;			//ָ����һ��pbuf
//			if(es->p)pbuf_ref(es->p);	//pbuf��ref��һ
//			pbuf_free(ptr);
//			tcp_recved(tpcb,plen); 		//����tcp���ڴ�С
//		}else if(wr_err==ERR_MEM)es->p=ptr;
//        tcp_output(tpcb);		        //�����ͻ�������е������������ͳ�ȥ
//    }
//    
//} 
////�ر�tcp����
//void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
//{
//	tcp_close(tpcb);
//	tcp_arg(tpcb,NULL);
//	tcp_sent(tpcb,NULL);
//	tcp_recv(tpcb,NULL);
//	tcp_err(tpcb,NULL);
//	tcp_poll(tpcb,NULL,0);
//	if(es)mem_free(es); 
//	tcp_server_flag&=~(1<<5);//������ӶϿ���
//}
//extern void tcp_pcb_purge(struct tcp_pcb *pcb);	//�� tcp.c���� 
//extern struct tcp_pcb *tcp_active_pcbs;			//�� tcp.c���� 
//extern struct tcp_pcb *tcp_tw_pcbs;				//�� tcp.c����  
////ǿ��ɾ��TCP Server�����Ͽ�ʱ��time wait
//void tcp_server_remove_timewait(void)
//{
//	struct tcp_pcb *pcb,*pcb2; 
//	u8 t=0;
//	while(tcp_active_pcbs!=NULL&&t<200)
//	{
//		lwip_periodic_handle();	//������ѯ
//		t++;
//		delay_ms(10);			//�ȴ�tcp_active_pcbsΪ��  
//	}
//	pcb=tcp_tw_pcbs;
//	while(pcb!=NULL)//����еȴ�״̬��pcbs
//	{
//		tcp_pcb_purge(pcb); 
//		tcp_tw_pcbs=pcb->next;
//		pcb2=pcb;
//		pcb=pcb->next;
//		memp_free(MEMP_TCP_PCB,pcb2);	
//	}
//}
u8 Imbuf[32];
u8 Imline_Flag;
void Data_Process(void)
{
	u8 res,i;
	//u16 Res;
	//u32 res4;
	
	if(tcp_server_recvbuf[0]==0xa0)
	{
		switch(tcp_server_recvbuf[1])
		{
		case 0x1:  //���ݲɼ�����
			
			res=tcp_server_recvbuf[2];
			if(res==1) 
			{
				ADS1299_Command(_START);
				Imline_Flag=1;
				Ad_Flag=1;
			}
				//ADS1299_START=1;//ADS1299оƬ����
			else if(res==0) 
			{
				ADS1299_Command(_STOP);
				Imline_Flag=0;
				Ad_Flag=0;
			}
			//ADS1299_START=0;
		break;
		case 0x11: //����������
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
		case 0x14://�Լ�����
			
			ADS1299_SDATAC();//�˳���������ģʽ���Ա���мĴ���������
		if(tcp_server_recvbuf[2]==1)
		{
			
			ADS1299_WREG(CH1SET,(0x05|(gain<<4)));//��һͨ�����ö�·������ϵͳ����
			ADS1299_WREG(CH2SET,(0x05|(gain<<4)));
			ADS1299_WREG(CH3SET,(0x05|(gain<<4)));
			ADS1299_WREG(CH4SET,(0X05|(gain<<4)));//��һͨ��������ͨ����
			ADS1299_WREG(CH5SET,(0X05|(gain<<4)));
			ADS1299_WREG(CH6SET,(0X05|(gain<<4)));
			ADS1299_WREG(CH7SET,(0X05|(gain<<4)));
			ADS1299_WREG(CH8SET,(0X05|(gain<<4)));
		}
		else if(tcp_server_recvbuf[2]==0)
		{
			ADS1299_WREG(CH1SET,(0x0|(gain<<4)));//��һͨ�����ö�·������ϵͳ����
			ADS1299_WREG(CH2SET,(0x0|(gain<<4)));
			ADS1299_WREG(CH3SET,(0x0|(gain<<4)));
			ADS1299_WREG(CH4SET,(0X0|(gain<<4)));//��һͨ��������ͨ����
			ADS1299_WREG(CH5SET,(0X0|(gain<<4)));
			ADS1299_WREG(CH6SET,(0X0|(gain<<4)));
			ADS1299_WREG(CH7SET,(0X0|(gain<<4)));
			ADS1299_WREG(CH8SET,(0X0|(gain<<4)));
		}
		len=selftesthbuf[1]+2;
		tcp_server_sendbuf=selftesthbuf;
		tcp_server_flag|=1<<7;
		
		ADS1299_Command(_RDATA);//�����ȡ����ģʽ
			break;
		case 0x41://�����������̼�
			Pwmledduty=10*tcp_server_recvbuf[3];
			if(tcp_server_recvbuf[2]==1)
			{
			if((Pwmledduty<=100)&&(tcp_server_recvbuf[4]<=250))
			{
				TIM_SetTIM2(Pwmledduty);
				LedhighFlag=1;
				Pwmledtimer1=led_pwmtime;
				ledcjtimer=led_shtime;
				PwmledFlag=1;//��������̼���־λ
				//Pwmledduty=res;
				Pwmledtimer=(u16)tcp_server_recvbuf[4];
				Pwmledfre=(u16)(50/tcp_server_recvbuf[5]);
				setledbuf[3]=0;
			}
			else setledbuf[3]=1;
		}
			else 
			{
				PwmledFlag=0;//��������̼���־λ
				TIM_SetTIM2(0);
				setledbuf[3]=0;
			}
			tcp_server_sendbuf=setledbuf;
			len=sendsetbuf[1]+2;
			tcp_server_flag|=1<<7;
			break;	
//		case 0x12://�ݲ�����
//			tcp_server_sendbuf=setfirbuf;
//			len=setfirbuf[1]+2;
//			tcp_server_flag|=1<<7;
//		break;
		
//		case 0x13: //�˲�����
//			tcp_server_sendbuf=setnorthbuf;
//			len=setnorthbuf[1]+2;
//			tcp_server_flag|=1<<7;
//		break;
		
		case 0x21://�豸������ѯ
			tcp_server_flag|=1<<7;
			tcp_server_sendbuf=parameter;
			len=parameter[1]+2;
		break;
		case 0x31://����ͨ���迹����
			//ADS1299_Command(_STOP);
			//ADS1299_START=0;
			ImpTest_Start();//�����迹����ģʽ
			ADS1299_Command(_START);
			ResTest_Flag=1;
			restestbuf[0]=0xa0;
			restestbuf[1]=3;
			restestbuf[2]=0x31;
			restestbuf[3]=0;
			restestbuf[4]=0xc0;
			//restestbuf[5]=0xc0;
			//res=tcp_server_recvbuf[2];
			len=restestbuf[1]+2;
			tcp_server_flag|=1<<7;
			tcp_server_sendbuf=restestbuf;		
		break;
		
		case 0x32:  //�����迹�ȶ�����
			
			
				//if(tcp_server_recvbuf[2]||tcp_server_recvbuf[3]||tcp_server_recvbuf[23]||tcp_server_recvbuf[24])
				//{
					//Res1[0]|=ledbuf1[0];//PG1=1;
					if(tcp_server_recvbuf[2]) Res1[0]&=0xf7ffff;//5,4,4
						else Res1[0]|=~0xf7ffff;
					if(tcp_server_recvbuf[3]) Res1[0]&=0x7fdfff;//3,2,6|=~
						else Res1[0]|=~0x7fdfff;
					if(tcp_server_recvbuf[23]) Res1[0]&=0x7fefff;//2,~7
					else Res1[0]|=~0x7fefff;
					if(tcp_server_recvbuf[24]) Res1[0]&=0x3fffff;//6��3
					else Res1[0]|=~0x3fffff;
//				}
//				else Res1[0]&=~ledbuf1[0];//pg1=0;
//				
//				if(tcp_server_recvbuf[13]||tcp_server_recvbuf[5]||tcp_server_recvbuf[20]||tcp_server_recvbuf[4]||tcp_server_recvbuf[12])//12,4,19,3,11
//				{
					//Res1[1]|=ledbuf1[1];//PG2=1;
					if(tcp_server_recvbuf[4]) Res1[1]&=ledbuf2[3];//4
						else Res1[1]|=(~ledbuf2[3]);
					if(tcp_server_recvbuf[5]) Res1[1]&=ledbuf2[5];//6
						else Res1[1]|=(~ledbuf2[5]);
					if(tcp_server_recvbuf[12]) Res1[1]&=ledbuf2[2];//3
					else Res1[1]|=~ledbuf2[2];
					if(tcp_server_recvbuf[13]) Res1[1]&=ledbuf2[6];//7
					else Res1[1]|=~ledbuf2[6];
					if(tcp_server_recvbuf[20]) Res1[1]&=ledbuf2[4];//5
					else Res1[1]|=~ledbuf2[4];
				
				
					if(tcp_server_recvbuf[18]) Res1[2]&=ledbuf2[1];//2
						else Res1[2]|=(~ledbuf2[1]);
						
					if(tcp_server_recvbuf[14]) Res1[2]&=ledbuf2[2];//3
						else Res1[1]|=(~ledbuf2[2]);
					if(tcp_server_recvbuf[6]) Res1[2]&=ledbuf2[3];//4
						else Res1[2]|=(~ledbuf2[3]);
					if(tcp_server_recvbuf[21]) Res1[2]&=ledbuf2[4];//5
					else Res1[2]|=~ledbuf2[4];
					if(tcp_server_recvbuf[7]) Res1[2]&=ledbuf2[5];//6
					else Res1[2]|=~ledbuf2[5];
					if(tcp_server_recvbuf[15]) Res1[2]&=ledbuf2[6];//7
					else Res1[2]|=~ledbuf2[6];
					if(tcp_server_recvbuf[19]) Res1[2]&=ledbuf2[7];//8
					else Res1[2]|=~ledbuf2[7];
					
					if(tcp_server_recvbuf[16]) Res1[3]&=ledbuf2[2];//3
						else Res1[3]|=(~ledbuf2[2]);
					if(tcp_server_recvbuf[8]) Res1[3]&=ledbuf2[3];//4
						else Res1[3]|=(~ledbuf2[3]);
					if(tcp_server_recvbuf[22]) Res1[3]&=ledbuf2[4];//5
					else Res1[3]|=~ledbuf2[4];
					if(tcp_server_recvbuf[9]) Res1[3]&=ledbuf2[5];//6
					else Res1[3]|=~ledbuf2[5];
					if(tcp_server_recvbuf[17]) Res1[3]&=ledbuf2[6];//7
					else Res1[3]|=~ledbuf2[6];
					
					if(tcp_server_recvbuf[10]) Res1[4]&=ledbuf2[3];//4
						else Res1[4]|=~ledbuf2[3];
					if(tcp_server_recvbuf[11]) Res1[4]&=ledbuf2[5];//6
						else Res1[4]|=~ledbuf2[5];
				
			for(i=23;i<32;i++)
			{
				if(tcp_server_recvbuf[i+2]) Res1[3]|=ledbuf3[i-23];
				else Res1[3]&=~ledbuf3[i-23];
			}
//			TCA6424_W_BIT(TCA6424A_OUTPUT_REG0,res);
//			
				//}
				//else Res1[1]&=~ledbuf1[1];//pg2=0;
				
//				for(i=0;i<32;i++)
//			//i=5;
//			{	
//				if(i<23)
//				{
//				if(tcp_server_recvbuf[i+2])
//				{	
//				Res1&=ledbuf1[i];
//				Res1|=ledbuf2[i];
//				//TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,ledbuf1[i]);
//				//delay_us(10);
//				}
//				else 
//				{
//					Res1|=~ledbuf1[i];
//					//Res1&=~ledbuf2[i];
//				}
//				}
////				else 
////				{
////					if(tcp_server_recvbuf[i+2])
////				{	
////					//Res1&=ledbuf1[i];
////					Res1|=ledbuf2[i];
////				//TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,ledbuf1[i]);
////				//delay_us(10);
////				}
////				else 
////				{
////					Res1&=ledbuf1[i];
////				}
//			//}
//					//TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,00);
//			}
				//res4=(Res1&0x00ffffff);
				//Res1|=ledbuf2[4];
			//TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,Res1);
			//res=(u8)(Res1);
			//TCA6424_W_BIT(TCA6424A_OUTPUT_REG0,res);
//			res=(u8)(Res1>>8);
//			TCA6424_W_BIT(TCA6424A_OUTPUT_REG1,res);
//			res=(u8)(Res1>>16);
//			TCA6424_W_BIT(TCA6424A_OUTPUT_REG2,res);
//			memcpy(Imbuf[0],&tcp_server_recvbuf[2],32);
			restestbuf[0]=0xa0;
			restestbuf[1]=3;
			restestbuf[2]=0x32;
			restestbuf[3]=0;
			restestbuf[4]=0xc0;
			//restestbuf[67]=0xc0;
			len=restestbuf[1]+2;
			tcp_server_flag|=1<<7;
			tcp_server_sendbuf=restestbuf;
		break;
		case 0x33://�˳��迹����ģʽ
			
			ADS1299_Command(_STOP);
			ImpTest_Stop();
			ResTest_Flag=0;
			TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,0x783c00);
			restestbuf[0]=0xa0;
			restestbuf[1]=3;
			restestbuf[2]=0x33;
			restestbuf[3]=0;
			restestbuf[4]=0xc0;
			//restestbuf[67]=0xc0;
			len=restestbuf[1]+2;
			tcp_server_flag|=1<<7;
			tcp_server_sendbuf=restestbuf;
		break;
		
		default :
		break;
		}
	}
}



//UDP����
void udp_demo_test(void)
{
 	err_t err;
	struct udp_pcb *udppcb;  	//����һ��TCP���������ƿ�
	struct ip_addr rmtipaddr;  	//Զ��ip��ַ
 	
	u8 *tbuf;
// 	u8 key;
	u8 res=0;	
	u16 timer0,timer1;
	static u16 t=0; 
 	u8 i=0,j,k;
	//udp_demo_set_remoteip();//��ѡ��IP
//	LCD_Clear(WHITE);	//����
//	POINT_COLOR=RED; 	//��ɫ����
//	LCD_ShowString(30,30,200,16,16,"Apollo STM32F4/F7");
//	LCD_ShowString(30,50,200,16,16,"UDP Test");
//	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");  
//	LCD_ShowString(30,90,200,16,16,"KEY0:Send data");  
//	LCD_ShowString(30,110,200,16,16,"KEY_UP:Quit");  
	tbuf=mymalloc(SRAMIN,1000);	//�����ڴ�
	netmem_malloc();
	if(tbuf==NULL)return ;		//�ڴ�����ʧ����,ֱ���˳� 
	udppcb=udp_new();
	
	if(udppcb)//�����ɹ�
	{ 
		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
		err=udp_connect(udppcb,&rmtipaddr,UDP_DEMO_PORT);//UDP�ͻ������ӵ�ָ��IP��ַ�Ͷ˿ںŵķ�����
		if(err==ERR_OK)
		{
			err=udp_bind(udppcb,IP_ADDR_ANY,UDP_DEMO_PORT);//�󶨱���IP��ַ��˿ں�
			if(err==ERR_OK)	//�����
			{
				udp_recv(udppcb,udp_demo_recv,NULL);//ע����ջص����� 
//				//LCD_ShowString(30,190,210,16,16,"STATUS:Connected   ");//�����������(UDP�Ƿǿɿ�����,���������ʾ����UDP�Ѿ�׼����)
//				POINT_COLOR=RED;
//				LCD_ShowString(30,210,lcddev.width-30,lcddev.height-190,16,"Receive Data:");//��ʾ��Ϣ		
//				POINT_COLOR=BLUE;//��ɫ����
			}else res=1;
		}else res=1;		
	}else res=1;
	while(res==0)
	{
		//key=KEY_Scan(0);
		//if(key==WKUP_PRES)break;
		//if(key==KEY0_PRES)//KEY0������,��������
		
		if(tcp_server_flag&1<<7)//������Ҫ����)//KEY0������,��������
		{
			udp_demo_senddata(udppcb);
		}
		if(tcp_server_flag&1<<6)//�Ƿ��յ�����?
		{
			//LCD_Fill(30,230,lcddev.width-1,lcddev.height-1,WHITE);//����һ������
			Data_Process();
			//IWDG_Feed();    //ι��
			//LCD_ShowString(30,230,lcddev.width-30,lcddev.height-230,16,tcp_server_recvbuf);//��ʾ���յ�������			
			tcp_server_flag&=~(1<<6);//��������Ѿ���������.
		} 
		if(PwmledFlag==1)//�������̼���־λ��1��ִ������̼�����
		{
				timer0=led_pwmtime-Pwmledtimer1;
			timer1=led_shtime-ledcjtimer;
			if(timer0>Pwmledtimer)
			{
				TIM_SetTIM2(0);//�ر�����
				PwmledFlag=0;   //��λ�����־λ
			}
			if((timer1>Pwmledfre)&&(LedhighFlag==1))
			{
				TIM_SetTIM2(0);//�ر�����
				ledcjtimer=led_shtime;
				LedhighFlag=0;
			}
			else if((LedhighFlag==0)&&(timer1>Pwmledfre))   
			{
				TIM_SetTIM2(Pwmledduty);
				ledcjtimer=led_shtime;
				LedhighFlag=1;
			}
		}
		lwip_periodic_handle();
		//LED2=0;
		//delay_ms(2);
		t++;
		//if(Ad_Flag==0) 
			IWDG_Feed();    //ι��
		//else if(Ad_Flag==1)
		//{
		if(t>=500)
		{
			k++;
			LED2=k%2;
			t=0;
			if(Imline_Flag==1)
			{
			switch(i)
			{
				case 0x0:
					if(!(stat[0]&(1))) Res1[0]&=ledbuf2[5];//1
						else Res1[0]|=~ledbuf2[5];		
					if(!(stat[0]&(0x01<<1))) Res1[0]&=ledbuf2[3];//2
						else Res1[0]|=~ledbuf2[3];
				
					if(!(stat[2]&(1<<6))) Res1[0]&=ledbuf2[6];//23
					else Res1[0]|=~ledbuf2[6];
					if(!(stat[2]&(1<<5))) Res1[0]&=ledbuf2[2];//22
					else Res1[0]|=~ledbuf2[2];
					TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,Res1[i]);
					i=0x01;
				break;
				case 0x01:
					if(!(stat[0]&(1<<2))) Res1[1]&=ledbuf2[5];//6
						else Res1[1]|=(~ledbuf2[5]);
					if(!(stat[0]&(1<<3))) Res1[1]&=ledbuf2[3];//4
						else Res1[1]|=(~ledbuf2[3]);
					if(!(stat[1]&(1<<2))) Res1[1]&=ledbuf2[6];//7
					else Res1[1]|=~ledbuf2[6];
					if(!(stat[1]&(1<<3))) Res1[1]&=ledbuf2[2];//3
					else Res1[1]|=~ledbuf2[2];
					if(!(stat[2]&(1<<2))) Res1[1]&=ledbuf2[4];//5
					else Res1[1]|=~ledbuf2[4];
					TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,Res1[i]);
					i=0x02;
				break;
				case 0x02:
					if(!(stat[2]&(1<<0))) Res1[2]&=ledbuf2[7];//8
					else Res1[2]|=~ledbuf2[7];
						
					if(!(stat[1]&(1<<4))) Res1[2]&=ledbuf2[6];//7
					else Res1[2]|=~ledbuf2[6];
					if(!(stat[0]&(1<<4))) Res1[2]&=ledbuf2[5];//4
						else Res1[2]|=(~ledbuf2[5]);
					if(!(stat[2]&(1<<3))) Res1[2]&=ledbuf2[4];//5
					else Res1[2]|=~ledbuf2[4];
					if(!(stat[0]&(1<<5))) Res1[2]&=ledbuf2[3];//6
					else Res1[2]|=~ledbuf2[3];
					if(!(stat[1]&(1<<5)))Res1[2]&=ledbuf2[2];//3
						else Res1[2]|=(~ledbuf2[2]);
					if(!(stat[2]&(1<<1))) Res1[2]&=ledbuf2[1];//2
						else Res1[2]|=(~ledbuf2[1]);
					TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,Res1[i]);
						i=0x03;
				break;
				
				case 0x03:	
					if(!(stat[1]&(1<<6))) Res1[3]&=ledbuf2[6];//7
					else Res1[3]|=~ledbuf2[6];
					if(!(stat[0]&(1<<6))) Res1[3]&=ledbuf2[5];//4
						else Res1[3]|=(~ledbuf2[5]);
					if(!(stat[2]&(1<<4))) Res1[3]&=ledbuf2[4];//5
					else Res1[3]|=~ledbuf2[4];
					if(!(stat[0]&(1<<7))) Res1[3]&=ledbuf2[3];//6
					else Res1[3]|=~ledbuf2[3];
					if(!(stat[1]&(1<<7))) Res1[3]&=ledbuf2[2];//3
						else Res1[3]|=(~ledbuf2[2]);
					TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,Res1[i]);
					i=0x04;
				break;
				case 0x04:	
					if(!(stat[1]&(1<<0))) Res1[4]&=ledbuf2[5];//4
						else Res1[4]|=~ledbuf2[5];
					if(!(stat[1]&(1<<1))) Res1[4]&=ledbuf2[3];//6
						else Res1[4]|=~ledbuf2[3];
					TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,Res1[i]);
						i=0x05;
				break;
				case 0x05:
					if(!(stat[2]&(1<<7))) Res1[i]|=ledbuf3[0];
						else Res1[i]&=~ledbuf3[0];
					for(j=0;j<8;j++)
					{
						if(!(stat[3]&(1<<j))) 
						{
							Res1[i]|=ledbuf3[j+1];
							//PWM_LED=0;
						}
						else Res1[i]&=~ledbuf3[j+1];
					}
					TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,Res1[i]);
					i=0;
					break;
				default:
					break;
					}	
				}
			else TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,0X200);
			//if(ResTest_Flag==1)
			//{
				//TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,Res1[i]);
				//TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,Res1[5]);
				//if(i>=0x06) i=0;
				//else i=0;
			//}
			
		}
		//else t=0;
	}
	udp_demo_connection_close(udppcb); 
	myfree(SRAMIN,tbuf);
	netmem_free();
} 

//UDP�������ص�����
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	if(p!=NULL)	//���յ���Ϊ�յ�����ʱ
	{
		memset(tcp_server_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //���ݽ��ջ���������
		for(q=p;q!=NULL;q=q->next)  //����������pbuf����
		{
			//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
			//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(tcp_server_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//��������
			else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
			data_len += q->len;  	
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
		}
		upcb->remote_ip=*addr; 				//��¼Զ��������IP��ַ
		upcb->remote_port=port;  			//��¼Զ�������Ķ˿ں�
		lwipdev.remoteip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwipdev.remoteip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwipdev.remoteip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1 
		tcp_server_flag|=1<<6;	//��ǽ��յ�������
		pbuf_free(p);//�ͷ��ڴ�
	}else
	{
		udp_disconnect(upcb); 
	} 
} 
//UDP��������������
void udp_demo_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr2;
	//ptr1=pbuf_alloc(PBUF_TRANSPORT,len/2,PBUF_RAM); //�����ڴ�
	ptr2=pbuf_alloc(PBUF_TRANSPORT,len,PBUF_RAM); //�����ڴ�
	if(ptr2)
	{
		//ptr2->next=ptr1;
		//pbuf_take(ptr1,(char*)tcp_server_sendbuf,len/2); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
		pbuf_take(ptr2,(char*)tcp_server_sendbuf,len); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
		//ptr2->len=len;
		//ptr2->payload=(void*)tcp_server_sendbuf; 
		//ptr2->payload=(void*)&tcp_server_sendbuf[len/2];
		//pbuf_chain(ptr1,ptr2);
		//udp_send(upcb,ptr1);	//udp�������� 
		udp_send(upcb,ptr2);	//udp�������� 
		//pbuf_take(ptr,(char*)tcp_server_sendbuf+2100,1050); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
		//udp_send(upcb,ptr);	//udp�������� 
//		pbuf_take(ptr,(char*)tcp_server_sendbuf+3150,1050); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
//		udp_send(upcb,ptr);	//udp�������� 
		//pbuf_take(ptr,(char*)tcp_server_sendbuf+4200,1050); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
		//udp_send(upcb,ptr);	//udp�������� 
		//ptr->payload=(void*)tcp_server_sendbuf+len;
		//udp_send(upcb,ptr);	//udp�������� 
		//ptr->payload=(void*)netcam_line_buf0;
		//udp_send(upcb,ptr);	//udp�������� 
		//ptr->payload=(void*)tcp_server_sendbuf[len];
		//pbuf_free(ptr1);//�ͷ��ڴ�
		pbuf_free(ptr2);//�ͷ��ڴ�
		 tcp_server_flag&=~(1<<7);	    //������ݷ��ͱ�־
	} 
//	ptr=pbuf_alloc(PBUF_TRANSPORT,1050,PBUF_POOL); //�����ڴ�
//	if(ptr)
//	{
//		pbuf_take(ptr,(char*)netcam_line_buf0,1050); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
//		//ptr->payload=(void*)netcam_line_buf0;
//		udp_send(upcb,ptr);	//udp�������� 
//		pbuf_free(ptr);//�ͷ��ڴ�
//		  tcp_server_flag&=~(1<<7);	    //������ݷ��ͱ�־
//	}
} 

////�˺���������������
//void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
//{
//	struct pbuf *ptr;
//	u16 plen;
//	err_t wr_err=ERR_OK;
//	while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb)))
//	{
//		ptr=es->p;
//		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
//		if(wr_err==ERR_OK)
//		{ 
//			plen=ptr->len;
//			es->p=ptr->next;			//ָ����һ��pbuf
//			if(es->p)pbuf_ref(es->p);	//pbuf��ref��һ
//			pbuf_free(ptr);
//			tcp_recved(tpcb,plen); 		//����tcp���ڴ�С
//		}else if(wr_err==ERR_MEM)es->p=ptr;
//        tcp_output(tpcb);		        //�����ͻ�������е������������ͳ�ȥ
//    }
//    
//} 
//�ر�tcp����
void udp_demo_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb); 
	udp_remove(upcb);		//�Ͽ�UDP���� 
}
































