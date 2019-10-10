#include "TCA6424A.h"
#include "myiic.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途

u32 Channelbit;
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
u8 temp1=0,temp2=0;
//初始化TCA6424A
//返回值:0,初始化成功
//      1,初始化失败
u8 TCA6424A_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Initure;
		
		__HAL_RCC_GPIOB_CLK_ENABLE();       //使能GPIOH时钟
		//PH9
    GPIO_Initure.Pin=GPIO_PIN_15;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;              //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //快速            

    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	RST_TCA6424A1=1;
	//复位TCA6424A
//	RST_TCA6424A1=1;
//	RST_TCA6424A2=1;
//	delay_us(6);
//	RST_TCA6424A1=0;
//	RST_TCA6424A2=0;
//	delay_us(10);
//	RST_TCA6424A1=1;
//	RST_TCA6424A2=1;
	delay_us(10);
	//IIC_Init();  //初始化IIC
	IIC_Start();
	IIC_Send_Byte(TCA6424A_ADDRESS1<<1);
	temp1=IIC_Wait_Ack();//等待应答，通过判断是否有ACK应答来判断TCA6424的状态
	//IIC_Send_Byte(TCA6424A_ADDRESS2<<1);
	//temp2=IIC_Wait_Ack();//等待应答
	IIC_Stop();
	
	TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_POLARITY_REG0,0X00);
	TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_CONFIG_REG0,0X00);
	//while(1)
	{
//	IIC_Start();
//	IIC_Send_Byte(TCA6424A_ADDRESS1<<1);
//	temp1=IIC_Wait_Ack();//等待应答，通过判断是否有ACK应答来判断TCA6424的状态
//	//IIC_Send_Byte(TCA6424A_ADDRESS2<<1);
//	//temp2=IIC_Wait_Ack();//等待应答
//	IIC_Stop();
	TCA6424A_W(TCA6424A_ADDRESS1,TCA6424A_OUTPUT_REG0,0X200);//760  0X2f8
	//TCA6424A_W(TCA6424A_ADDRESS2,TCA6424A_OUTPUT_REG0,0X899438);//初始输出设为零
	}
	//temp1=TCA6424A_R(1);
//	temp1=TCA6424A_R(2);
	
	if(temp1)return 1;				//不正常
	else return 0;						//正常
} 
/*
void TCA6424A_Init(void)
	{
		GPIO_InitTypeDef GPIO_Initure;
		
		__HAL_RCC_GPIOB_CLK_ENABLE();       //使能GPIOH时钟
		//PF2,3
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;              //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //快速            
    //GPIO_Initure.Alternate=GPIO_AF5_SPI5;           //复用为SPI5
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	//复位TCA6424A
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

//IIC写一个字节  
//reg:寄存器地址
//data:要写入的数据
//返回值:0,正常
//    其他,错误代码

//向TCA6424A设置各个IO口
//DataToWrite:要写入的数据
//void TCA6424A_Write(u8 Reg,u32 DataToWrite)
//{	
//	//u8 i,j,k;
//	
//    IIC_Start();  
//    IIC_Send_Byte(TCA6424A_ADDRESS1|0X00);   //发送器件地址0X40,写数据 	 
//	IIC_Wait_Ack();  
//	IIC_Send_Byte(TCA6424A_OUTPUT_REG0);	//发送命令
//	IIC_Wait_Ack();
//	IIC_Send_Byte(((u8)DataToWrite));    	 	//发送字节							   
//	IIC_Wait_Ack();
//	IIC_Send_Byte(((u8)DataToWrite>>8));    	 	//发送字节	
//	IIC_Wait_Ack();
//	IIC_Send_Byte(((u8)DataToWrite>>16));    	 	//发送字节	
//    IIC_Stop();							//产生一个停止条件 
//	//DELAY_MS(10);	 
//}
/*对器件进行写操作*/


void TCA6424A_W(u8 ADDR,u8 reg,u32 DataToWrite)
{
	u8 res1,res2,res3;
	
	res1=(u8)DataToWrite;
	res2=(u8)(DataToWrite>>8);
	res3=(u8)(DataToWrite>>16);
	
	IIC_Start();  
    IIC_Send_Byte(ADDR<<1|0X00);   //发送器件地址0X40,写数据 	 
	IIC_Wait_Ack();  
	IIC_Send_Byte(reg);	//发送命令
	IIC_Wait_Ack();
	IIC_Send_Byte(res1);    	 	//发送字节							   
	IIC_Wait_Ack();
	IIC_Send_Byte(res2);    	 	//发送字节	
	IIC_Wait_Ack();
	IIC_Send_Byte(res3);    	 	//发送字节	
	IIC_Wait_Ack();
    IIC_Stop();							//产生一个停止条件 

}
void TCA6424_W_BIT(u8 reg,u8 res)
{
	IIC_Start();  
    IIC_Send_Byte(TCA6424A_ADDRESS1<<1|0X00);   //发送器件地址0X40,写数据 	 
	IIC_Wait_Ack();  
	IIC_Send_Byte(reg);	//发送命令
	IIC_Wait_Ack();
	IIC_Send_Byte(res);    	 	//发送字节	
	IIC_Wait_Ack();
    IIC_Stop();							//产生一个停止条件 

}

u8 TCA6424A_R(u8 res)
{
	u8 tem1,tem2,tem3;
	if(res==1)
	{
		IIC_Start();
	IIC_Send_Byte(TCA6424A_ADDRESS1<<1|0X00);//发送奇案地址0x23,
	IIC_Wait_Ack();	  
	IIC_Send_Byte(0x88);	//发送命令
	IIC_Wait_Ack();
	IIC_Send_Byte(TCA6424A_ADDRESS1<<1|0X01);//发送奇案地址0x23,读数据
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
//	IIC_Send_Byte(TCA6424A_ADDRESS2<<1|0X00);//发送奇案地址0x23,
//	IIC_Wait_Ack();	  
//	IIC_Send_Byte(TCA6424A_OUTPUT_REG0);	//发送命令
//	IIC_Wait_Ack();
//	IIC_Send_Byte(TCA6424A_ADDRESS2<<1|0X01);//发送奇案地址0x23,读数据
//	IIC_Wait_Ack();
//	tem1=IIC_Read_Byte(1);
//	IIC_NAck();
//	IIC_Stop();
//	}
	return tem1;
}
//设置TCA6424A某个IO的高低电平w
//bit:要设置的IO编号，0~24
//sta:IO的状态：0或1
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
