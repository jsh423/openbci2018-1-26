#include "timer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//��ʱ���ж���������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

TIM_HandleTypeDef TIM3_Handler;      //��ʱ����� 
TIM_HandleTypeDef TIM4_Handler;      //��ʱ����� 
TIM_HandleTypeDef TIM5_Handler;      //��ʱ����� 
extern u32 lwip_localtime;	         //lwip����ʱ�������,��λ:ms
extern u16 led_pwmtime;			//����̼���ʱ�� ��λs
extern u16 led_shtime;		//����̼���˸��ʱ�� ��λ
TIM_HandleTypeDef TIM2_Handler;         //��ʱ��3PWM��� 10ms
TIM_OC_InitTypeDef TIM2_CH1Handler;	    //��ʱ��3ͨ��4���
//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!(��ʱ��3����APB1�ϣ�ʱ��ΪHCLK/2)
void TIM3_Init(u16 arr,u16 psc)
{  
    TIM3_Handler.Instance=TIM3;                          //ͨ�ö�ʱ��3
    TIM3_Handler.Init.Prescaler=psc;                     //��Ƶ
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM3_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�ж�   
}

void TIM4_Init(u16 arr,u16 psc)
{  
    TIM4_Handler.Instance=TIM4;                          //ͨ�ö�ʱ��3
    TIM4_Handler.Init.Prescaler=psc;                     //��Ƶ
    TIM4_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM4_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM4_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&TIM4_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM4_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�ж�   
}

//void TIM5_Init(u16 arr,u16 psc)
//{  
//    TIM5_Handler.Instance=TIM5;                          //ͨ�ö�ʱ��3
//    TIM5_Handler.Init.Prescaler=psc;                     //��Ƶ
//    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
//    TIM5_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
//    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
//    HAL_TIM_Base_Init(&TIM5_Handler);
//    
//    HAL_TIM_Base_Start_IT(&TIM5_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�ж�   
//}
//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻHAL_TIM_Base_Init()��������
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM3�ж�   
	}
	if(htim->Instance==TIM4)
	{
		__HAL_RCC_TIM4_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM4_IRQn,1,5);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM4_IRQn);          //����ITM3�ж�   
	}
//	if(htim->Instance==TIM5)
//	{
//		__HAL_RCC_TIM5_CLK_ENABLE();            //ʹ��TIM3ʱ��
//		HAL_NVIC_SetPriority(TIM5_IRQn,1,6);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
//		HAL_NVIC_EnableIRQ(TIM5_IRQn);          //����ITM3�ж�   
//	}
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}
//��ʱ��4�жϷ�����
void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM4_Handler);
}

////��ʱ��4�жϷ�����
//void TIM5_IRQHandler(void)
//{
//    HAL_TIM_IRQHandler(&TIM5_Handler);
//}
//��ʱ��3�жϷ���������
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM3_Handler))
    {
        lwip_localtime +=10; //��10
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
//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM2_PWM_Init(u16 arr,u16 psc)
{ 
    TIM2_Handler.Instance=TIM2;            //��ʱ��3
    TIM2_Handler.Init.Prescaler=psc;       //��ʱ����Ƶ
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//���ϼ���ģʽ
    TIM2_Handler.Init.Period=arr;          //�Զ���װ��ֵ
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM2_Handler);       //��ʼ��PWM
    
    TIM2_CH1Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM1
    TIM2_CH1Handler.Pulse=arr;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
    TIM2_CH1Handler.OCPolarity=TIM_OCPOLARITY_LOW; //����Ƚϼ���Ϊ�� 
    HAL_TIM_PWM_ConfigChannel(&TIM2_Handler,&TIM2_CH1Handler,TIM_CHANNEL_1);//����TIM3ͨ��4
	
    HAL_TIM_PWM_Start(&TIM2_Handler,TIM_CHANNEL_1);//����PWMͨ��4
}


//��ʱ���ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_PWM_Init()����
//htim:��ʱ�����
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_TIM2_CLK_ENABLE();			//ʹ�ܶ�ʱ��3
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOBʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_0;           	//PB1
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//�����������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	GPIO_Initure.Alternate= GPIO_AF1_TIM2;	//PB1����ΪTIM3_CH4
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}


//����TIM2ͨ��1��ռ�ձȺ�Ƶ��
//compare:�Ƚ�ֵ
void TIM_SetTIM2(u32 compare)
{
	TIM2->CCR1=1000-compare; 
	//TIM2_Handler.Init.Period=fre;          //�Զ���װ��ֵ
	//TIM2_CH1Handler.Pulse=compare;
}
