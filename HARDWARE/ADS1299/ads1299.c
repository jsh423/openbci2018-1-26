#include "ads1299.h"
#include "spi.h"
#include "delay.h"
#include "malloc.h"
#include "stm32f4xx_hal_gpio.h"
//#include "tcp_server.demo.h"
#include "tcp_server_demo.h"
#include "led.h"
//#include "dma.h"
//////////////////////////////////////////////////////////////////////////////////	 
//
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
u8 check=0;
u8 TxData0[4]={0x12,0,0,0}; //发送地址
u8 TxData1[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 RxData[4]; //发送地址
//u32 Adcres[32];
u8 Flag_adc;
u32 stat;
u8 index1;
u8 res3;
u8 adc_buf2[NETCAM_LINE_SIZE];
 //adc_buf2[0]=0xaa;

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
//初始化24L01的IO口
void ADS1299_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOG时钟
	__HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOI时钟
	__HAL_RCC_GPIOD_CLK_ENABLE();			//开启GPIOI时钟

	GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9; //PG10,12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //初始化

	GPIO_Initure.Pin=GPIO_PIN_3; //PG10,12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);     //初始化
	//     
	//    GPIO_Initure.Pin=GPIO_PIN_11;           //PI11
	//    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
	//    HAL_GPIO_Init(GPIOI,&GPIO_Initure);     //初始化

	GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;            //PC6
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速         
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //初始化

	SPI2_Init();    		                //初始化SPI2  /
	//SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_16); //spi速度为2.8Mhz（24L01的最大SPI时钟为10Mhz,这里大一点没关系）  
	// NRF24L01_SPI_Init();                    //针对NRF的特点修改SPI的设置
	delay_ms(50);

	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi速度为11.25Mhz（24L01的最大SPI时钟为10Mhz,这里大一点没关系） 
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
	ADS1299_SDATAC();//退出连续读数模式，以便进行寄存器的设置
	ADS1299_WREG(0X03,0xe0);//开启内部基准

	delay_ms(20);
	 		 	 
}

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
	
	GPIO_Initure.Pin=GPIO_PIN_13;               //PC13
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;
	//GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
	 //中断线13-PC13
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,4,1);   //抢占优先级为2，子优先级为3
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //使能中断线13 
	ADS1299_START=1;
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
	ADS1299_CS2=0;
	SPI2_ReadWriteByte(0x00|0x20);
	SPI2_ReadWriteByte(0x12);
	for(i=0;i<3;i++)
{
	p[i]=SPI2_ReadWriteByte(0X00);
}
	ADS1299_CS2=1;
return p[0];
}
//检测ADS1299是否存在
//返回值:0，成功;1，失败	
u8 ADS1299_Check(void)
{
	u8 buf[19];
	u8 i;
	
	//ADS1299_START=1;
	//delay_ms(1000);
	
	
	
//	delay_ms(1);
	//ADS1299_RDATAC();
	//SPI2_ReadWriteByte(0x0);
	//SPI2_ReadWriteByte(_SDATAC);
	//delay_us(10);
	//ADS1299_CS0=1;
	ADS1299_WREG(0X01,0X95);	//设置多回读模式，通信速率250SPS 1k
	ADS1299_WREG(0X02,0Xc0);//设置测试信号由内部产生
	//check=ADS1299_PREG(0X00);
	check=ADS1299_PREGS();
	//delay_ms(10);
	if(check==0x3e) 
	{
		
	//check=ADS1299_PREG(0X00);
	ADS1299_Command(0x08);
	
	
	
	ADS1299_WREG(0X05,0X50);//第一通道设置短路，测试系统噪声
	ADS1299_WREG(0X02,0XD0);//测试信号由内部产生
	//ADS1299_WREG(0x05,0x05);
	//ADS1299_WREG(0X02,0XD
//	ADS1299_WREG(0X05,0X0A);//第一通道设置为测试信号输入 此处测试BIASREF
//	ADS1299_WREG(0X03,0XF0);//开启内部基准
	
	
	//ADS1299_START=1;
	ADS1299_WREG(0X06,0x50);
	ADS1299_WREG(0X07,0x50);
	ADS1299_WREG(0X08,0X50);//第一通道设置普通输入
	ADS1299_WREG(0X09,0x50);
	ADS1299_WREG(0X0A,0x50);
	ADS1299_WREG(0X0B,0x50);
	ADS1299_WREG(0X0C,0x50);
	//ADS1299_WREG(0X0D,0X01);
	//ADS1299_WREG(0X0E,0XFF);
	//ADS1299_WREG(0X03,0xE0);//关闭阻抗测试
	ADS1299_WREG(0X15,0X10);//SRB1闭合
	//ADS1299_RDATAC();//连续模式
		delay_ms(1);
	//ADS1299_CS0=0;
	return 0;
	}
		
	
	else return 1;
	//else return 0;
//	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
//	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
//	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
//	if(i!=5)return 1;//检测24L01错误	
//	return 0;		 //检测到24L01
}	 	 


