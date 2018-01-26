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

////KEY任务
//#define KEY_TASK_PRIO 		8
////任务堆栈大小
//#define KEY_STK_SIZE		64		
////任务堆栈
//OS_STK KEY_TASK_STK[KEY_STK_SIZE];
////任务函数
//void key_task(void *pdata);  

////在LCD上显示地址信息任务
////任务优先级
//#define DISPLAY_TASK_PRIO	9
////任务堆栈大小
//#define DISPLAY_STK_SIZE	128
////任务堆栈
//OS_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
////任务函数
//void display_task(void *pdata);


//ADS1299数据接受任务
//任务优先级
#define ADS1299_TASK_PRIO		2
//任务堆栈大小
#define ADS1299_STK_SIZE		500
//任务堆栈
OS_STK	ADS1299_TASK_STK[ADS1299_STK_SIZE];
//任务函数
void ads1299_task(void *pdata);  


//START任务
//任务优先级
#define START_TASK_PRIO		11
//任务堆栈大小
#define START_STK_SIZE		128
//任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata); 





int main(void)
{   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz   
    HAL_Init();                     //初始化HAL库
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
    usmart_dev.init(90); 		    //初始化USMART	
    LED_Init();                     //初始化LED 
   	IIC_Init();
	Adg731_Init();
	ADS1299_Init();
	TCA6424A_Init();   
	my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMCCM);		    //初始化CCM内存池
	OSInit(); 					    //UCOS初始化
	while(lwip_comm_init()) 	    //lwip初始化
	{
		
		delay_ms(500);
	}
    openbci_init();
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart(); //开启UCOS
}

//start任务
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//初始化统计任务
	OS_ENTER_CRITICAL();  	//关中断
#if LWIP_DHCP
	lwip_comm_dhcp_creat(); //创建DHCP任务
#endif
	
	OSTaskCreate(ads1299_task,(void*)0,(OS_STK*)&ADS1299_TASK_STK[ADS1299_STK_SIZE-1],ADS1299_TASK_PRIO);//创建ADS1299采集任务
    //OSTaskCreate(key_task,(void*)0,(OS_STK*)&KEY_TASK_STK[KEY_STK_SIZE-1],KEY_TASK_PRIO);//按键任务
	//OSTaskCreate(display_task,(void*)0,(OS_STK*)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE-1],DISPLAY_TASK_PRIO); //显示任务
	OSTaskSuspend(OS_PRIO_SELF); //挂起start_task任务
	OS_EXIT_CRITICAL();  		//开中断
}

////显示地址等信息
//void display_task(void *pdata)
//{
//	while(1)
//	{ 
//#if LWIP_DHCP									//当开启DHCP的时候
//		if(lwipdev.dhcpstatus != 0) 			//开启DHCP
//		{
//			show_inform(lwipdev.dhcpstatus );	//显示地址信息
//			OSTaskSuspend(OS_PRIO_SELF); 		//显示完地址信息后挂起自身任务
//		}
//#else
//		show_inform(0); 						//显示静态地址
//		OSTaskSuspend(OS_PRIO_SELF); 		 	//显示完地址信息后挂起自身任务
//#endif //LWIP_DHCP
//		OSTimeDlyHMSM(0,0,0,500);
//	}
//}

////key任务
//void key_task(void *pdata)
//{
//    u8 resolut=0;
//	u8 key=0; 
//	while(1)
//	{
//        key=KEY_Scan(0);
//        if(key==WKUP_PRES)//对焦
//        {
//           // OV5640_Focus_Single();
//        }
//        if(key==KEY0_PRES)//调整分辨率
//        {
//            resolut++;
//            if(resolut==4) resolut=0;
//           // netcam_resolu_set(resolut);//设置分辨率
//            switch(resolut)            //显示所设置的分辨率
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
//		OSTimeDlyHMSM(0,0,0,100);  //延时10ms
//	}
//}


