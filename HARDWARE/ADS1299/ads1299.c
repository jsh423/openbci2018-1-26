#include "ads1299.h"
#include "spi.h"
#include "delay.h"
#include "malloc.h"
#include "stm32f4xx_hal_gpio.h"
//#include  "tcp_server_demo.h"
#include "openbci.h"
#include  <string.h>
#include "led.h"
#include "math.h"
//#include "arm_math.h"
//#include "dma.h"
//////////////////////////////////////////////////////////////////////////////////	 
//
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
u8 check=0;
u8 TxData0[28]={0x12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //发送地址
//u8 TxData1[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 RxData[4]; //发送地址
u8 sps=0x05;
u8 parameter[10];
u8 Flag_adc;
u32 stat;
u8 index1=0;
u8 res3;
u8 adc_buf2[101];
 //adc_buf2[0]=0xaa;
int32_t Adcres[32];
//float32_t input[20];
//float32_t output[20];
//float32_t res[20+28];

void ADS1299_SDATAC(void)

{
	u8 i;
	for(i=0;i<4;i++)
	{
	ADS1299_CHANGE_CHANEL(i,0);
	//ADS1299_CS0=0;
	SPI2_ReadWriteByte(0X11);
	//ADS1299_CS0=1;
		ADS1299_CHANGE_CHANEL(i,1);
		delay_us(2);
	}
}
void ADS1299_RDATAC(void)
{
	u8 i;
	for(i=0;i<4;i++)
	{
	ADS1299_CHANGE_CHANEL(i,0);
	SPI2_ReadWriteByte(0X10);
	ADS1299_CHANGE_CHANEL(i,1);
	}
}
void ADS1299_Command(u8 Command)
{
	u8 i;
	for(i=0;i<4;i++)
	{
	ADS1299_CHANGE_CHANEL(i,0);
	SPI2_ReadWriteByte(Command);
	ADS1299_CHANGE_CHANEL(i,1);
	}
}
#if 1
void ADS1299_CHANGE_CHANEL(u8 n,u8 sw)
{
	switch(n)
	{
		case 0:
			ADS1299_CS0=sw;
			break;
		case 1:
			ADS1299_CS1=sw;
			break;
		case 2:
			ADS1299_CS2=sw;
			break;
		case 3:
			ADS1299_CS3=sw;
			break;
		default:
			break;
	}
}
#endif
//初始化ADS1299的IO口
void ADS1299_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOG时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOG时钟
	__HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOI时钟
	__HAL_RCC_GPIOD_CLK_ENABLE();			//开启GPIOI时钟

	GPIO_Initure.Pin=GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_12; //PG10,12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //初始化

	GPIO_Initure.Pin=GPIO_PIN_15; //PG10,12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);     //初始化
	//     
	//    GPIO_Initure.Pin=GPIO_PIN_11;           //PI11
	//    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
	//    HAL_GPIO_Init(GPIOI,&GPIO_Initure);     //初始化

	GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;            //PC6
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速         
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //初始化

	SPI2_Init();    		                //初始化SPI2  /
	//SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_16); //spi速度为2.8Mhz（24L01的最大SPI时钟为10Mhz,这里大一点没关系）  
	// NRF24L01_SPI_Init();                    //针对NRF的特点修改SPI的设置
	delay_ms(50);

	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_16); //spi速度为11.25Mhz（24L01的最大SPI时钟为10Mhz,这里大一点没关系） 
	ADS1299_START=0;
	ADS1299_CS0=1; //屏蔽第一片ADS1299,测试单芯片
	ADS1299_CS1=1;
	ADS1299_CS2=1;
	ADS1299_CS3=1;
	ADS1299_RST0=1;
	ADS1299_RST2=1;
	ADS1299_RST3=1;
	ADS1299_PWDN1=1;
	ADS1299_PWDN0=1;
	ADS1299_PWDN2=1;
	ADS1299_PWDN3=1;
	ADS1299_RST1=1;
	delay_ms(5);
	ADS1299_RST1=0;
	ADS1299_RST0=0;
	ADS1299_RST2=0;
	ADS1299_RST3=0;
	delay_ms(5);
	ADS1299_RST0=1;
	ADS1299_RST1=1;
	ADS1299_RST2=1;
	ADS1299_RST3=1;
	delay_ms(5); 
	//delay_ms(2);
	//ADS1299_SDATAC();//退出连续读数模式，以便进行寄存器的设置
	//ADS1299_WREG(0X03,0xe0);//开启内部基准
	parameter[0]=0xa0;//帧头
	parameter[1]=8;//帧长
	parameter[9]=0xc0;//帧尾
	parameter[2]=0x21;
	parameter[3]=0x00;//设备代码为00脑电
	parameter[4]=32;//导联数为32
	parameter[5]=1;		//放大倍数为24倍
	parameter[6]=sps;	//采样率：06为250，05为500sps,04为1Ksps,03为2Ksps
	parameter[7]=0;		//目前滤波放到上位机
	parameter[8]=0;
	
	delay_ms(20);
	 	ADS1299_SDATAC();//退出连续读数模式，以便进行寄存器的设置
	ADS1299_WREG(0X03,0xe0);//开启内部基准
	
	delay_ms(10);
	//ADS1299_RDATAC();
	//SPI2_ReadWriteByte(0x0);
	//SPI2_ReadWriteByte(_SDATAC);
	//delay_us(10);
	//ADS1299_CS0=1;
	ADS1299_WREG(0X01,0X94);	//设置多回读模式，通信速率250SPS 1k	 
	ADS1299_Check();	
