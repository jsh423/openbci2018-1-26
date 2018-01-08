#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sdram.h"
#include "lan8720.h"
#include "timer.h"
#include "pcf8574.h"
#include "usmart.h"
#include "malloc.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "tcp_server_demo.h"
#include "ads1299.h"
#include "ad5933.h"
/************************************************

************************************************/

extern u8 tcp_server_flag;	 //TCP Server ����ȫ��״̬��Ǳ���
u32 value;


int main(void)
{   
    //u8 key;
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��,180Mhz   
    HAL_Init();                     //��ʼ��HAL��
    delay_init(180);                //��ʼ����ʱ����
    uart_init(115200);              //��ʼ��USART
    usmart_dev.init(90); 		    //��ʼ��USMART	
    SDRAM_Init();                   //��ʼ��SDRAM
	IIC_Init();
	ADS1299_Init();
    my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
	my_mem_init(SRAMCCM);		    //��ʼ��CCM�ڴ��
  
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
	///while(AD5933_Init())
	while(1)
	{
		delay_ms(50);
		value=wendu();
		AD5933_Start();
		value=AD5933_RES();
		//delay_ms(50);
	}
	//AD5933_Start();
//value=AD5933_RES();
	delay_ms(1000);			//��ʱ1s
	while(1)
	{
		tcp_server_test();		//���Ͽ����Ӻ�,����tcp_server_test()����
	}
}
