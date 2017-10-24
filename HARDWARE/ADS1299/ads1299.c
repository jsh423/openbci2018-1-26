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
u8 TxData0[4]={0x12,0,0,0}; //���͵�ַ
u8 TxData1[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 RxData[4]; //���͵�ַ
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
//��ʼ��24L01��IO��
void ADS1299_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOGʱ��
	__HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOIʱ��
	__HAL_RCC_GPIOD_CLK_ENABLE();			//����GPIOIʱ��

	GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9; //PG10,12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //��ʼ��

	GPIO_Initure.Pin=GPIO_PIN_3; //PG10,12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);     //��ʼ��
	//     
	//    GPIO_Initure.Pin=GPIO_PIN_11;           //PI11
	//    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
	//    HAL_GPIO_Init(GPIOI,&GPIO_Initure);     //��ʼ��

	GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;            //PC6
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����         
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //��ʼ��

	SPI2_Init();    		                //��ʼ��SPI2  /
	//SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_16); //spi�ٶ�Ϊ2.8Mhz��24L01�����SPIʱ��Ϊ10Mhz,�����һ��û��ϵ��  
	// NRF24L01_SPI_Init();                    //���NRF���ص��޸�SPI������
	delay_ms(50);

	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi�ٶ�Ϊ11.25Mhz��24L01�����SPIʱ��Ϊ10Mhz,�����һ��û��ϵ�� 
	ADS1299_START=0;
	ADS1299_CS0=1; //���ε�һƬADS1299,���Ե�оƬ
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
	ADS1299_SDATAC();//�˳���������ģʽ���Ա���мĴ���������
	ADS1299_WREG(0X03,0xe0);//�����ڲ���׼

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
   // __HAL_RCC_GPIOG_CLK_ENABLE();			//����GPIOGʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOIʱ��
	
//	GPIO_Initure.Pin=GPIO_PIN_12;               //PC12
//	//GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //�����ش���
//   // GPIO_Initure.Pull=GPIO_PULLDOWN;
//    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
//    GPIO_Initure.Pull=GPIO_PULLUP;
//    HAL_GPIO_Init(GPIOC,&GPIO_Initure);		//��ʱ����PH13Ϊ����
	
	GPIO_Initure.Pin=GPIO_PIN_13;               //PC13
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;
	//GPIO_MODE_IT_FALLING;     //�½��ش���
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
	 //�ж���13-PC13
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,4,1);   //��ռ���ȼ�Ϊ2�������ȼ�Ϊ3
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //ʹ���ж���13 
	ADS1299_START=1;
	delay_ms(10);
	//ADS1299_RDATAC();
//	 //�ж���12-PC12
//    HAL_NVIC_SetPriority(EXTI15_10_IRQn,2,1);   //��ռ���ȼ�Ϊ2�������ȼ�Ϊ3
//    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //ʹ���ж���12
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
//���ADS1299�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
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
	ADS1299_WREG(0X01,0X95);	//���ö�ض�ģʽ��ͨ������250SPS 1k
	ADS1299_WREG(0X02,0Xc0);//���ò����ź����ڲ�����
	//check=ADS1299_PREG(0X00);
	check=ADS1299_PREGS();
	//delay_ms(10);
	if(check==0x3e) 
	{
		
	//check=ADS1299_PREG(0X00);
	ADS1299_Command(0x08);
	
	
	
	ADS1299_WREG(0X05,0X50);//��һͨ�����ö�·������ϵͳ����
	ADS1299_WREG(0X02,0XD0);//�����ź����ڲ�����
	//ADS1299_WREG(0x05,0x05);
	//ADS1299_WREG(0X02,0XD
//	ADS1299_WREG(0X05,0X0A);//��һͨ������Ϊ�����ź����� �˴�����BIASREF
//	ADS1299_WREG(0X03,0XF0);//�����ڲ���׼
	
	
	//ADS1299_START=1;
	ADS1299_WREG(0X06,0x50);
	ADS1299_WREG(0X07,0x50);
	ADS1299_WREG(0X08,0X50);//��һͨ��������ͨ����
	ADS1299_WREG(0X09,0x50);
	ADS1299_WREG(0X0A,0x50);
	ADS1299_WREG(0X0B,0x50);
	ADS1299_WREG(0X0C,0x50);
	//ADS1299_WREG(0X0D,0X01);
	//ADS1299_WREG(0X0E,0XFF);
	//ADS1299_WREG(0X03,0xE0);//�ر��迹����
	ADS1299_WREG(0X15,0X10);//SRB1�պ�
	//ADS1299_RDATAC();//����ģʽ
		delay_ms(1);
	//ADS1299_CS0=0;
	return 0;
	}
		
	
	else return 1;
	//else return 0;