//	ADS1299_WREG(0X05,0X00);//第一通道设置短路，测试系统噪声
//	//ADS1299_WREG(0X02,0XD0);//测试信号由内部产生
//	//ADS1299_WREG(0x05,0x05);
//	//ADS1299_WREG(0X02,0XD
////	ADS1299_WREG(0X05,0X0A);//第一通道设置为测试信号输入 此处测试BIASREF
////	ADS1299_WREG(0X03,0XF0);//开启内部基准
//	
//	
//	//ADS1299_START=1;
//	ADS1299_WREG(0X06,0x05);
//	ADS1299_WREG(0X07,0x00);
//	ADS1299_WREG(0X08,0X00);//第一通道设置普通输入
//	ADS1299_WREG(0X09,0x00);
//	ADS1299_WREG(0X0A,0x00);
//	ADS1299_WREG(0X0B,0x00);
//	ADS1299_WREG(0X0C,0x00);
//	//ADS1299_WREG(0X0D,0Xff);
//	//ADS1299_WREG(0X0E,0XFF);
//	//ADS1299_WREG(0X03,0xE0);//关闭阻抗测试
//	ADS1299_WREG(0X15,0X10);//SRB1闭合
//	//ADS1299_RDATAC();//连续模式
//	ADS1299_Command(0x12);//命令读取数据模式
//	delay_ms(5);
	ADS1299_IT();
}
#if 0
u32 buf1[3];
void ADS1299_RDATA(u8 *p)
{
	u8 i,j,inbyte,n=0;
	//u32 Adc_buf[8];
	//u32 stat;
	//stat=0;
	ADS1299_CS0=0;
	
	SPI2_ReadWriteByte(0x12);
	//delay_us(2);
	for(i=0;i<1;i++)
	{
	//inbyte=SPI2_ReadWriteByte(0x00);
	//stat=(stat<<8)| inbyte;	
	buf1[i++]=SPI2_ReadWriteByte(0X00);
		//Adcres[0]=buf1[i
	//
	}
	
	////Adcres[0]=(buf1[0]<<16)+(buf1[1]<<8)+buf1[2];
	for(i=0;i<8;i++)
	{
		for(j=0;j<3;j++)
		{
			p[n++]=SPI2_ReadWriteByte(0x00);
			//inbyte=SPI2_ReadWriteByte(0x00);
				//Adc_buf[i]=(Adc_buf[i]<<8)| inbyte;
			
		}
		//Adcres[i+1]=(p[n-2]<<16)+(p[n-1]<<8)+p[n];
		//Adcres[i]=(buf1[0]<<16)+(buf1[1]<<8)+buf1[2];
	}
	n=0;
	ADS1299_CS0=1;
}			
#endif			
	
