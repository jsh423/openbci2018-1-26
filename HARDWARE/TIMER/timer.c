#include "timer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//定时器中断驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 
TIM_HandleTypeDef TIM4_Handler;      //定时器句柄 
TIM_HandleTypeDef TIM5_Handler;      //定时器句柄 
extern u32 lwip_localtime;	         //lwip本地时间计数器,单位:ms
extern u16 led_pwmtime;			//闪光刺激计时器 单位s
extern u16 led_shtime;		//闪光刺激闪烁计时器 单位
TIM_HandleTypeDef TIM2_Handler;         //定时器3PWM句柄 10ms
TIM_OC_InitTypeDef TIM2_CH1Handler;	    //定时器3通道4句柄
//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM3_Init(u16 arr,u16 psc)
{  
    TIM3_Handler.Instance=TIM3;                          //通用定时器3
    TIM3_Handler.Init.Prescaler=psc;                     //分频
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM3_Handler.Init.Period=arr;                        //自动装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //使能定时器3和定时器3中断   
}

void TIM4_Init(u16 arr,u16 psc)
{  
    TIM4_Handler.Instance=TIM4;                          //通用定时器3
    TIM4_Handler.Init.Prescaler=psc;                     //分频
    TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM4_Handler.Init.Period=arr;                        //自动装载值
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM4_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM4_Handler); //使能定时器3和定时器3中断   
}

//void TIM5_Init(u16 arr,u16 psc)
//{  
//    TIM5_Handler.Instance=TIM5;                          //通用定时器3
//    TIM5_Handler.Init.Prescaler=psc;                     //分频
//    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
//    TIM5_Handler.Init.Period=arr;                        //自动装载值
//    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
//    HAL_TIM_Base_Init(&TIM5_Handler);
//    
//    HAL_TIM_Base_Start_IT(&TIM5_Handler); //使能定时器3和定时器3中断   
//}
//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断   
	}
	if(htim->Instance==TIM4)
	{
		__HAL_RCC_TIM4_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM4_IRQn,1,5);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM4_IRQn);          //开启ITM3中断   
	}
//	if(htim->Instance==TIM5)
//	{
//		__HAL_RCC_TIM5_CLK_ENABLE();            //使能TIM3时钟
//		HAL_NVIC_SetPriority(TIM5_IRQn,1,6);    //设置中断优先级，抢占优先级1，子优先级3
//		HAL_NVIC_EnableIRQ(TIM5_IRQn);          //开启ITM3中断   
//	}
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}
//定时器4中断服务函数
void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM4_Handler);
}

////定时器4中断服务函数
//void TIM5_IRQHandler(void)
//{
//    HAL_TIM_IRQHandler(&TIM5_Handler);
//}
//定时器3中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM3_Handler))
    {
        lwip_localtime +=10; //加10
		led_shtime++;
    }
	if(htim==(&TIM4_Handler))
    {
       led_pwmtime++;
    }
//	if(htim==(&TIM5_Handler))
//    {
//       //led_pwmtime++;
//    }
}
//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM2_PWM_Init(u16 arr,u16 psc)
{ 
    TIM2_Handler.Instance=TIM2;            //定时器3
    TIM2_Handler.Init.Prescaler=psc;       //定时器分频
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//向上计数模式
    TIM2_Handler.Init.Period=arr;          //自动重装载值
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM2_Handler);       //初始化PWM
    
    TIM2_CH1Handler.OCMode=TIM_OCMODE_PWM1; //模式选择PWM1
    TIM2_CH1Handler.Pulse=arr;            //设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
    TIM2_CH1Handler.OCPolarity=TIM_OCPOLARITY_LOW; //输出比较极性为低 
    HAL_TIM_PWM_ConfigChannel(&TIM2_Handler,&TIM2_CH1Handler,TIM_CHANNEL_1);//配置TIM3通道4
	
    HAL_TIM_PWM_Start(&TIM2_Handler,TIM_CHANNEL_1);//开启PWM通道4
}


//定时器底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_PWM_Init()调用
//htim:定时器句柄
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM2_CLK_ENABLE();			//使能定时器3
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_0;           	//PB1
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	GPIO_Initure.Alternate= GPIO_AF1_TIM2;	//PB1复用为TIM3_CH4
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}


//设置TIM2通道1的占空比和频率
//compare:比较值
void TIM_SetTIM2(u32 compare)
{
	TIM2->CCR1=1000-compare; 
	//TIM2_Handler.Init.Period=fre;          //自动重装载值
	//TIM2_CH1Handler.Pulse=compare;
}
