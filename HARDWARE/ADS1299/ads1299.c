#include "ads1299.h"
#include "spi.h"
#include "delay.h"
#include "malloc.h"
#include "stm32f4xx_hal_gpio.h"
#include  "tcp_server_demo.h"
//#include "openbci.h"
#include  <string.h>
#include "led.h"
#include "math.h"
#include "udp_demo.h" 
#include "iwdg.h"
//#include "arm_math.h"
//#include "dma.h"
//////////////////////////////////////////////////////////////////////////////////	 
//
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
u8 check=0;
u8 TxData0[55]={0x12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //发送地址
//u8 TxData1[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//u8 RxData[4][4]; //接受地址
u8 sps=0x06;
u8 gain=0x05;//06:24,05:12,04:8,03:6,02:4,01:2,00:1
u8 parameter[10];
u8 Flag_adc;
//u32 stat;
u8 index1;
u8 res3;
u8 adc_buf2[105];
u8 stat[4];
extern u8 *netcamfifobuf;
 //adc_buf2[0]=0xaa;
//int32_t Adcres[32];
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

//读取寄存器数据
u8 ADS1299_PREG(u8 reg)
{
	  u8 Byte;	
   	ADS1299_CS0=0;                 //使能SPI传输
//delay_us(200);
  	SPI2_ReadWriteByte(0X00|0X20);//发送寄存器号
	
  	SPI2_ReadWriteByte(0X00);      //写入寄存器的值
	Byte=SPI2_ReadWriteByte(0);
//	status=SPI2_ReadWriteByte(0);
	//delay_us(2);
  	ADS1299_CS0=1;                 //禁止SPI传输	   
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
void ADS1299_WREG_Single(u8 n,u8 address,u8 value)
{
	//u8 i;
	
	ADS1299_CHANGE_CHANEL(n,0);
	//delay_us(3);
	SPI2_ReadWriteByte(0X40|address);
	SPI2_ReadWriteByte(00);
	SPI2_ReadWriteByte(value);
	//delay_us(2);
	ADS1299_CHANGE_CHANEL(n,1);
	
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
//初始化24L01的IO口
void ADS1299_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOG_CLK_ENABLE();			//开启GPIOG时钟
	__HAL_RCC_GPIOD_CLK_ENABLE();			//开启GPIOI时钟
	__HAL_RCC_GPIOH_CLK_ENABLE();			//开启GPIOI时钟

//	GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5; //PG2，3，4，5ADS1299片选信号
//	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
//	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
//	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
//	HAL_GPIO_Init(GPIOG,&GPIO_Initure);     //初始化

	GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14; //片选信号
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);     //初始化
	//     
	
//	GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_3; //PH8 ADS1299 bias信号切换开关
//	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
//	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
//	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
//	HAL_GPIO_Init(GPIOH,&GPIO_Initure);     //初始化
	
	SPI2_Init();    		                //初始化SPI2  /
	delay_ms(50);

	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_16); //spi速度为11.25Mhz（24L01的最大SPI时钟为10Mhz,这里大一点没关系） 
	//ADS1299_START=0;
	ADS1299_CS0=1; //屏蔽第一片ADS1299,测试单芯片
	ADS1299_CS1=1;
	ADS1299_CS2=1;
	ADS1299_CS3=1;
	ADS1299_Command(_WAKEUP);
	ADS1299_Command(_RESET);
	
	delay_ms(5); 
	parameter[0]=0xa0;//帧头
	parameter[1]=8;//帧长
	parameter[9]=0xc0;//帧尾
	parameter[2]=0x21;
	parameter[3]=0x00;//设备代码为00脑电
	parameter[4]=32;//导联数为32
	parameter[5]=gain;		//放大倍数为24倍
	parameter[6]=sps;	//采样率：06为250，05为500sps,04为1Ksps,03为2Ksps
	parameter[7]=0;		//目前滤波放到上位机
	parameter[8]=0;
	
	delay_ms(20);
	ADS1299_SDATAC();//退出连续读数模式，以便进行寄存器的设置
	ADS1299_WREG(CONFIG3,0xe0);//开启内部基准
	
	SW_IMP=1;
	PWM_LED=1;
	delay_ms(100);
	ADS1299_WREG(CONFIG1,(0Xd0|sps));	//设置多回读模式，通信速率250SPS 1k	 
	delay_ms(10);
	ADS1299_WREG_Single(0,CONFIG1,(0Xf0|sps));
	delay_ms(5);
}
		
	
void ADS1299_IT(void)
{
		  GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOD_CLK_ENABLE();			//开启GPIOI时钟
	
	
	GPIO_Initure.Pin=GPIO_PIN_10;               //PB12
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;
	//GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
	
	 //中断线13-PD10
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,0,1);   //抢占优先级为2，子优先级为1
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //使能中断线13 
	if(openvibeflag) ADS1299_Command(_START);
	else ADS1299_Command(_STOP);
	delay_ms(10);
}
u8 p[18];
u8 ADS1299_PREGS(void)
{
	u8 i;
	ADS1299_CS0=0;
	SPI2_ReadWriteByte(0x00|0x20);
	SPI2_ReadWriteByte(0x12);
	for(i=0;i<3;i++)
	{
		p[i]=SPI2_ReadWriteByte(0X00);
	}
	ADS1299_CS0=1;
return p[0];
}
//检测ADS1299是否存在
//返回值:0，成功;1，失败	
u8 ADS1299_Check(void)
{
	
	check=ADS1299_PREG(ID);

	//if(check==0x3e) 
	{

		
		ADS1299_WREG(CONFIG2,0XD0);//测试信号由内部产生
		ADS1299_WREG(CONFIG3,0XFc);//开启内部基准
		//ADS1299_WREG_Single(0,CONFIG3,0XFc);
		delay_ms(100);
		ADS1299_WREG(LOFF,0x00);//DC-Lead-off检查
		ADS1299_WREG(CH1SET,(0x0|(gain<<4)));//第一通道设置短路，测试系统噪声
		ADS1299_WREG(CH2SET,(0x0|(gain<<4)));
		ADS1299_WREG(CH3SET,(0x0|(gain<<4)));
		ADS1299_WREG(CH4SET,(0X0|(gain<<4)));//第一通道设置普通输入
		ADS1299_WREG(CH5SET,(0X0|(gain<<4)));
		ADS1299_WREG(CH6SET,(0X0|(gain<<4)));
		ADS1299_WREG(CH7SET,(0X00|(gain<<4)));
		ADS1299_WREG(CH8SET,(0X00|(gain<<4)));
		//ADS1299_START=1;
//		ADS1299_WREG(CH1SET,(0x08|(gain<<4)));//第一通道设置短路，测试系统噪声
//		ADS1299_WREG(CH2SET,(0x08|(gain<<4)));
//		ADS1299_WREG(CH3SET,(0x08|(gain<<4)));
//		ADS1299_WREG(CH4SET,(0X08|(gain<<4)));//第一通道设置普通输入
//		ADS1299_WREG(CH5SET,(0X08|(gain<<4)));
//		ADS1299_WREG(CH6SET,(0X08|(gain<<4)));
//		ADS1299_WREG(CH7SET,(0X08|(gain<<4)));
//		ADS1299_WREG(CH8SET,(0X08|(gain<<4)));
		ADS1299_WREG(BIAS_SENSP,0Xff);//开启直流偏置
		ADS1299_WREG(BIAS_SENSN,0Xff);
       //ADS1299_WREG_Single(0,BIAS_SENSP,0X10);
		//ADS1299_WREG_Single(0,BIAS_SENSP,0X10);
////		
		//ADS1299_WREG_Single(1,CH7SET,(0X00|(gain<<4)));
		//ADS1299_WREG_Single(0,LOFF_SENSP,0Xff);
		//ADS1299_WREG_Single(0,LOFF_SENSN,0xff);
		ADS1299_WREG(LOFF_SENSP,0xff);//开启阻抗测试
		ADS1299_WREG(LOFF_SENSN,0xff);
		//ADS1299_WREG(LOFF_FLIP,0XFF);
		ADS1299_WREG(LOFF,0x00);//DC-Lead-off检查
		
		//ADS1299_WREG(LOFF_SENSP,0xff);//开启阻抗测试
		
		//ADS1299_WREG_Single(3,LOFF_SENSN,0Xff);
		//ADS1299_WREG(LOFF_SENSN,0xff);
	
	
		ADS1299_WREG_Single(0,MISC1,0X20);
		ADS1299_WREG_Single(1,MISC1,0X20);
		ADS1299_WREG_Single(2,MISC1,0X20);
		//ADS1299_WREG_Single(3,MISC1,0X20);
		
		//ADS1299_WREG(MISC1,0X20);//SRB1闭合
		//ADS1299_WREG(CONFIG4,2);//开启leadoff比较器
		//ADS1299_RDATAC();//连续模式
		ADS1299_Command(_RDATA);//命令读取数据模式
		SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi速度为11.25Mhz（24L01的最大SPI时钟为10Mhz,这里大一点没关系） 
		delay_ms(1);
		//ADS1299_CS0=0;
		return 0;
	}
		
	
	//else return 1;

}	 
void ImpTest_Start(void)
{
	//ADS1299_SDATAC();//退出连续读数模式，以便进行寄存器的设置//进入设置模式
	ADS1299_WREG(LOFF,0x0a);//AC-Lead-off检查
	SW_IMP=0;
	PWM_LED=0;
	//ADS1299_WREG(BIAS_SENSP,0Xff);//开启直流偏置
	//ADS1299_WREG(BIAS_SENSN,0Xff);
	//ADS1299_WREG(LOFF_SENSP,0xff);//开启阻抗测试
	//ADS1299_WREG(LOFF_SENSN,0xff);
}