void ADS1299_IT(void)
{
		  GPIO_InitTypeDef GPIO_Initure;
   // __HAL_RCC_GPIOG_CLK_ENABLE();			//开启GPIOG时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOI时钟
	
//	GPIO_Initure.Pin=GPIO_PIN_12;               //PC12
//	//GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //上升沿触发
//   // GPIO_Initure.Pull=GPIO_PULLDOWN;
//    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
//    GPIO_Initure.Pull=GPIO_PULLUP;
//    HAL_GPIO_Init(GPIOC,&GPIO_Initure);		//暂时定义PH13为数据
	
	GPIO_Initure.Pin=GPIO_PIN_4;               //PC13
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;
	//GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	 //中断线13-PC13
    HAL_NVIC_SetPriority(EXTI4_IRQn,1,1);   //抢占优先级为2，子优先级为1
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);         //使能中断线13 
	if(openvibeflag) ADS1299_START=1;
	else ADS1299_START=0;
	//ADS1299_Command(0x08);
	delay_ms(10);
	//ADS1299_RDATAC();
//	 //中断线12-PC12
//    HAL_NVIC_SetPriority(EXTI15_10_IRQn,2,1);   //抢占优先级为2，子优先级为3
//    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //使能中断线12
}
u8 p[18];
u8 ADS1299_PREGS(void)
{
	u8 i;
	ADS1299_CS1=0;
	SPI2_ReadWriteByte(0x00|0x20);
	SPI2_ReadWriteByte(0x12);
	for(i=0;i<3;i++)
{
	p[i]=SPI2_ReadWriteByte(0X00);
}
	ADS1299_CS1=1;
return p[0];
}
//检测ADS1299是否存在
//返回值:0，成功;1，失败	
u8 ADS1299_Check(void)
{
	//u8 buf[19];
//	u8 i;
	
	//ADS1299_START=1;
	//delay_ms(1000);
	
	
	//ADS1299_WREG(0X02,0Xc0);//设置测试信号由内部产生
	check=ADS1299_PREG(0X00);
	//check=ADS1299_PREGS();
	//delay_ms(10);
//	if(check==0x3e) 
	{
		
	//check=ADS1299_PREG(0X00);
	//ADS1299_Command(0x08);
	
	
	
	ADS1299_WREG(0X05,0X05);//第一通道设置短路，测试系统噪声
	//ADS1299_WREG(0X02,0XD0);//测试信号由内部产生
	//ADS1299_WREG(0x05,0x05);
	//ADS1299_WREG(0X02,0XD
//	ADS1299_WREG(0X05,0X0A);//第一通道设置为测试信号输入 此处测试BIASREF
//	ADS1299_WREG(0X03,0XF0);//开启内部基准
	
	
	//ADS1299_START=1;
	ADS1299_WREG(0X06,0x05);
	ADS1299_WREG(0X07,0x05);
	ADS1299_WREG(0X08,0X00);//第一通道设置普通输入
	ADS1299_WREG(0X09,0x00);
	ADS1299_WREG(0X0A,0x00);
	ADS1299_WREG(0X0B,0x00);
	ADS1299_WREG(0X0C,0x00);
	//ADS1299_WREG(0X0D,0Xff);
	//ADS1299_WREG(0X0E,0XFF);
	//ADS1299_WREG(0X03,0xE0);//关闭阻抗测试
	ADS1299_WREG(0X15,0X10);//SRB1闭合
	//ADS1299_RDATAC();//连续模式
	ADS1299_Command(0x12);//命令读取数据模式
	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi速度为11.25Mhz（24L01的最大SPI时钟为10Mhz,这里大一点没关系） 
		delay_ms(1);
	//ADS1299_CS0=0;
	return 0;
	}
		
	
	//else return 1;
	//else return 0;
//	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
//	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
//	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
//	if(i!=5)return 1;//检测24L01错误	
//	return 0;		 //检测到24L01
}	 