//	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
//	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
//	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
//	if(i!=5)return 1;//���24L01����	
//	return 0;		 //��⵽24L01
}	 	 


//��ȡ�Ĵ�������
u8 ADS1299_PREG(u8 reg)
{
	  u8 status,Byte;	
   	ADS1299_CS0=0;                 //ʹ��SPI����
//delay_us(200);
  	SPI2_ReadWriteByte(0X00|0X20);//���ͼĴ�����
	
  	SPI2_ReadWriteByte(0X00);      //д��Ĵ�����ֵ
	Byte=SPI2_ReadWriteByte(0);
//	status=SPI2_ReadWriteByte(0);
	//delay_us(2);
  	ADS1299_CS0=1;                 //��ֹSPI����	   
  	return(Byte);       		    //����״ֵ̬
}
//д��ADS1299�Ĵ�������
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
//		if(__HAL_DMA_GET_FLAG(&SPI2RxDMA_Handler,DMA_FLAG_TCIF3_7))//�ȴ�DMA2_Steam7�������
//                {
//                    __HAL_DMA_CLEAR_FLAG(&SPI2RxDMA_Handler,DMA_FLAG_TCIF3_7);//���DMA2_Steam7������ɱ�־
//					HAL_SPI_DMAStop(&SPI2_Handler);      //��������Ժ�رմ���DMA
//					//HAL_UART_DMAStop(&SPI2_Handler);      //��������Ժ�رմ���DMA
//					//break; 
//                }
//		SPI2_ReadWriteByte(0x12);
//			//DMA1_Channel2->CCR |= 1 << 0 ;               //����DMAͨ��2
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
				tcp_server_flag|=(1<<7);//������Ҫ����
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
		OSSemPost(Sem_Task_LED2); // �����ź���,�����������������ϵͳ���ȣ������жϷ�����һ��Ҫ��ࡣ
		//EXTI_ClearITPendingBit(EXTI_Line13); // �����־λ
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
         //HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);//�����жϴ����ú���
		OSIntExit();
	//}
}
u8 err;
//led����
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
		OSSemPend(Sem_Task_LED2,0,&err);  // �ȴ��ź���
