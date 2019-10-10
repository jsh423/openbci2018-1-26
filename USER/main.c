#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
//#include "lcd.h"
//#include "sdram.h"
#include "lan8720.h"
#include "timer.h"
#include "usmart.h"
#include "malloc.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "tcp_server_demo.h"
//#include "udp_demo.h"
#include "ads1299.h"
//#include "ad5933.h"
#include "TCA6424A.h"
//#include "adg731.h"
#include "myiic.h"
#include "iwdg.h"

/************************************************

************************************************/



int main(void)
{   
    
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��,180Mhz   
    HAL_Init();                     //��ʼ��HAL��
    delay_init(180);                //��ʼ����ʱ����
	//delay_ms(1000);			//��ʱ1s
   // uart_init(115200);              //��ʼ��USART
   // usmart_dev.init(90); 		    //��ʼ��USMART	
	LED_Init();
	TIM4_Init(10000-1,9000-1);        //��ʱ��3��ʼ������ʱ��ʱ��Ϊ90M����Ƶϵ��Ϊ900-1��
									//���Զ�ʱ��3��Ƶ��Ϊ90M/9000=10K���Զ���װ��Ϊ10000-1����ô��ʱ�����ھ���1s	
	TIM2_PWM_Init(1000-1,90-1);      //90M/90=10k�ļ���Ƶ�ʣ��Զ���װ��Ϊ1000����ôPWMƵ��Ϊ1M/1000=1kHZ
	//TIM5_Init(1000-1,900-1);			//90M/900=100k�ļ���Ƶ�ʣ��Զ���װ��Ϊ1000����ôPWMƵ��Ϊ100K/1000=100HZ,��ʱ��������10ms
	TIM_SetTIM2(000);
	IIC_Init();
	//Adg731_Init();
	ADS1299_Init();
	TCA6424A_Init();
	//AD5933_Init();//���ӻ�Ӵ��ź�����
    my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
	//my_mem_init(SRAMCCM);		    //��ʼ��CCM�ڴ��
	
    TIM3_Init(1000-1,900-1);        //��ʱ��3��ʼ������ʱ��ʱ��Ϊ90M����Ƶϵ��Ϊ900-1��
   	                               //���Զ�ʱ��3��Ƶ��Ϊ90M/900=100K���Զ���װ��Ϊ1000-1����ô��ʱ�����ھ���10ms
	while(lwip_comm_init())         //lwip��ʼ��
	{
		delay_ms(500);
	}
#if LWIP_DHCP   //ʹ��DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
	{  
		lwip_periodic_handle();	//LWIP�ں���Ҫ��ʱ����ĺ���
	}
#endif
	hardware_init();
	delay_ms(100);			//��ʱ1s
	IWDG_Init(IWDG_PRESCALER_64,500);  	//��Ƶ��Ϊ64,����ֵΪ500,���ʱ��Ϊ1s
	while(1)
	{
		 //IWDG_Feed();    //ι��
		//tcp_server_test();		//���Ͽ����Ӻ�,����tcp_server_test()����
		udp_demo_test();
	}
}
