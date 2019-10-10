#include "TCA6424A.h"
#include "myiic.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;

u32 Channelbit;
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
u8 temp1=0,temp2=0;
//��ʼ��TCA6424A
//����ֵ:0,��ʼ���ɹ�
//      1,��ʼ��ʧ��
u8 TCA6424A_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Initure;
		
		__HAL_RCC_GPIOB_CLK_ENABLE();       //ʹ��GPIOHʱ��
		//PH9
    GPIO_Initure.Pin=GPIO_PIN_15;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;              //�����������
    GPIO_Initure.Pull=GPIO_PULLUP;                  //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //����            

    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	RST_TCA6424A1=1;
	//��λTCA6424A
//	RST_TCA6424A1=1;
//	RST_TCA6424A2=1;
//	delay_us(6);
//	RST_TCA6424A1=0;
//	RST_TCA6424A2=0;
//	delay_us(10);
//	RST_TCA6424A1=1;
//	RST_TCA6424A2=1;
	delay_us(10);
	//IIC_Init();  //��ʼ��IIC
	IIC_Start();
	IIC_Send_Byte(TCA6424A_ADDRESS1<<1);
	temp1=IIC_Wait_Ack();//�ȴ�Ӧ��ͨ���ж��Ƿ���ACKӦ�����ж�TCA6424��״̬
	//IIC_Send_Byte(TCA6424A_ADDRESS2<<1);
	//temp2=IIC_Wait_Ack();//�ȴ�Ӧ��
	IIC_Stop();
	
	TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_POLARITY_REG0,0X00);
	TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_CONFIG_REG0,0X00);
	//while(1)
	{
//	IIC_Start();
//	IIC_Send_Byte(TCA6424A_ADDRESS1<<1);
//	temp1=IIC_Wait_Ack();//�ȴ�Ӧ��ͨ���ж��Ƿ���ACKӦ�����ж�TCA6424��״̬
//	//IIC_Send_Byte(TCA6424A_ADDRESS2<<1);
//	//temp2=IIC_Wait_Ack();//�ȴ�Ӧ��
//	IIC_Stop();
	TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,0X200);//760  0X2f8
	//TCA6424A_W(TCA6424A_ADDRESS2,TCA6424A_OUTPUT_REG0,0X899438);//��ʼ�����Ϊ��
	}
	//temp1=TCA6424A_R(1);
//	temp1=TCA6424A_R(2);
	
	if(temp1)return 1;				//������
	else return 0;						//����
} 
/*
void TCA6424A_Init(void)
	{
		GPIO_InitTypeDef GPIO_Initure;
		
		__HAL_RCC_GPIOB_CLK_ENABLE();       //ʹ��GPIOHʱ��
		//PF2,3
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;              //�����������
    GPIO_Initure.Pull=GPIO_PULLUP;                  //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //����            
    //GPIO_Initure.Alternate=GPIO_AF5_SPI5;           //����ΪSPI5
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	//��λTCA6424A
	RST_TCA6424A1=1;
	RST_TCA6424A2=1;
	//Delay_us(6);
	RST_TCA6424A1=0;
	RST_TCA6424A2=0;
	Delay_us(10);
	RST_TCA6424A1=1;
	RST_TCA6424A2=1;
		
	//Regs->Output.all = 0x00FFFFFF;
	//Regs->PolarityInversion.all = 0x00000000;
	//Regs->Config.all = 0x00000000;
}*/

//IICдһ���ֽ�  
//reg:�Ĵ�����ַ
//data:Ҫд�������
//����ֵ:0,����
//    ����,�������

