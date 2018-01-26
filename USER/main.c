#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lan8720.h"
#include "timer.h"
#include "usmart.h"
#include "malloc.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "openbci.h"
#include "ads1299.h"
#include "ad5933.h"
#include "TCA6424A.h"
#include "adg731.h"
#include "myiic.h"
/************************************************

************************************************/

////KEY����
//#define KEY_TASK_PRIO 		8
////�����ջ��С
//#define KEY_STK_SIZE		64		
////�����ջ
//OS_STK KEY_TASK_STK[KEY_STK_SIZE];
////������
//void key_task(void *pdata);  

////��LCD����ʾ��ַ��Ϣ����
////�������ȼ�
//#define DISPLAY_TASK_PRIO	9
////�����ջ��С
//#define DISPLAY_STK_SIZE	128
////�����ջ
//OS_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
////������
//void display_task(void *pdata);


//ADS1299���ݽ�������
//�������ȼ�
#define ADS1299_TASK_PRIO		2
//�����ջ��С
#define ADS1299_STK_SIZE		500
//�����ջ
OS_STK	ADS1299_TASK_STK[ADS1299_STK_SIZE];
//������
void ads1299_task(void *pdata);  


//START����
//�������ȼ�
#define START_TASK_PRIO		11
//�����ջ��С
#define START_STK_SIZE		128
//�����ջ
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata); 





int main(void)
{   
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��,180Mhz   
    HAL_Init();                     //��ʼ��HAL��
    delay_init(180);                //��ʼ����ʱ����
    uart_init(115200);              //��ʼ��USART
    usmart_dev.init(90); 		    //��ʼ��USMART	
    LED_Init();                     //��ʼ��LED 
   	IIC_Init();
	Adg731_Init();
	ADS1299_Init();
	TCA6424A_Init();   
	my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
	my_mem_init(SRAMCCM);		    //��ʼ��CCM�ڴ��
	OSInit(); 					    //UCOS��ʼ��
	while(lwip_comm_init()) 	    //lwip��ʼ��
	{
		
		delay_ms(500);
	}
    openbci_init();
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart(); //����UCOS
}

//start����
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//��ʼ��ͳ������
	OS_ENTER_CRITICAL();  	//���ж�
#if LWIP_DHCP
	lwip_comm_dhcp_creat(); //����DHCP����
#endif
	
	OSTaskCreate(ads1299_task,(void*)0,(OS_STK*)&ADS1299_TASK_STK[ADS1299_STK_SIZE-1],ADS1299_TASK_PRIO);//����ADS1299�ɼ�����
    //OSTaskCreate(key_task,(void*)0,(OS_STK*)&KEY_TASK_STK[KEY_STK_SIZE-1],KEY_TASK_PRIO);//��������
	//OSTaskCreate(display_task,(void*)0,(OS_STK*)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE-1],DISPLAY_TASK_PRIO); //��ʾ����
	OSTaskSuspend(OS_PRIO_SELF); //����start_task����
	OS_EXIT_CRITICAL();  		//���ж�
}

////��ʾ��ַ����Ϣ
//void display_task(void *pdata)
//{
//	while(1)
//	{ 
//#if LWIP_DHCP									//������DHCP��ʱ��
//		if(lwipdev.dhcpstatus != 0) 			//����DHCP
//		{
//			show_inform(lwipdev.dhcpstatus );	//��ʾ��ַ��Ϣ
//			OSTaskSuspend(OS_PRIO_SELF); 		//��ʾ���ַ��Ϣ�������������
//		}
//#else
//		show_inform(0); 						//��ʾ��̬��ַ
//		OSTaskSuspend(OS_PRIO_SELF); 		 	//��ʾ���ַ��Ϣ�������������
//#endif //LWIP_DHCP
//		OSTimeDlyHMSM(0,0,0,500);
//	}
//}

////key����
//void key_task(void *pdata)
//{
//    u8 resolut=0;
//	u8 key=0; 
//	while(1)
//	{
//        key=KEY_Scan(0);
//        if(key==WKUP_PRES)//�Խ�
//        {
//           // OV5640_Focus_Single();
//        }
//        if(key==KEY0_PRES)//�����ֱ���
//        {
//            resolut++;
//            if(resolut==4) resolut=0;
//           // netcam_resolu_set(resolut);//���÷ֱ���
//            switch(resolut)            //��ʾ�����õķֱ���
//            {
//                case 0:
//                    LCD_ShowString(78,170,210,16,16,"320*240 ");
//                    break;
//                case 1:
//                    LCD_ShowString(78,170,210,16,16,"640*480 ");
//                    break;
//                case 2:
//                    LCD_ShowString(78,170,210,16,16,"1024*768");
//                    break;
//                case 3:
//                    LCD_ShowString(78,170,210,16,16,"1280*800");
//                    break;
//            }     
//        }
//		OSTimeDlyHMSM(0,0,0,100);  //��ʱ10ms
//	}
//}