//		HAL_SPI_Receive_DMA(&SPI2_Handler,&adc_buf2[24+2],24);
//		if(__HAL_DMA_GET_FLAG(&SPI2RxDMA_Handler,DMA_FLAG_TCIF3_7))//�ȴ�DMA2_Steam7�������
//                {
//                    __HAL_DMA_CLEAR_FLAG(&SPI2RxDMA_Handler,DMA_FLAG_TCIF3_7);//���DMA2_Steam7������ɱ�־
//					HAL_SPI_DMAStop(&SPI2_Handler);      //��������Ժ�رմ���DMA
//					//HAL_UART_DMAStop(&SPI2_Handler);      //��������Ժ�رմ���DMA
//					//break; 
//                }
		//netcam_fifo_write(adc_buf2);
		Recev_Data();
		//DMA_AD_Transmit(&SPI2_Handler,(u8*)adc_buf2,27);
			//LED0 = !LED0;
		
		OSTimeDlyHMSM(0,0,0,2);  //��ʱ500ms
 	}
}
//�жϷ����������Ҫ��������
//��HAL�������е��ⲿ�жϷ�����������ô˺���
//��ADS1299�����ݽ��д���
u8 t;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
//u8 inbyte,i,j,k,n=2;
//adc_buf2[0]=0xa0;
	//adc_buf2[26]=0xc0;
	//u32 byteCounter=0,channelData[8];
	//delay_us(2);//��һ��С��ʱ���Է�ֹ����
	if(GPIO_Pin==GPIO_PIN_13)
	{
		//ads1299_data
		//ads1299_data_flag=1;
		Recev_Data();
		//tcp_server_flag|=(1<<7);//������Ҫ����
		
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

////�жϷ����������Ҫ��������
////��HAL�������е��ⲿ�жϷ�����������ô˺���
////��ADS1299�����ݽ��д���
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
//	delay_us(50);//��һ��С��ʱ���Է�ֹ����
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
//			tcp_server_flag|=(1<<7);//������Ҫ����
//		//Flag_adc=1;//��ʾ�����и���
//	//}
//	
//}
//}
////��ȡSPI�Ĵ���ֵ
////reg:Ҫ���ļĴ���
//u8 NRF24L01_Read_Reg(u8 reg)
//{
//	  u8 reg_val;	    
//   	NRF24L01_CSN=0;             //ʹ��SPI����		
//  	SPI2_ReadWriteByte(reg);    //���ͼĴ�����
//  	reg_val=SPI2_ReadWriteByte(0XFF);//��ȡ�Ĵ�������
//  	NRF24L01_CSN=1;             //��ֹSPI����		    
//  	return(reg_val);            //����״ֵ̬
//}	
////��ָ��λ�ö���ָ�����ȵ�����
////reg:�Ĵ���(λ��)
////*pBuf:����ָ��
////len:���ݳ���
////����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
//u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
//{
//	  u8 status,u8_ctr;	       
//  	NRF24L01_CSN=0;            //ʹ��SPI����
//  	status=SPI2_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
// 	  for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//��������
//  	NRF24L01_CSN=1;            //�ر�SPI����
//  	return status;             //���ض�����״ֵ̬
//}
////��ָ��λ��дָ�����ȵ�����
////reg:�Ĵ���(λ��)
////*pBuf:����ָ��
////len:���ݳ���
////����ֵ,�˴ζ�����״̬�Ĵ���ֵ
//u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
//{
//	  u8 status,u8_ctr;	    
// 	  NRF24L01_CSN=0;             //ʹ��SPI����
//  	status = SPI2_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
//  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //д������	 
//  	NRF24L01_CSN=1;             //�ر�SPI����
//  	return status;              //���ض�����״ֵ̬
//}				   
////����NRF24L01����һ������
////txbuf:�����������׵�ַ
////����ֵ:�������״��
//u8 NRF24L01_TxPacket(u8 *txbuf)
//{
//	u8 sta;
// 	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi�ٶ�Ϊ6.75Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
//	NRF24L01_CE=0;
//  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
// 	NRF24L01_CE=1;                         //��������	   
//	while(NRF24L01_IRQ!=0);                 //�ȴ��������
//	sta=NRF24L01_Read_Reg(STATUS);          //��ȡ״̬�Ĵ�����ֵ	   
//	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
//	if(sta&MAX_TX)                          //�ﵽ����ط�����
//	{
//		NRF24L01_Write_Reg(FLUSH_TX,0xff);  //���TX FIFO�Ĵ��� 
//		return MAX_TX; 
//	}
//	if(sta&TX_OK)                           //�������
//	{
//		return TX_OK;
//	}
//	return 0xff;//����ԭ����ʧ��
//}
////����NRF24L01����һ������
////txbuf:�����������׵�ַ
////����ֵ:0��������ɣ��������������
//u8 NRF24L01_RxPacket(u8 *rxbuf)
//{
//	u8 sta;		    							   
//	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi�ٶ�Ϊ6.75Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
//	sta=NRF24L01_Read_Reg(STATUS);          //��ȡ״̬�Ĵ�����ֵ    	 
//	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
//	if(sta&RX_OK)//���յ�����
//	{
//		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
//		NRF24L01_Write_Reg(FLUSH_RX,0xff);  //���RX FIFO�Ĵ��� 
//		return 0; 
//	}	   
//	return 1;//û�յ��κ�����
//}					    
////�ú�����ʼ��NRF24L01��RXģʽ
////����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
////��CE��ߺ�,������RXģʽ,�����Խ���������		   
//void NRF24L01_RX_Mode(void)
//{
//	  NRF24L01_CE=0;	  
//  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
//	  
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);       //ʹ��ͨ��0���Զ�Ӧ��    
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);   //ʹ��ͨ��0�Ľ��յ�ַ  	 
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	        //����RFͨ��Ƶ��		  
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	    
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);    //����TX�������,0db����,2Mbps,���������濪��   
//  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);     //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
//  	NRF24L01_CE=1; //CEΪ��,�������ģʽ 
//}						 
////�ú�����ʼ��NRF24L01��TXģʽ
////����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
////PWR_UP,CRCʹ��
////��CE��ߺ�,������RXģʽ,�����Խ���������		   
////CEΪ�ߴ���10us,����������.	 
//void NRF24L01_TX_Mode(void)
//{														 
//	NRF24L01_CE=0;	    
//  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
//  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

//  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
//  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
//  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //����RFͨ��Ϊ40
//  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
//  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
//	NRF24L01_CE=1;//CEΪ��,10us����������
//}