void ImpTest_Stop(void)
{
	//ADS1299_SDATAC();//退出连续读数模式，以便进行寄存器的设置//进入设置模式
	ADS1299_WREG(LOFF,0x00);//AC-Lead-off检查关闭
	SW_IMP=1;
	PWM_LED=1;
	//ADS1299_WREG(BIAS_SENSP,0Xff);//开启直流偏置
	//ADS1299_WREG(BIAS_SENSN,0Xff);
	//ADS1299_WREG(LOFF_SENSP,0x00);//关闭阻抗测试
	//ADS1299_WREG(LOFF_SENSN,0x00);
}
	
void Set_Sps(u8 i)
{
				//ADS1299_START=0;//先将ADS1299数据采样给关掉
//				ADS1299_SDATAC();//退出连续读数模式，以便进行寄存器的设置//进入设置模式
//				//ADS1299_WREG(CONFIG1,(0Xd0|sps));	//设置多回读模式，通信速率250SPS 1k	 
				ADS1299_WREG_Single(0,CONFIG1,(0Xf0|i));
				ADS1299_WREG_Single(1,CONFIG1,(0Xd0|i));
				ADS1299_WREG_Single(2,CONFIG1,(0Xd0|i));
				ADS1299_WREG_Single(3,CONFIG1,(0Xd0|i));
//				ADS1299_WREG(CONFIG1,(0Xd0|i));	//设置多回读模式，通信速率250SPS 1k	 
//				ADS1299_WREG_Single(0,CONFIG1,(0Xf0|i));
				//ADS1299_WREG(0X01,(0X90|sps));	//设置多回读模式，通信速率250SPS 1k//设置采样率
				//ADS1299_Command(0x12);//命令读取数据模式//退出设置模式
}

