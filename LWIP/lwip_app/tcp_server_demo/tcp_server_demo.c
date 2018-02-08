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
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
//TCP Server�������ݻ�����
u8 tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	
//TCP������������������
u8 *tcp_server_sendbuf;//="Apollo STM32F4/F7 TCP Server send data\r\n";


//TCP Server ����ȫ��״̬��Ǳ���
//bit7:0,û������Ҫ����;1,������Ҫ����
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û�пͻ���������;1,�пͻ�����������.
//bit4~0:����
u8 tcp_server_flag;	 
 u8 Num;
 u32 len;
u8 sendsetbuf[5]={0xa0,3,0x11,00,0xc0};
u8 setfirbuf[5]={0xa0,3,0x12,0,0xc0};
u8 setnorthbuf[5]={0xa0,3,0x13,0,0xc0};
//u8 restestbuf[6]={0xa0,4,0x31,0,0,0xc0};
u8 restestbuf[68];//={0xa0,66,0x33,0,0,0,0,0

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
	u16 i;
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
	
		memcpy(&netcamfifobuf[netcamfifowrpos*res],buf,res);
//		else
//		{
//			memcpy(&netcamfifobuf[netcamfifowrpos*NETCAM_LINE_SIZE],buf,NETCAM_LINE_SIZE);
//		}
		netcamfifowrpos++;			//дλ�ü�1
		if(netcamfifowrpos<NETCAM_FIFO_NUM)return 0;
		else
		{
			netcam_fifo_read(&tcp_server_sendbuf);
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
    //netcam_line_buf0=mymalloc(SRAMIN,NETCAM_LINE_SIZE);
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
static void spi2_rx_dma_callback(void)
{    
	//if(DMA1_Stream3->CR&(1<<19))netcam_fifo_write(netcam_line_buf0);	//netcam_line_buf0д��FIFO
	//else netcam_fifo_write(netcam_line_buf1);	
	//netcam_fifo_write(adc_buf2);
	//tcp_server_flag|=(1<<7);//������Ҫ����
}
//openbci��ʼ��
void hardware_init(void)
{
	
	
	
	
	//while(1) 									//��ʼ��tcp_server(����tcp_server��)
	{
		//LCD_ShowString(30,110,200,20,16,"ADS1299 Check failed! "); 
		//delay_ms(500);
		//LCD_Fill(30,110,230,170,WHITE);
		if(ADS1299_Check()) delay_ms(100);
		//else break;
		
	}
	//LCD_ShowString(30,150,200,20,16,"ads1299 check Success!  "); 	//tcp�����������ɹ�
	
	// spi2_rx_callback=spi2_rx_dma_callback;//�������ݻص�����
    // BCI_DMA_Init(netcam_line_buf0[0],netcam_line_buf1[0],NETCAM_LINE_SIZE); 
	//ADS1299_DMA_Config(DMA1_Stream3,DMA_CHANNEL_0);//��ʼ��DMA
	//ADS1299_DMA_Start();
	
	ADS1299_IT();		//�ж�����
}
 
//TCP Server ����
void tcp_server_test(void)
{
	err_t err;  
	struct tcp_pcb *tcppcbnew;  	//����һ��TCP���������ƿ�
	struct tcp_pcb *tcppcbconn;  	//����һ��TCP���������ƿ�
	
	u8 *tbuf;
 	u8 key;
	u8 res=0,res1=0;		
	u16 t=0; 
	
	LED1=1;
	//netmem_free();
	tbuf=mymalloc(SRAMIN,200);	//�����ڴ�
	netmem_malloc();
	if(tbuf==NULL)return ;		//�ڴ�����ʧ����,ֱ���˳�
	//sprintf((char*)tbuf,"Server IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//������IP
	//sprintf((char*)tbuf,"Server Port:%d",TCP_SERVER_PORT);//�������˿ں� 
	tcppcbnew=tcp_new();	//����һ���µ�pcb
	if(tcppcbnew)			//�����ɹ�
	{ 
		err=tcp_bind(tcppcbnew,IP_ADDR_ANY,TCP_SERVER_PORT);	//������IP��ָ���Ķ˿ںŰ���һ��,IP_ADDR_ANYΪ�󶨱������е�IP��ַ
		if(err==ERR_OK)	//�����
		{
			tcppcbconn=tcp_listen(tcppcbnew); 			//����tcppcb�������״̬
			tcp_accept(tcppcbconn,tcp_server_accept); 	//��ʼ��LWIP��tcp_accept�Ļص�����
		}else res=1;  
	}else res=1;
	delay_ms(100);
	hardware_init();
	if((tcp_server_flag&1<<5)&&(res==0))
	{
		res1=1;
	}
	while(res1==1)//����Ѿ�����
	{
//		while(res1==1)
//		{
		if(tcp_server_flag&1<<7)//������Ҫ����)//KEY0������,��������
		{
			//netcam_fifo_read(&tcp_server_sendbuf);
			//tcp_server_sendbuf=netcamfifobuf;
            tcp_server_usersent(tcppcbnew);//��������
		}
		if(tcp_server_flag&1<<6)//�Ƿ��յ�����?
		{
			Data_Process();
			//LCD_Fill(30,210,lcddev.width-1,lcddev.height-1,WHITE);//����һ������
			//LCD_ShowString(30,210,lcddev.width-30,lcddev.height-210,16,tcp_server_recvbuf);//��ʾ���յ�������	
			//�˴���ͨ�����ݽ��н���
			//��ͷ��β�Ƿ���ȷ
			//�������ж�
			//21���豸�����ϴ�
			//11������������
			//12:�ղ�����
			//13���˲�����
			//01��AD���ݲɼ�
			tcp_server_flag&=~(1<<6);//��������Ѿ���������.
		}
		
		//}
		if((tcp_server_flag&1<<5)==0)
		{
			res1=0;
			//LCD_Fill(30,170,lcddev.width-1,lcddev.height-1,WHITE);//����
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
	tcp_server_connection_close(tcppcbnew,0);//�ر�TCP Server����
	tcp_server_connection_close(tcppcbconn,0);//�ر�TCP Server���� 
	tcp_server_remove_timewait(); 
	memset(tcppcbnew,0,sizeof(struct tcp_pcb));
	memset(tcppcbconn,0,sizeof(struct tcp_pcb)); 
	myfree(SRAMIN,tbuf);
	netmem_free();
	
} 
//lwIP tcp_accept()�Ļص�����
err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err)
{
	err_t ret_err;
	struct tcp_server_struct *es; 
 	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);
	tcp_setprio(newpcb,TCP_PRIO_MIN);//�����´�����pcb���ȼ�
	es=(struct tcp_server_struct*)mem_malloc(sizeof(struct tcp_server_struct)); //�����ڴ�
 	if(es!=NULL) //�ڴ����ɹ�
	{
		es->state=ES_TCPSERVER_ACCEPTED;  	//��������
		es->pcb=newpcb;
		es->p=NULL;
		
		tcp_arg(newpcb,es);
		tcp_recv(newpcb,tcp_server_recv);	//��ʼ��tcp_recv()�Ļص�����
		tcp_err(newpcb,tcp_server_error); 	//��ʼ��tcp_err()�ص�����
		tcp_poll(newpcb,tcp_server_poll,1);	//��ʼ��tcp_poll�ص�����
		tcp_sent(newpcb,tcp_server_sent);  	//��ʼ�����ͻص�����
		  
		tcp_server_flag|=1<<5;				//����пͻ���������
		lwipdev.remoteip[0]=newpcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(newpcb->remote_ip.addr>>8)&0xff;  	//IADDR3
		lwipdev.remoteip[2]=(newpcb->remote_ip.addr>>16)&0xff; 	//IADDR2
		lwipdev.remoteip[3]=(newpcb->remote_ip.addr>>24)&0xff; 	//IADDR1 
		ret_err=ERR_OK;
	}else ret_err=ERR_MEM;
	return ret_err;
}
//lwIP tcp_recv()�����Ļص�����
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	err_t ret_err;
	u32 data_len = 0;
	struct pbuf *q;
  	struct tcp_server_struct *es;
	LWIP_ASSERT("arg != NULL",arg != NULL);
	es=(struct tcp_server_struct *)arg;
	if(p==NULL) //�ӿͻ��˽��յ�������
	{
		LED1=1;
		es->state=ES_TCPSERVER_CLOSING;//��Ҫ�ر�TCP ������
		es->p=p; 
		ret_err=ERR_OK;
	}else if(err!=ERR_OK)	//�ӿͻ��˽��յ�һ���ǿ�����,��������ĳ��ԭ��err!=ERR_OK
	{
		if(p)pbuf_free(p);	//�ͷŽ���pbuf
		ret_err=err;
		LED1=1;
	}else if(es->state==ES_TCPSERVER_ACCEPTED) 	//��������״̬
	{
		if(p!=NULL)  //����������״̬���ҽ��յ������ݲ�Ϊ��ʱ�����ӡ����
		{
			memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //���ݽ��ջ���������
			for(q=p;q!=NULL;q=q->next)  //����������pbuf����
			{
				//�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
				//�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
				if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//��������
				else memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
				data_len += q->len;  	
				if(data_len > TCP_SERVER_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
			}
			tcp_server_flag|=1<<6;	//��ǽ��յ�������
			lwipdev.remoteip[0]=tpcb->remote_ip.addr&0xff; 		//IADDR4
			lwipdev.remoteip[1]=(tpcb->remote_ip.addr>>8)&0xff; //IADDR3
			lwipdev.remoteip[2]=(tpcb->remote_ip.addr>>16)&0xff;//IADDR2
			lwipdev.remoteip[3]=(tpcb->remote_ip.addr>>24)&0xff;//IADDR1 
 			tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
			pbuf_free(p);  	//�ͷ��ڴ�
			ret_err=ERR_OK;
		}
	}else//�������ر���
	{
		tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
		es->p=NULL;
		pbuf_free(p); //�ͷ��ڴ�
		ret_err=ERR_OK;
		tcp_server_flag&=~(1<<5);//������ӶϿ���
	}
	return ret_err;
}
//lwIP tcp_err�����Ļص�����
void tcp_server_error(void *arg,err_t err)
{  
	LWIP_UNUSED_ARG(err);  
	tcp_server_flag&=~(1<<5);//������ӶϿ���
	//printf("tcp error:%x\r\n",(u32)arg);
	if(arg!=NULL)mem_free(arg);//�ͷ��ڴ�
} 

//LWIP���ݷ��ͣ��û�Ӧ�ó�����ô˺�������������
//tpcb:TCP���ƿ�
//����ֵ:0���ɹ���������ʧ��
err_t tcp_server_usersent(struct tcp_pcb *tpcb)
{
    err_t ret_err;
	struct tcp_server_struct *es; 
	es=tpcb->callback_arg;
	if(es!=NULL)  //���Ӵ��ڿ��п��Է�������
	{
//es->p=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)tcp_server_sendbuf),PBUF_POOL);	//�����ڴ� 
       // pbuf_take(es->p,(char*)tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf));	//��tcp_server_sentbuf[]�е����ݿ�����es->p_tx��
		es->p=pbuf_alloc(PBUF_TRANSPORT, len,PBUF_POOL);	//�����ڴ� 
        pbuf_take(es->p,(char*)tcp_server_sendbuf,len);	//��tcp_server_sentbuf[]�е����ݿ�����es->p_tx��
        tcp_server_senddata(tpcb,es);   //��tcp_server_sentbuf[]���渴�Ƹ�pbuf�����ݷ��ͳ�ȥ
        tcp_server_flag&=~(1<<7);	    //������ݷ��ͱ�־
        if(es->p!=NULL)pbuf_free(es->p);//�ͷ��ڴ�
		ret_err=ERR_OK;
	}else
	{ 
		LED1=1;
		tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
		ret_err=ERR_ABRT;
		LED1=1;
	}
	return ret_err;
}

//lwIP tcp_poll�Ļص�����
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_server_struct *es; 
	es=(struct tcp_server_struct *)arg; 
    if(es->state==ES_TCPSERVER_CLOSING)//��Ҫ�ر�����?ִ�йرղ���
    {
        tcp_server_connection_close(tpcb,es);//�ر�����
		//tcp_server_flag&=~(1<<5);//������ӶϿ���
		LED1=1;
    }
    ret_err=ERR_OK;
	return ret_err;
} 
//lwIP tcp_sent�Ļص�����(����Զ���������յ�ACK�źź�������)
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	struct tcp_server_struct *es;
	LWIP_UNUSED_ARG(len); 
	es = (struct tcp_server_struct *) arg;
	if(es->p)tcp_server_senddata(tpcb,es);//��������
	return ERR_OK;
} 
//�˺���������������
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
			es->p=ptr->next;			//ָ����һ��pbuf
			if(es->p)pbuf_ref(es->p);	//pbuf��ref��һ
			pbuf_free(ptr);
			tcp_recved(tpcb,plen); 		//����tcp���ڴ�С
		}else if(wr_err==ERR_MEM)es->p=ptr;
        tcp_output(tpcb);		        //�����ͻ�������е������������ͳ�ȥ
    }
    
} 
//�ر�tcp����
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
	tcp_close(tpcb);
	tcp_arg(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_recv(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);
	if(es)mem_free(es); 
	tcp_server_flag&=~(1<<5);//������ӶϿ���
}
extern void tcp_pcb_purge(struct tcp_pcb *pcb);	//�� tcp.c���� 
extern struct tcp_pcb *tcp_active_pcbs;			//�� tcp.c���� 
extern struct tcp_pcb *tcp_tw_pcbs;				//�� tcp.c����  
//ǿ��ɾ��TCP Server�����Ͽ�ʱ��time wait
void tcp_server_remove_timewait(void)
{
	struct tcp_pcb *pcb,*pcb2; 
	u8 t=0;
	while(tcp_active_pcbs!=NULL&&t<200)
	{
		lwip_periodic_handle();	//������ѯ
		t++;
		delay_ms(10);			//�ȴ�tcp_active_pcbsΪ��  
	}
	pcb=tcp_tw_pcbs;
	while(pcb!=NULL)//����еȴ�״̬��pcbs
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
		case 0x1:  //���ݲɼ�����
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
				ADS1299_START=1;//ADS1299оƬ����
			}
		else if(res==0)
			ADS1299_START=0;
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
		
		case 0x12://�ݲ�����
			tcp_server_sendbuf=setfirbuf;
			len=setfirbuf[1]+2;
			tcp_server_flag|=1<<7;
		break;
		
		case 0x13: //�˲�����
			tcp_server_sendbuf=setnorthbuf;
			len=setnorthbuf[1]+2;
			tcp_server_flag|=1<<7;
		break;
		
		case 0x21://�豸������ѯ
			tcp_server_flag|=1<<7;
			tcp_server_sendbuf=parameter;
			len=parameter[1]+2;
		break;
		case 0x31://����ͨ���迹����
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
		case 0x32://N��ͨ���迹ͬʱ����
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































