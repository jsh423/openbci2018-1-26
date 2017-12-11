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
/************************************************

************************************************/

extern u8 tcp_server_flag;	 //TCP Server 测试全局状态标记变量

//加载UI
//mode:
//bit0:0,不加载;1,加载前半部分UI
//bit1:0,不加载;1,加载后半部分UI
void lwip_test_ui(u8 mode)
{
	u8 speed;
	u8 buf[30]; 
	POINT_COLOR=RED;
	if(mode&1<<0)
	{
		LCD_Fill(30,30,lcddev.width,110,WHITE);	//清除显示
		LCD_ShowString(30,30,200,16,16,"Apollo STM32F4/F7");
		LCD_ShowString(30,50,200,16,16,"RAW TCP Server");
		LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
		LCD_ShowString(30,90,200,16,16,"2016/2/29"); 	
	}
	if(mode&1<<1)
	{
		LCD_Fill(30,110,lcddev.width,lcddev.height,WHITE);	//清除显示
		LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
		if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印动态IP地址
		else sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印静态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		speed=LAN8720_Get_Speed();//得到网速
		if(speed&1<<1)LCD_ShowString(30,150,200,16,16,"Ethernet Speed:100M");
		else LCD_ShowString(30,150,200,16,16,"Ethernet Speed:10M");
		LCD_ShowString(30,170,200,16,16,"PORT: 8088");
		LCD_ShowString(30,170,200,16,16,"KEY0: Send data");
	}
}

int main(void)
{   
    u8 key;
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz   
    HAL_Init();                     //初始化HAL库
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
    usmart_dev.init(90); 		    //初始化USMART	
   // LED_Init();                     //初始化LED 
   // KEY_Init();                     //初始化按键
    SDRAM_Init();                   //初始化SDRAM
   // LCD_Init();                     //初始化LCD
   // PCF8574_Init();                 //初始化PCF8574
    my_mem_init(SRAMIN);		    //初始化内部内存池
	//my_mem_init(SRAMEX);		    //初始化外部内存池
	my_mem_init(SRAMCCM);		    //初始化CCM内存池
    POINT_COLOR = RED; 		
    //lwip_test_ui(1);                //加载前半部分UI
    TIM3_Init(1000-1,900-1);        //定时器3初始化，定时器时钟为90M，分频系数为900-1，
                                    //所以定时器3的频率为90M/900=100K，自动重装载为1000-1，那么定时器周期就是10ms
	while(lwip_comm_init())         //lwip初始化
	{
		//LCD_ShowString(30,110,200,20,16,"LWIP Init Falied! ");
		delay_ms(500);
		//LCD_ShowString(30,110,200,16,16,"Retrying...       ");
        delay_ms(500);
	}
	//LCD_ShowString(30,110,200,20,16,"LWIP Init Success!");
 	//LCD_ShowString(30,130,200,16,16,"DHCP IP configing...");  //等待DHCP获取 
#if LWIP_DHCP   //使用DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
	{  
		lwip_periodic_handle();	//LWIP内核需要定时处理的函数
	}
#endif
    //lwip_test_ui(2);		//加载后半部分UI 
	delay_ms(1000);			//延时1s
    //tcp_server_test();  	//TCP Server模式
	while(1)
	{
        lwip_periodic_handle();	//LWIP内核需要定时处理的函数
	 
		tcp_server_test();		//当断开连接后,调用tcp_server_test()函数

		delay_ms(10);
	}
}