//u8 regdata[18];

				  


					  
void Recev_Data(void)
{
	u8 k;
	u8 buf3[28]={0};

	
	adc_buf2[0]=0xa0;
	if(openvibeflag) 
	{
		adc_buf2[1]=res3;
		adc_buf2[102]=0xc0;
	}
	else
	{
	adc_buf2[1]=sizeof(adc_buf2)-2;
	adc_buf2[104]=0xc0;
	adc_buf2[2]=0x01;
//	adc_buf2[26]=0xc0;
	
	adc_buf2[3]=res3++;
	}
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
//		for(n=0;n<24;n++)
//		{
//			buf3[n]=n;
//		}
		if(openvibeflag) memcpy(&adc_buf2[k*24+2],&buf3[4],24);
		else
		{
		stat[k]=(buf3[1]<<4)|(buf3[2]>>4);
		memcpy(&adc_buf2[k*24+8],&buf3[4],24);
		//memcpy(&adc_buf2[4],&buf3[4],24);
	//	memcpy(&stat[k*3],&buf3[1],3);
		//stat[k]=buf3[0]
		}
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
			memcpy(&adc_buf2[4],stat,4);
			Num=netcam_fifo_write(&adc_buf2[0]);
			if((Num>0)&&(~(tcp_server_flag&1<<7)))
			{
				tcp_server_flag|=(1<<7);//有数据要发送
				//udp_demo_flag|=(1<<7);
				
//				//Num=index1;
////				//index1=0;
			}
}
 