//读取寄存器数据
u8 ADS1299_PREG(u8 reg)
{
	  u8 status,Byte;	
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
//u8 regdata[18];

#if 1					  
void Recev_Data(void)
{
	u8 i,j,n=2,k;
	u8 inbyte;
	u32 stat1;
	
	//index1=0;
	adc_buf2[0]=0xa0;
	adc_buf2[NETCAM_LINE_SIZE-1]=0xc0;
	if(res3<0xff) 
	{
		res3++;
	}
	else res3=0;
	adc_buf2[1]=res3;
	//tcp_server_sendbuf=buf3;
	for(k=0;k<4;k++)
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
			delay_us(5);
			//ADS1299_CS3=1;
		}
		//}
			
			//netcam_line_buf0=adc_buf2;
			Num=netcam_fifo_write(adc_buf2);
			if((Num)&&(~(tcp_server_flag&1<<7)))
			{
				tcp_server_flag|=(1<<7);//有数据要发送
				//Num=index1;
				//index1=0;
			}
}
#endif
 
void EXTI15_10_IRQHandler(void)
{
//	if(_HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13)!=RESET)
//	{
//		_HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
		OSIntEnter();
		OSSemPost(Sem_Task_LED2); // 发送信号量,这个函数并不会引起系统调度，所以中断服务函数一定要简洁。
		//EXTI_ClearITPendingBit(EXTI_Line13); // 清除标志位
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
         //HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);//调用中断处理公用函数
		OSIntExit();
	//}
}
u8 err;
//led任务
void led_task(void *pdata)
{
	
//	Sem_Task_LED2=OSSemCreate(0);
//	OSSemPend(Sem_Task_LED2,0,&errno);
//	OSSemPost(Sem_Task_LED2);
	(void) pdata;
	Sem_Task_LED2 = OSSemCreate(0);
	//ADS1299_CS0=0;
	while(1)
	{
		OSSemPend(Sem_Task_LED2,0,&err);  // 等待信号量
//		HAL_SPI_Receive_DMA(&SPI2_Handler,&adc_buf2[24+2],24);
//		if(__HAL_DMA_GET_FLAG(&SPI2RxDMA_Handler,DMA_FLAG_TCIF3_7))//等待DMA2_Steam7传输完成
//                {
//                    __HAL_DMA_CLEAR_FLAG(&SPI2RxDMA_Handler,DMA_FLAG_TCIF3_7);//清除DMA2_Steam7传输完成标志
//					HAL_SPI_DMAStop(&SPI2_Handler);      //传输完成以后关闭串口DMA
//					//HAL_UART_DMAStop(&SPI2_Handler);      //传输完成以后关闭串口DMA
//					//break; 
//                }
		//netcam_fifo_write(adc_buf2);
		Recev_Data();
		//DMA_AD_Transmit(&SPI2_Handler,(u8*)adc_buf2,27);
			//LED0 = !LED0;
		
		OSTimeDlyHMSM(0,0,0,2);  //延时500ms
 	}
}
//中断服务程序中需要做的事情
//在HAL库中所有的外部中断服务函数都会调用此函数
//对ADS1299的数据进行处理
u8 t;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
//u8 inbyte,i,j,k,n=2;
//adc_buf2[0]=0xa0;
	//adc_buf2[26]=0xc0;
	//u32 byteCounter=0,channelData[8];
	//delay_us(2);//做一个小延时，以防止干扰
	if(GPIO_Pin==GPIO_PIN_13)
	{
		//ads1299_data
		//ads1299_data_flag=1;
		Recev_Data();
		//tcp_server_flag|=(1<<7);//有数据要发送
		
		//delay_us(2);
////		ADS1299_CS0=0;
//		ADS1299_CS0=0;
//		SPI2_ReadWriteByte(0x12);
//		
//	
//			//ADS1299_CS_SWITCH(0,0);
//			for(i=0;i<3;i++)
//			{
//				inbyte=SPI2_ReadWriteByte(0x00);
//				stat=(stat<<8)| inbyte;
//			}

//			for(i=0;i<8;i++)
//			{
//				for(j=0;j<3;j++)
//				{
//						adc_buf2[n++]=SPI2_ReadWriteByte(0x00);
//					//inbyte=SPI2_ReadWriteByte(0x00);
//						//Adc_buf[i]=(Adc_buf[i]<<8)| inbyte;
//					
//				}
//				//Adcres[i+1]=(adc_buf2[n-2]<<16)+(adc_buf2[n-1]<<8)+adc_buf2[n];
//				//Adcres[i]=(buf1[0]<<16)+(buf1[1]<<8)+buf1[2];
//			}
//			
//			//n=0;
//	
//			ADS1299_CS0=1;
//		//}
//			if(adc_buf2[1]<0xff) adc_buf2[1]++;
//			else adc_buf2[1]=0;
			//netcam_fifo_write(adc_buf2);
		
			
	
}
}

