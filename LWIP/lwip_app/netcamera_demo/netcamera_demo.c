#include "netcamera_demo.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "led.h"
#include "ov5640.h"
#include "dcmi.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "delay.h"
#include "malloc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32������
//��������ͷ����  
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/2/24
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
//��������ͷ����
//�����������ȼ�
#define NETCAMERA_TASK_PRIO       		   2 
//���������ջ��С
#define NETCAMERA_STK_SIZE  		 	   1500
//�����ջ	
OS_STK NETCAMERA_TASK_STK[NETCAMERA_STK_SIZE];	
//������
void netcamera_task(void *pdata); 

                                        
struct netconn *newconn=0;		            //NET CAM TCP�������ӽṹ��ָ�� 
u32 *netcam_line_buf0;					    //�����л���0  
u32 *netcam_line_buf1;					    //�����л���1   

//NET CAM FIFO
vu16 netcamfifordpos=0;					    //FIFO��λ��
vu16 netcamfifowrpos=0;					    //FIFOдλ��
u32 *netcamfifobuf[NETCAM_FIFO_NUM];	    //����NETCAM_FIFO_SIZE������FIFO			

//��ȡFIFO
//buf:���ݻ������׵�ַ
//����ֵ:0,û�����ݿɶ�;
//      1,������1�����ݿ�
u8 netcam_fifo_read(u32 **buf)
{
	if(netcamfifordpos==netcamfifowrpos)return 0;
	netcamfifordpos++;		//��λ�ü�1
	if(netcamfifordpos>=NETCAM_FIFO_NUM)netcamfifordpos=0;//���� 
	*buf=netcamfifobuf[netcamfifordpos];
	return 1;
}
//дһ��FIFO
//buf:���ݻ������׵�ַ
//����ֵ:0,д��ɹ�;
//      1,д��ʧ��
u8 netcam_fifo_write(u32 *buf)
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


//��Ӧ�������ڴ�����
//����ֵ:0 �ɹ������� ʧ��
u8 netmem_malloc(void)
{
    u16 t=0;
    netcam_line_buf0=mymalloc(SRAMIN,NETCAM_LINE_SIZE*4);
	netcam_line_buf1=mymalloc(SRAMIN,NETCAM_LINE_SIZE*4);

    //��FIFO�����ڴ�
    for(t=0;t<NETCAM_FIFO_NUM;t++) 
	{
		netcamfifobuf[t]=mymalloc(SRAMEX,NETCAM_LINE_SIZE*4);
	}  
    if(!netcamfifobuf[NETCAM_FIFO_NUM-1]||!netcam_line_buf1||!netcam_line_buf0)//�ڴ�����ʧ��  
    {
        netmem_free();//�ͷ��ڴ�
        return 1;
    }
    return 0;
}

//��Ӧ�������ڴ��ͷ�
void netmem_free(void)
{
    u16 t=0;
    myfree(SRAMIN,netcam_line_buf0);
    myfree(SRAMIN,netcam_line_buf1);
    //�ͷ�FIFO���ڴ�
    for(t=0;t<NETCAM_FIFO_NUM;t++) 
	{
        myfree(SRAMEX,netcamfifobuf[t]);
	}  
}

//������������ͷ�ֱ���
//resol: 0,320*240
//      1,640*480
//      3,1024*768
//      4,1280*800
void netcam_resolu_set(u8 resol)
{
    switch(resol)
    {
        case 0: 
            OV5640_OutSize_Set(16,4,320,240);	
            break;
        case 1: 
            OV5640_OutSize_Set(16,4,640,480);	
            break;
        case 2: 
            OV5640_OutSize_Set(16,4,1024,768);	
            break;
        case 3: 
            OV5640_OutSize_Set(16,4,1280,800);	
            break;
        default:
            OV5640_OutSize_Set(16,4,1280,800);	//Ĭ��Ϊ1280*800
            break;               
    }
}    
    
//����ͷjpeg���ݽ��ջص�����
static void netcam_dcmi_rx_callback(void)
{    
	if(DMA2_Stream1->CR&(1<<19))netcam_fifo_write(netcam_line_buf0);	//netcam_line_buf0д��FIFO
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
     BCI_DMA_Init((u32)netcam_line_buf0,0,NETCAM_LINE_SIZE,DMA_MDATAALIGN_BYTE,DMA_MINC_ENABLE); 
	//ADS1299_DMA_Config(DMA1_Stream3,DMA_CHANNEL_0);//��ʼ��DMA
	//ADS1299_Start();
	ADS1299_IT();		//�ж�����
}