//��TCA6424A���ø���IO��
//DataToWrite:Ҫд�������
//void TCA6424A_Write(u8 Reg,u32 DataToWrite)
//{	
//	//u8 i,j,k;
//	
//    IIC_Start();  
//    IIC_Send_Byte(TCA6424A_ADDRESS1|0X00);   //����������ַ0X40,д���� 	 
//	IIC_Wait_Ack();  
//	IIC_Send_Byte(TCA6424A_OUTPUT_REG0);	//��������
//	IIC_Wait_Ack();
//	IIC_Send_Byte(((u8)DataToWrite));    	 	//�����ֽ�							   
//	IIC_Wait_Ack();
//	IIC_Send_Byte(((u8)DataToWrite>>8));    	 	//�����ֽ�	
//	IIC_Wait_Ack();
//	IIC_Send_Byte(((u8)DataToWrite>>16));    	 	//�����ֽ�	
//    IIC_Stop();							//����һ��ֹͣ���� 
//	//DELAY_MS(10);	 
//}
/*����������д����*/


void TCA6424A_W(u8 ADDR,u8 reg,u32 DataToWrite)
{
	u8 res1,res2,res3;
	
	res1=(u8)DataToWrite;
	res2=(u8)(DataToWrite>>8);
	res3=(u8)(DataToWrite>>16);
	
	IIC_Start();  
    IIC_Send_Byte(ADDR<<1|0X00);   //����������ַ0X40,д���� 	 
	IIC_Wait_Ack();  
	IIC_Send_Byte(reg);	//��������
	IIC_Wait_Ack();
	IIC_Send_Byte(res1);    	 	//�����ֽ�							   
	IIC_Wait_Ack();
	IIC_Send_Byte(res2);    	 	//�����ֽ�	
	IIC_Wait_Ack();
	IIC_Send_Byte(res3);    	 	//�����ֽ�	
	IIC_Wait_Ack();
    IIC_Stop();							//����һ��ֹͣ���� 

}
void TCA6424_W_BIT(u8 reg,u8 res)
{
	IIC_Start();  
    IIC_Send_Byte(TCA6424A_ADDRESS1<<1|0X00);   //����������ַ0X40,д���� 	 
	IIC_Wait_Ack();  
	IIC_Send_Byte(reg);	//��������
	IIC_Wait_Ack();
	IIC_Send_Byte(res);    	 	//�����ֽ�	
	IIC_Wait_Ack();
    IIC_Stop();							//����һ��ֹͣ���� 

}

u8 TCA6424A_R(u8 res)
{
	u8 tem1,tem2,tem3;
	if(res==1)
	{
		IIC_Start();
	IIC_Send_Byte(TCA6424A_ADDRESS1<<1|0X00);//�����永��ַ0x23,
	IIC_Wait_Ack();	  
	IIC_Send_Byte(0x88);	//��������
	IIC_Wait_Ack();
	IIC_Send_Byte(TCA6424A_ADDRESS1<<1|0X01);//�����永��ַ0x23,������
	IIC_Wait_Ack();
	tem1=IIC_Read_Byte(1);
	IIC_Wait_Ack();
	tem1=IIC_Read_Byte(1);	
	IIC_Wait_Ack();
	tem1=IIC_Read_Byte(0);		
	//IIC_NAck();	
	IIC_Stop();
	}	
//	else if(res==2)
//	{
//		IIC_Start();
//	IIC_Send_Byte(TCA6424A_ADDRESS2<<1|0X00);//�����永��ַ0x23,
//	IIC_Wait_Ack();	  
//	IIC_Send_Byte(TCA6424A_OUTPUT_REG0);	//��������
//	IIC_Wait_Ack();
//	IIC_Send_Byte(TCA6424A_ADDRESS2<<1|0X01);//�����永��ַ0x23,������
//	IIC_Wait_Ack();
//	tem1=IIC_Read_Byte(1);
//	IIC_NAck();
//	IIC_Stop();
//	}
	return tem1;
}
//����TCA6424Aĳ��IO�ĸߵ͵�ƽw
//bit:Ҫ���õ�IO��ţ�0~24
//sta:IO��״̬��0��1
void TCA6424A_WriteBit(u8 bit,u8 sta)
{
	//if(index==1) 
	u32 temp;
	//TCA6424A_W(TCA6424A_OUTPUT_REG0,0x0f);
		//temp=TCA6424A_R();
	//return temp;
	temp=1<<bit;
	TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,temp);
	//else  TCA6424_Write(TCA6424A_ADDRESS2,TCA6424A_OUTPUT_REG0,0xff);
	}