////中断服务程序中需要做的事情
////在HAL库中所有的外部中断服务函数都会调用此函数
////对ADS1299的数据进行处理
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	u8 inbyte,i,j,k,n=4;
//	adc_buf2[0]=0xaa;
//	adc_buf2[1]=0x00;
//	adc_buf2[2]=0x64;
//	adc_buf2[3]=0x40;
//	//adc_buf2[4]=0x
//	adc_buf2[52]=0xcc;
//	//u8 adc_buf2[27];
//	//u32 byteCounter=0,channelData[8];
//	delay_us(50);//做一个小延时，以防止干扰
//	if(GPIO_Pin==GPIO_PIN_13)
//	{
//		
//		//if(adc_buf2[1]<0xff) adc_buf2[1]++;
//		//else adc_buf2[1]=0;
//		//delay_us(2);
//		ADS1299_CS0=0;
//		//ADS1299_CS0=0;
//		SPI2_ReadWriteByte(0x12);
//		
//	
//			//ADS1299_CS_SWITCH(0,0);
//			for(i=0;i<3;i++)
//			{
//				inbyte=SPI2_ReadWriteByte(0x00);
//				stat=(stat<<8)| inbyte;
//			}

//			for(i=0;i<8;i++)
//			{
//				for(j=0;j<3;j++)
//				{
//						adc_buf2[n++]=SPI2_ReadWriteByte(0x00);
//					//inbyte=SPI2_ReadWriteByte(0x00);
//						//Adc_buf[i]=(Adc_buf[i]<<8)| inbyte;
//					
//				}
//				//Adcres[i+1]=(adc_buf2[n-2]<<16)+(adc_buf2[n-1]<<8)+adc_buf2[n];
//				//Adcres[i]=(buf1[0]<<16)+(buf1[1]<<8)+buf1[2];
//			}
//			
//			//n=0;
//	
//			ADS1299_CS0=1;
//		//}
//			netcam_fifo_write(adc_buf2);
//			tcp_server_flag|=(1<<7);//有数据要发送
//		//Flag_adc=1;//表示数据有更新
//	//}
//	
//}
//}
////读取SPI寄存器值
////reg:要读的寄存器
//u8 NRF24L01_Read_Reg(u8 reg)
//{
//	  u8 reg_val;	    
//   	NRF24L01_CSN=0;             //使能SPI传输		
//  	SPI2_ReadWriteByte(reg);    //发送寄存器号
//  	reg_val=SPI2_ReadWriteByte(0XFF);//读取寄存器内容
//  	NRF24L01_CSN=1;             //禁止SPI传输		    
//  	return(reg_val);            //返回状态值
//}	
////在指定位置读出指定长度的数据
////reg:寄存器(位置)
////*pBuf:数据指针
////len:数据长度
////返回值,此次读到的状态寄存器值 
//u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
//{
//	  u8 status,u8_ctr;	       
//  	NRF24L01_CSN=0;            //使能SPI传输
//  	status=SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值   	   
// 	  for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//读出数据
//  	NRF24L01_CSN=1;            //关闭SPI传输
//  	return status;             //返回读到的状态值
//}
////在指定位置写指定长度的数据
////reg:寄存器(位置)
////*pBuf:数据指针
////len:数据长度
////返回值,此次读到的状态寄存器值
//u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
//{
//	  u8 status,u8_ctr;	    
// 	  NRF24L01_CSN=0;             //使能SPI传输
//  	status = SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
//  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //写入数据	 
//  	NRF24L01_CSN=1;             //关闭SPI传输
//  	return status;              //返回读到的状态值
//}				   
////启动NRF24L01发送一次数据
////txbuf:待发送数据首地址
////返回值:发送完成状况
//u8 NRF24L01_TxPacket(u8 *txbuf)
//{
//	u8 sta;
// 	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi速度为6.75Mhz（24L01的最大SPI时钟为10Mhz）   
//	NRF24L01_CE=0;
//  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
// 	NRF24L01_CE=1;                         //启动发送	   
//	while(NRF24L01_IRQ!=0);                 //等待发送完成
//	sta=NRF24L01_Read_Reg(STATUS);          //读取状态寄存器的值	   
//	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
//	if(sta&MAX_TX)                          //达到最大重发次数
//	{
//		NRF24L01_Write_Reg(FLUSH_TX,0xff);  //清除TX FIFO寄存器 
//		return MAX_TX; 
//	}
//	if(sta&TX_OK)                           //发送完成
//	{
//		return TX_OK;
//	}
//	return 0xff;//其他原因发送失败
//}
////启动NRF24L01发送一次数据
////txbuf:待发送数据首地址
////返回值:0，接收完成；其他，错误代码
//u8 NRF24L01_RxPacket(u8 *rxbuf)
//{
//	u8 sta;		    							   
//	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi速度为6.75Mhz（24L01的最大SPI时钟为10Mhz）   
//	sta=NRF24L01_Read_Reg(STATUS);          //读取状态寄存器的值    	 
//	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
//	if(sta&RX_OK)//接收到数据
//	{
//		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
//		NRF24L01_Write_Reg(FLUSH_RX,0xff);  //清除RX FIFO寄存器 
//		return 0; 
//	}	   
//	return 1;//没收到任何数据
//}					    
////该函数初始化NRF24L01到RX模式
////设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
////当CE变高后,即进入RX模式,并可以接收数据了		   
//void NRF24L01_RX_Mode(void)
//{
//	  NRF24L01_CE=0;	  
//  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
//	  
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);       //使能通道0的自动应答    
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);   //使能通道0的接收地址  	 
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	        //设置RF通信频率		  
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);    //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);     //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
//  	NRF24L01_CE=1; //CE为高,进入接收模式 
//}						 
////该函数初始化NRF24L01到TX模式
////设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
////PWR_UP,CRC使能
////当CE变高后,即进入RX模式,并可以接收数据了		   
////CE为高大于10us,则启动发送.	 
//void NRF24L01_TX_Mode(void)
//{														 
//	NRF24L01_CE=0;	    
//  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
//  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  

//  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
//  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
//  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //设置RF通道为40
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
//  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
//	NRF24L01_CE=1;//CE为高,10us后启动发送
//}