void Set_Sps(u8 i)
{
				ADS1299_START=0;//先将ADS1299数据采样给关掉
				ADS1299_SDATAC();//退出连续读数模式，以便进行寄存器的设置//进入设置模式
				ADS1299_WREG(0X01,(0X90|sps));	//设置多回读模式，通信速率250SPS 1k//设置采样率
				ADS1299_Command(0x12);//命令读取数据模式//退出设置模式
}

//读取寄存器数据
u8 ADS1299_PREG(u8 reg)
{
	  u8 Byte;	
   	ADS1299_CS3=0;                 //使能SPI传输
//delay_us(200);
  	SPI2_ReadWriteByte(0X00|0X20);//发送寄存器号
	
  	SPI2_ReadWriteByte(0X00);      //写入寄存器的值
	Byte=SPI2_ReadWriteByte(0);
//	status=SPI2_ReadWriteByte(0);
	//delay_us(2);
  	ADS1299_CS3=1;                 //禁止SPI传输	   
  	return(Byte);       		    //返回状态值
}
//写入ADS1299寄存器数据
void ADS1299_WREG(u8 address,u8 value)
{
	u8 i;
	for(i=0;i<4;i++)
	{
	ADS1299_CHANGE_CHANEL(i,0);
	//delay_us(3);
	SPI2_ReadWriteByte(0X40|address);
	SPI2_ReadWriteByte(00);
	SPI2_ReadWriteByte(value);
	//delay_us(2);
	ADS1299_CHANGE_CHANEL(i,1);
	}
}
//u8 regdata[18];

#if 0					  
void Recev_Data(void)
{
	u8 k;
	//u8 inbyte;
	//u32 stat1;
	
	//index1=0;
	adc_buf2[0]=0xa0;
	adc_buf2[27-1]=0xc0;
	if(res3<0xff) 
	{
		res3++;
	}
	else res3=0;
	adc_buf2[1]=res3;
	//tcp_server_sendbuf=buf3;
	for(k=0;k<1;k++)
	{
		ADS1299_CHANGE_CHANEL(k,0);
		//HAL_SPI_Receive(&SPI2_Handler,&adc_buf2[k*24+2],24,1000);
		//ADS1299_CS0=0;
		//ADS1299_CS3=0;
		//HAL_SPI_Receive_DMA(&SPI2_Handler,&adc_buf2[k*24+2],24);
		//HAL_DMA_Start(&SPI2RxDMA_Handler,(u32)&TxData1,(u32)&adc_buf2[k*24+2],24);
		//HAL_SPI_TransmitReceive_DMA(&SPI2_Handler,TxData0,RxData,4);
		//HAL_SPI_TransmitReceive_DMA(&SPI2_Handler,TxData1,&adc_buf2[k*24+2],24);
		HAL_SPI_TransmitReceive(&SPI2_Handler,TxData0,RxData,4, 1000); 
		HAL_SPI_TransmitReceive(&SPI2_Handler,TxData1,&adc_buf2[k*24+2],24, 1200); 
		//HAL_SPI_Receive_DMA(&SPI2_Handler,&adc_buf2[k*24+2],24);
//		if(__HAL_DMA_GET_FLAG(&SPI2RxDMA_Handler,DMA_FLAG_TCIF3_7))//等待DMA2_Steam7传输完成
//                {
//                    __HAL_DMA_CLEAR_FLAG(&SPI2RxDMA_Handler,DMA_FLAG_TCIF3_7);//清除DMA2_Steam7传输完成标志
//					HAL_SPI_DMAStop(&SPI2_Handler);      //传输完成以后关闭串口DMA
//					//HAL_UART_DMAStop(&SPI2_Handler);      //传输完成以后关闭串口DMA
//					//break; 
//                }
//		SPI2_ReadWriteByte(0x12);
//			//DMA1_Channel2->CCR |= 1 << 0 ;               //开启DMA通道2
//			//HAL_SPI_Receive_DMA(&SPI2_Handler, adc_buf2, 27);
//			//ADS1299_CS_SWITCH(0,0);
//			for(i=0;i<3;i++)
//			{
//				inbyte=SPI2_ReadWriteByte(0x00);
//				stat1=(stat1<<8)| inbyte;
//			}
//			
//			for(i=0;i<8;i++)
//			{
//				for(j=0;j<3;j++)
//				{
//				//	DMA_AD_Transmit(&SPI2_Handler,(u8*)tcp_server_sendbuf,96);
//						adc_buf2[n++]=SPI2_ReadWriteByte(0x00);
//					//inbyte=SPI2_ReadWriteByte(0x00);
//						//Adc_buf[i]=(Adc_buf[i]<<8)| inbyte;
//					
//				}
//				//Adcres[i+1]=(adc_buf2[n-2]<<16)+(adc_buf2[n-1]<<8)+adc_buf2[n];
//				//Adcres[i]=(buf1[0]<<16)+(buf1[1]<<8)+buf1[2];
//			}
////			
			//n=0;
			ADS1299_CHANGE_CHANEL(k,1);
			//delay_us(5);
			//ADS1299_CS3=1;
		}
		//}
			
			//netcam_line_buf0=adc_buf2;
			Num=netcam_fifo_write(adc_buf2);
			if((Num>0)&&(~(tcp_server_flag&1<<7)))
			{
				tcp_server_flag|=(1<<7);//有数据要发送
				//Num=index1;
				//index1=0;
			}
}
#endif
//陷波器中间值 
#if 1

