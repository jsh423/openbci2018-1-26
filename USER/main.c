#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sdram.h"
#include "lan8720.h"
#include "timer.h"
#include "usmart.h"
#include "malloc.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "tcp_server_demo.h"
#include "ads1299.h"
#include "ad5933.h"
#include "TCA6424A.h"
#include "adg731.h"
#include "myiic.h"
/************************************************

************************************************/



int main(void)
{   
    
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz   
    HAL_Init();                     //初始化HAL库
    delay_init(180);                //初始化延时函数
	delay_ms(1000);			//延时1s
    uart_init(115200);              //初始化USART
    usmart_dev.init(90); 		    //初始化USMART	

	IIC_Init();
	Adg731_Init();
	ADS1299_Init();
	TCA6424A_Init();
	//AD5933_Init();//增加会加大信号噪声
    my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMCCM);		    //初始化CCM内存池
  
    TIM3_Init(1000-1,900-1);        //定时器3初始化，定时器时钟为90M，分频系数为900-1，
                                    //所以定时器3的频率为90M/900=100K，自动重装载为1000-1，那么定时器周期就是10ms
	while(lwip_comm_init())         //lwip初始化
	{
		delay_ms(500);
	}
#if LWIP_DHCP   //使用DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
	{  
		lwip_periodic_handle();	//LWIP内核需要定时处理的函数
	}
#endif
	
	delay_ms(1000);			//延时1s
	while(1)
	{
		tcp_server_test();		//当断开连接后,调用tcp_server_test()函数
	}
}