//void EXTI4_IRQHandler(void)
void EXTI15_10_IRQHandler(void)
{
//	if(_HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13)!=RESET)
//	{
//		_HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
		//OSIntEnter();
		//OSSemPost(Sem_Task_ads1299); // 发送信号量,这个函数并不会引起系统调度，所以中断服务函数一定要简洁。
		//EXTI_ClearITPendingBit(EXTI_Line13); // 清除标志位
		//index1=0;
		//ADS1299_CS0=0;
		//HAL_SPI_TransmitReceive_DMA(&SPI2_Handler,TxData0,netcamfifobuf,55);
		Recev_Data();
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);
         //HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);//调用中断处理公用函数
		//OSIntExit();
	//}
}

//void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
//{
//  /* Turn LED2 on: Transfer in transmission/reception process is complete */
//  //BSP_LED_On(LED2);
//	u8 buf[28];
//  //wTransferState = TRANSFER_COMPLETE;
//	//static u8 Num1;
//	
//	switch(index1)
//	{
//		case 0:
//	
//		ADS1299_CS0=1;
//		memcpy(&adc_buf2[index1*24+8],&buf[4],28);
////		ADS1299_CS3=0;
////		HAL_SPI_TransmitReceive_DMA(&SPI2_Handler,TxData0,buf,28);
//		//memcpy(netcamfifobuf[28],buf,28);
//		tcp_server_flag|=(1<<7);//有数据要发送
//			//udp_demo_flag|=(1<<7);
//			
////				//Num=index1;
//////				//index1=0;
//	//}
//		//memset(adc_buf2,0,105);
////		tcp_server_flag|=(1<<7);//有数据要发送
//		tcp_server_sendbuf=adc_buf2;
//		len=28*4;
//		//sprintf(netcamfifobuf,%x,buf);
//		index1=4;
//		break;
//		case 1:
//		ADS1299_CS1=1;
//		memcpy(&adc_buf2[index1*24+8],&buf[4],24);
//		ADS1299_CS2=0;
//		HAL_SPI_TransmitReceive_DMA(&SPI2_Handler,TxData0,buf,28);
//		///memcpy(&adc_buf2[index1*24+2],&buf[4],24);
//		index1=2;
//		break;
//		case 2:
//		ADS1299_CS2=1;
//		memcpy(&adc_buf2[index1*24+8],&buf[4],24);
//		ADS1299_CS3=0;
//		HAL_SPI_TransmitReceive_DMA(&SPI2_Handler,TxData0,buf,28);
//		//memcpy(&adc_buf2[index1*24+2],&buf[4],24);
//		index1=3;
//		break;
//		case 3:
//		ADS1299_CS3=1;
//		memcpy(&adc_buf2[index1*24+8],&buf[4],24);
//		//memcpy(&adc_buf2[4],stat,4);
//		//Num=netcam_fifo_write(&adc_buf2[0]);
//		//if((Num>0)&&(~(tcp_server_flag&1<<7)))
//		{
//			tcp_server_flag|=(1<<7);//有数据要发送
//			//udp_demo_flag|=(1<<7);
//			
////				//Num=index1;
//////				//index1=0;
//		}
//		//memset(adc_buf2,0,105);
////		tcp_server_flag|=(1<<7);//有数据要发送
//		tcp_server_sendbuf=adc_buf2;
//		len=55;
//		index1=4;
//		break;
//		default:
//			break;
//	
//	}

//}
//led任务
//void ads1299_task(void *pdata)
//{
//	
////	Sem_Task_LED2=OSSemCreate(0);
////	OSSemPend(Sem_Task_LED2,0,&errno);
////	OSSemPost(Sem_Task_LED2);
//	(void) pdata;
//	Sem_Task_ads1299 = OSSemCreate(0);
//	//ADS1299_CS0=0;
//	while(1)
//	{
//		//OSSemPend(Sem_Task_ads1299,0,&err);  // 等待信号量
//		//Recev_Data();
//		
//			
//		OSTimeDlyHMSM(0,0,0,20);  //延时500ms
// 	}
//}
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

