const float NUM_NOT60[3] = {
    // 0.9980853796,   -1.855992556,   0.9980853796 //1024
	//1,  -1.9753766811902755,  0.99999999999999978
	0.9868505798785,   -1.949401623311,   0.9868505798785
};
const float DEN_NOT60[3] = {
               // 1,   -1.855992556,   0.9961706996
	//1,  -1.9494016233110956 , 0.97370115975700511
	1,   -1.949401623311,    0.973701159757
};







//1024 SAMPRATE

const float NUM_NOT50[3] = {
//     0.9982684255,   -1.903310657,   0.9982684255
//     0.9922542572,   -1.891843915,   0.9922542572
     0.9922542572,   -1.891843915,   0.9922542572
};
const float DEN_NOT50[3] = {
//                1,   -1.903310657,   0.9965368509
//                1,   -1.891843915,   0.9845084548
                1,   -1.891843915,   0.9845084548
};

#define MWSPT_NSEC 3





#if 1
//0.03hz~250hz 1024samp
const int NL[MWSPT_NSEC] = { 1,3,1 };
const float NUM[MWSPT_NSEC][3] = {
  {
     0.3308998942,              0,              0 
  },
  {
                1,              0,             -1 
  },
  {
                1,              0,              0 
  }
};
const int DL[MWSPT_NSEC] = { 1,3,1 };
const float DEN[MWSPT_NSEC][3] = {
 {
                1,              0,              0 
  },
  {
                1,   -1.336979747,   0.3382001817 
  },
  {
                1,              0,              0 
  }
};

//const float32_t firCoeffs32BS[29] = {
//  -0.001792566938705,-0.001878883511931,-0.002028505583856,-0.001671210034247,
//   1.02275721636e-18,  0.00385526032868,  0.01060603804348,  0.02058545495837,
//    0.03358461851545,  0.04878946860343,  0.06484204393623,   0.0800227460084,
//    0.09252242589602,   0.1007515123545,   0.1036231948483,   0.1007515123545,
//    0.09252242589602,   0.0800227460084,  0.06484204393623,  0.04878946860343,
//    0.03358461851545,  0.02058545495837,  0.01060603804348,  0.00385526032868,
//   1.02275721636e-18,-0.001671210034247,-0.002028505583856,-0.001878883511931,
//  -0.001792566938705
//};