//��������ͷ��ʼ��
u8 camera_init(void)
{
    u8 rval=0;
    
    netmem_malloc();//�����ڴ�
    //����OV5640
    if(OV5640_Init())	    //��ʼ��OV5640
	{
		printf("ov5640 error\r\n");
		delay_ms(500);  
        rval=1;
	}  
    if(rval==0)
    {
        OV5640_RGB565_Mode();	//RGB565ģʽ 
        OV5640_Focus_Init();    //�Զ��Խ���ʼ��
        OV5640_Light_Mode(0);	//�Զ�ģʽ
        OV5640_Color_Saturation(3);//ɫ�ʱ��Ͷ�0
        OV5640_Brightness(4);	//����0
        OV5640_Contrast(3);		//�Աȶ�0
        OV5640_Sharpness(33);	//�Զ����
        OV5640_Focus_Constant();//���������Խ�
        DCMI_Init();			//DCMI����
        dcmi_rx_callback=netcam_dcmi_rx_callback;//JPEG�������ݻص�����
        DCMI_DMA_Init((u32)netcam_line_buf0,(u32)netcam_line_buf1,NETCAM_LINE_SIZE,DMA_MDATAALIGN_WORD,DMA_MINC_ENABLE);  
        OV5640_JPEG_Mode();		//JPEGģʽ
        //��Ϊ15֡
//        OV5640_WR_Reg(0X3035,0X11);
//        OV5640_WR_Reg(0x3824,0X04);
//        OV5640_ImageWin_Set(0,0,1280,800);

        OV5640_OutSize_Set(16,4,1280,800);		//��������ߴ�(1280*800)  
        printf("����DCMI\r\n");
        DCMI_Start(); 		//��������         
        delay_ms(100);
        printf("��ʼ�����\r\n");
    } 
    return rval;
}

//��������ͷ��λ
void camera_deinit(void)
{
    netmem_free();      //�ͷ��ڴ�
    netcamfifordpos=0;  //��λ������
    netcamfifowrpos=0;  //дλ������
    DCMI_Stop();        //�رմ���
    HAL_DCMI_DeInit(&DCMI_Handler); //��λDCMI
    HAL_DMA_DeInit(&DMADMCI_Handler);//��λDMA
}
    
//��������ͷ����
void netcamera_task(void *arg)
{
	err_t err;
	struct netconn *conn;
    static ip_addr_t ipaddr;
    u8 remot_addr[4];
	static u16_t port;
    
    u8 res=0;
	u32 *tbuf;
    
	LWIP_UNUSED_ARG(arg);

	conn = netconn_new(NETCONN_TCP);  //����һ��TCP����
	netconn_bind(conn,IP_ADDR_ANY,8088);  //�󶨶˿� 8088�Ŷ˿�
	netconn_listen(conn);  		//�������ģʽ
    
    while(1)    //�ȴ�����
    {
        err=netconn_accept(conn,&newconn);  //������������
			
        if(err==ERR_OK) //�ɹ���⵽����
        {
            netconn_getaddr(newconn,&ipaddr,&port,0); //��ȡԶ��IP��ַ�Ͷ˿ں�
            remot_addr[3]=(uint8_t)(ipaddr.addr>>24); 
            remot_addr[2]=(uint8_t)(ipaddr.addr>>16);
            remot_addr[1]=(uint8_t)(ipaddr.addr>>8);
            remot_addr[0]=(uint8_t)(ipaddr.addr);
            printf("����%d.%d.%d.%d��������������ͷ������,�����˿ں�Ϊ:%d\r\n",\
                    remot_addr[0],remot_addr[1],remot_addr[2],remot_addr[3],port);
            camera_init();
            delay_ms(1000); //����ʱһ��Ҫ�ӣ���
            while(1)        //��ʼ��Ƶ����
            {
                res=netcam_fifo_read(&tbuf);    //��ȡFIFO�е�����
                if(res)     //������Ҫ����
                {
                    err=netconn_write(newconn,tbuf,NETCAM_LINE_SIZE*4,NETCONN_COPY);//��������
                    if((err==ERR_CLSD)||(err==ERR_RST))//�ر�����,������������ 
                    {
                        camera_deinit();//��λ��������ͷ
                        netconn_close(newconn);
                        netconn_delete(newconn);
                        printf("����:%d.%d.%d.%d�Ͽ�����Ƶ������������\r\n",\
                            remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
                        break;
                    }	                    
                }else
                { 
                    OSTimeDlyHMSM(0,0,0,2);//��ʱ2ms
                }	
            }
        }
    }
   
}

//������������ͷ�߳�
//����ֵ:0 �����ɹ�
//		���� ����ʧ��
INT8U netcamera_init(void)
{
	INT8U res;
	OS_CPU_SR cpu_sr;

	OS_ENTER_CRITICAL();	//���ж�
	res = OSTaskCreate(netcamera_task,(void*)0,(OS_STK*)&NETCAMERA_TASK_STK[NETCAMERA_STK_SIZE-1],NETCAMERA_TASK_PRIO); //������������ͷ�߳�
	OS_EXIT_CRITICAL();		//���ж�
	
	return res;
}