#endif

#endif

typedef struct
{
__IO  float NotMid[3]; 
}IIRNOTMID;

IIRNOTMID   IIRNMID[30];

typedef struct
{
//滤波器中间值
__IO	float  w[MWSPT_NSEC][3];
}IIRTEMP;
 
 
 
IIRTEMP IIRWDATA[30];

#if 1

/*****************************************************************************
函数名: float IIRFilter(float ADdata,uint8 chnum)
功能:滤波
输入:数据+通道号
输出:滤波后的数据
编写:
******************************************************************************/	
float IIRFilter(float ADdata,u8 chnum)
{
	u8 k;
	float ADtemp=ADdata;
	float DenMulW1,DenMulW2;
	float NumMulW0,NumMulW1,NumMulW2;
	//if(Get_Filter_F()==RESET)
		//return ADtemp;
	
	for(k=0;k<MWSPT_NSEC;k++)
	 {
//计算分母各项		 
		 if(DL[k]==3)
		 {  
			 if(DEN[k][1]==1.0)
				 DenMulW1 = IIRWDATA[chnum].w[k][1];
			 else
				 DenMulW1 = DEN[k][1]*IIRWDATA[chnum].w[k][1];
			 if(DEN[k][2]==1.0)
				 DenMulW2 = IIRWDATA[chnum].w[k][2];
			 else
				 DenMulW2 = DEN[k][2]*IIRWDATA[chnum].w[k][2];
				 
			 
		   IIRWDATA[chnum].w[k][0]=ADtemp-DenMulW1-DenMulW2;
		 }
		 else
		 { 
		   IIRWDATA[chnum].w[k][0]=ADtemp;
		 }
//计算分子各项			 
		 if(NL[k]==3)
		 {
       if(NUM[k][0]==1.0)
				 NumMulW0 = IIRWDATA[chnum].w[k][0];
			 else
				 NumMulW0 = NUM[k][0]*IIRWDATA[chnum].w[k][0];
			 
       if(NUM[k][1]==1.0)
				 NumMulW1 = IIRWDATA[chnum].w[k][1];
			 else
				 NumMulW1 = NUM[k][1]*IIRWDATA[chnum].w[k][1];

       if(NUM[k][2]==1.0)
				 NumMulW2 = IIRWDATA[chnum].w[k][2];
			 else
				 NumMulW2 = NUM[k][2]*IIRWDATA[chnum].w[k][2];

			 
		   ADtemp=NumMulW0+ NumMulW1+NumMulW2;

		 }
		 else
		 {
			 if(NUM[k][0]==1.0)
		      ADtemp=IIRWDATA[chnum].w[k][0];
			 else 
		      ADtemp=NUM[k][0]*IIRWDATA[chnum].w[k][0];
		 }
			 

		 
		 IIRWDATA[chnum].w[k][2]=IIRWDATA[chnum].w[k][1];
		 IIRWDATA[chnum].w[k][1]=IIRWDATA[chnum].w[k][0];
   }
	 
	return  ADtemp;
 }	 

#endif

 #if 1
 
/*****************************************************************************
函数名: float IIRNotching(float Xindata,uint8 chnum,uint8 Ntype)
功能:陷波滤波器
输入:数据+通道号+陷波类型(=0 无;  =1 50Hz ;=2 60Hz)
输出:滤波后的数据
编写:
******************************************************************************/	
float IIRNotching(float Xindata,u8 chnum,u8 Ntype)
{
	float ADdatabuf=Xindata;
	u8  Ntypetemp=Ntype;
//	  if(Get_NotchF()==RESET)
//			return ADdatabuf;
    	
//		if(Get_NFre_F()==RESET)
		   Ntypetemp=1;
//		else
//		   Ntypetemp=2;
	
//计算分母各项		 
	
	
	
		if(Ntypetemp==1)
		{			

			
			   IIRNMID[chnum].NotMid[0]=ADdatabuf-DEN_NOT50[1]*IIRNMID[chnum].NotMid[1]-DEN_NOT50[2]*IIRNMID[chnum].NotMid[2];
 	       ADdatabuf=NUM_NOT50[0]*IIRNMID[chnum].NotMid[0]+ NUM_NOT50[1]*IIRNMID[chnum].NotMid[1]+NUM_NOT50[2]*IIRNMID[chnum].NotMid[2];
		}
		else
		{
	     IIRNMID[chnum].NotMid[0]=ADdatabuf-DEN_NOT60[1]*IIRNMID[chnum].NotMid[1]-DEN_NOT60[2]*IIRNMID[chnum].NotMid[2];
	     ADdatabuf=NUM_NOT60[0]*IIRNMID[chnum].NotMid[0]+ NUM_NOT60[1]*IIRNMID[chnum].NotMid[1]+NUM_NOT60[2]*IIRNMID[chnum].NotMid[2];
		}
			
		 IIRNMID[chnum].NotMid[2]=IIRNMID[chnum].NotMid[1];
		 IIRNMID[chnum].NotMid[1]=IIRNMID[chnum].NotMid[0];
	 
	return  ADdatabuf;
 }	
#if 0
 void arm_fir_f32_bs(void)
 {
	 u8 i;
	 arm_fir_instance_f32 S;
	 float32_t *input1,*output1;
	 input1=&input[0];
	 output1=&output[0];
	 //初始化
	 arm_fir_init_f32(&S,29,(float32_t*)&firCoeffs32BS[0],&res[0],20);
	 //实现FIR滤波
	 for(i=0;i<32;i++)//数组形式要改
	 {
		arm_fir_f32(&S,input[i],output[i],20);
	 }
	 
 }
 #endif
#endif 
//采用滤波处理
					  
void Recev_Data(void)
{
	u8 i,j,k,n=0;
	u8 buf3[28];
	//u8 res1,res2,res3;
	//u8 inbyte[100];
	//u8 Txda=0xff;
	//u32 stat1;
	//u32 Adcres[32];
	float  tempdata_f;
	//arm_fir_instance_f32 S;
	//index1=0;
	//arm_fir_instance_f32 S;
	// float32_t *input1,*output1;
//	 input1=&input[0];
	// output1=&output[0];
	 //初始化
	// arm_fir_init_f32(&S,29,(float32_t*)&firCoeffs32BS[0],&res[0],20);
	if(res3<0xff) 
	{
		res3++;
	}
	else res3=0;
	adc_buf2[0]=0xa0;
	if(openvibeflag) 
	{
		adc_buf2[1]=res3;
		adc_buf2[98]=0xc0;
	}
	else
	{
	adc_buf2[1]=sizeof(adc_buf2)-2;
	adc_buf2[100]=0xc0;
	adc_buf2[2]=0x01;
//	adc_buf2[26]=0xc0;
	
	adc_buf2[3]=res3;
	}
	//tcp_server_sendbuf=buf3;
	for(k=0;k<4;k++)
	{
		ADS1299_CHANGE_CHANEL(k,0);
		//ADS1299_DMA_Start();
		//ADS1299_DMA_Start();
//		HAL_SPI_TransmitReceive_DMA(&SPI2_Handler,TxData0,adc_buf2,28);
//		while(1)
//	{
//		if(__HAL_DMA_GET_FLAG(&DMASPIRx_Handler,DMA_FLAG_TCIF3_7))//等待DMA2_Steam7传输完成
//         {
//                    __HAL_DMA_CLEAR_FLAG(&DMASPIRx_Handler,DMA_FLAG_TCIF3_7);//清除DMA2_Steam7传输完成标志
//                    HAL_SPI_DMAStop(&SPI2_Handler);      //传输完成以后关闭串口DMA
//			 break;
//		 }
//	 }
		//HAL_SPI_TransmitReceive(&SPI2_Handler,TxData0,RxData,4, 1000); 
		HAL_SPI_TransmitReceive(&SPI2_Handler,TxData0,buf3,28, 1200); 
		if(openvibeflag) memcpy(&adc_buf2[k*24+2],&buf3[4],24);
		else
		memcpy(&adc_buf2[k*24+4],&buf3[4],24);
//		SPI2_ReadWriteByte(0x12);
//		SPI2_ReadWriteByte(0x00);
//		SPI2_ReadWriteByte(0x00);
//		SPI2_ReadWriteByte(0x00);
			
				//Adcres[index1][n]=0;
//				for(j=0;j<24;j++)
//				{

//					adc_buf2[j
//						//adc_buf2[n++]=SPI2_ReadWriteByte(0x00);
//					inbyte[k*24+3*i+j]=SPI2_ReadWriteByte(0x00);
//					Adcres[index1][k*8+i]=(Adcres[index1][k*8+i]<<8)| inbyte[k*24+3*i+j];
//					
//				}
				//n++;
				//Adcres[i+1]=(adc_buf2[n-2]<<16)+(adc_buf2[n-1]<<8)+adc_buf2[n];
				//Adcres[i]=(buf1[0]<<16)+(buf1[1]<<8)+buf1[2];
			//}
////////			
			
			ADS1299_CHANGE_CHANEL(k,1);
//			for(i=0;i<8;i++)
//			{
//				
//				//adc_buf2[0]=SPI2_ReadWriteByte(0x00);
//				Adcres[k*8+i]=buf3[3*i+4]<<24;
//				Adcres[k*8+i]|=buf3[i*3+5]<<16;
//				Adcres[k*8+i]|=buf3[i*3+6]<<8;
//				Adcres[k*4+i]/=80;
//				
//			}
//			
			//delay_us(2);
			//ADS1299_CS3=1;
			
		//}
			
		}
			//if(openvibeflag) 
			Num=netcam_fifo_write(&adc_buf2[0]);
			if((Num>0)&&(~(tcp_server_flag&1<<7)))
			{
				tcp_server_flag|=(1<<7);//有数据要发送
				
//				//Num=index1;
////				//index1=0;
			}
}
 

OS_EVENT* Sem_Task_ads1299;

 
void EXTI4_IRQHandler(void)
{
//	if(_HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13)!=RESET)
//	{
//		_HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
		OSIntEnter();
		//Recev_Data();
		OSSemPost(Sem_Task_ads1299); // 发送信号量,这个函数并不会引起系统调度，所以中断服务函数一定要简洁。
		//EXTI_ClearITPendingBit(EXTI_Line13); // 清除标志位
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
         //HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);//调用中断处理公用函数
		OSIntExit();
	//}
}

u8 err;
//led任务
void ads1299_task(void *pdata)
{
	(void) pdata;
	Sem_Task_ads1299 = OSSemCreate(0);
	while(1)
	{
		OSSemPend(Sem_Task_ads1299,0,&err);  // 等待信号量
		Recev_Data();
		OSTimeDlyHMSM(0,0,0,1);  //延时1ms
 	}
}
//中断服务程序中需要做的事情
//在HAL库中所有的外部中断服务函数都会调用此函数
//对ADS1299的数据进行处理
//u8 t;
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
////u8 inbyte,i,j,k,n=2;
////adc_buf2[0]=0xa0;
//	//adc_buf2[26]=0xc0;
//	//u32 byteCounter=0,channelData[8];
//	//delay_us(2);//做一个小延时，以防止干扰
//	if(GPIO_Pin==GPIO_PIN_13)
//	{
//		//ads1299_data
//		//ads1299_data_flag=1;
//		Recev_Data();

//		
//			
//	
//}
//}

//#define Sample 2000		//采样点数
//#define BLOCK_SIZE 20	//调用一次arm_fir_f32处理的采样点个数
//#define NUM_TAPS 28

//u32 blocksize = BLOCK_SIZE;
//u32 numBlocks=	Sample/BLOCK_SIZE;

//void arm_fir_f32bs(void)
//{
//	arm_fir_instance_f32 S;
//	float *inputf32,*outputf32;
//	

























