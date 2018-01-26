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
u8 TxData0[28]={0x12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //���͵�ַ
//u8 TxData1[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
u8 RxData[4]; //���͵�ַ
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
//��ʼ��ADS1299��IO��
void ADS1299_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOGʱ��
	__HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOGʱ��
	__HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOIʱ��
	__HAL_RCC_GPIOD_CLK_ENABLE();			//����GPIOIʱ��

	GPIO_Initure.Pin=GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_12; //PG10,12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //��ʼ��

	GPIO_Initure.Pin=GPIO_PIN_15; //PG10,12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);     //��ʼ��
	//     
	//    GPIO_Initure.Pin=GPIO_PIN_11;           //PI11
	//    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
	//    HAL_GPIO_Init(GPIOI,&GPIO_Initure);     //��ʼ��

	GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;            //PC6
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����         
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //��ʼ��

	SPI2_Init();    		                //��ʼ��SPI2  /
	//SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_16); //spi�ٶ�Ϊ2.8Mhz��24L01�����SPIʱ��Ϊ10Mhz,�����һ��û��ϵ��  
	// NRF24L01_SPI_Init();                    //���NRF���ص��޸�SPI������
	delay_ms(50);

	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_16); //spi�ٶ�Ϊ11.25Mhz��24L01�����SPIʱ��Ϊ10Mhz,�����һ��û��ϵ�� 
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
	//ADS1299_SDATAC();//�˳���������ģʽ���Ա���мĴ���������
	//ADS1299_WREG(0X03,0xe0);//�����ڲ���׼
	parameter[0]=0xa0;//֡ͷ
	parameter[1]=8;//֡��
	parameter[9]=0xc0;//֡β
	parameter[2]=0x21;
	parameter[3]=0x00;//�豸����Ϊ00�Ե�
	parameter[4]=32;//������Ϊ32
	parameter[5]=1;		//�Ŵ���Ϊ24��
	parameter[6]=sps;	//�����ʣ�06Ϊ250��05Ϊ500sps,04Ϊ1Ksps,03Ϊ2Ksps
	parameter[7]=0;		//Ŀǰ�˲��ŵ���λ��
	parameter[8]=0;
	
	delay_ms(20);
	 	ADS1299_SDATAC();//�˳���������ģʽ���Ա���мĴ���������
	ADS1299_WREG(0X03,0xe0);//�����ڲ���׼
	
	delay_ms(10);
	//ADS1299_RDATAC();
	//SPI2_ReadWriteByte(0x0);
	//SPI2_ReadWriteByte(_SDATAC);
	//delay_us(10);
	//ADS1299_CS0=1;
	ADS1299_WREG(0X01,0X94);	//���ö�ض�ģʽ��ͨ������250SPS 1k	 
	ADS1299_Check();	
//	ADS1299_WREG(0X05,0X00);//��һͨ�����ö�·������ϵͳ����
//	//ADS1299_WREG(0X02,0XD0);//�����ź����ڲ�����
//	//ADS1299_WREG(0x05,0x05);
//	//ADS1299_WREG(0X02,0XD
////	ADS1299_WREG(0X05,0X0A);//��һͨ������Ϊ�����ź����� �˴�����BIASREF
////	ADS1299_WREG(0X03,0XF0);//�����ڲ���׼
//	
//	
//	//ADS1299_START=1;
//	ADS1299_WREG(0X06,0x05);
//	ADS1299_WREG(0X07,0x00);
//	ADS1299_WREG(0X08,0X00);//��һͨ��������ͨ����
//	ADS1299_WREG(0X09,0x00);
//	ADS1299_WREG(0X0A,0x00);
//	ADS1299_WREG(0X0B,0x00);
//	ADS1299_WREG(0X0C,0x00);
//	//ADS1299_WREG(0X0D,0Xff);
//	//ADS1299_WREG(0X0E,0XFF);
//	//ADS1299_WREG(0X03,0xE0);//�ر��迹����
//	ADS1299_WREG(0X15,0X10);//SRB1�պ�
//	//ADS1299_RDATAC();//����ģʽ
//	ADS1299_Command(0x12);//�����ȡ����ģʽ
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
   // __HAL_RCC_GPIOG_CLK_ENABLE();			//����GPIOGʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOIʱ��
	
//	GPIO_Initure.Pin=GPIO_PIN_12;               //PC12
//	//GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //�����ش���
//   // GPIO_Initure.Pull=GPIO_PULLDOWN;
//    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //�½��ش���
//    GPIO_Initure.Pull=GPIO_PULLUP;
//    HAL_GPIO_Init(GPIOC,&GPIO_Initure);		//��ʱ����PH13Ϊ����
	
	GPIO_Initure.Pin=GPIO_PIN_4;               //PC13
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;
	//GPIO_MODE_IT_FALLING;     //�½��ش���
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	 //�ж���13-PC13
    HAL_NVIC_SetPriority(EXTI4_IRQn,1,1);   //��ռ���ȼ�Ϊ2�������ȼ�Ϊ1
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);         //ʹ���ж���13 
	if(openvibeflag) ADS1299_START=1;
	else ADS1299_START=0;
	//ADS1299_Command(0x08);
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
//���ADS1299�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
u8 ADS1299_Check(void)
{
	//u8 buf[19];
//	u8 i;
	
	//ADS1299_START=1;
	//delay_ms(1000);
	
	
	//ADS1299_WREG(0X02,0Xc0);//���ò����ź����ڲ�����
	check=ADS1299_PREG(0X00);
	//check=ADS1299_PREGS();
	//delay_ms(10);
//	if(check==0x3e) 
	{
		
	//check=ADS1299_PREG(0X00);
	//ADS1299_Command(0x08);
	
	
	
	ADS1299_WREG(0X05,0X05);//��һͨ�����ö�·������ϵͳ����
	//ADS1299_WREG(0X02,0XD0);//�����ź����ڲ�����
	//ADS1299_WREG(0x05,0x05);
	//ADS1299_WREG(0X02,0XD
//	ADS1299_WREG(0X05,0X0A);//��һͨ������Ϊ�����ź����� �˴�����BIASREF
//	ADS1299_WREG(0X03,0XF0);//�����ڲ���׼
	
	
	//ADS1299_START=1;
	ADS1299_WREG(0X06,0x05);
	ADS1299_WREG(0X07,0x05);
	ADS1299_WREG(0X08,0X00);//��һͨ��������ͨ����
	ADS1299_WREG(0X09,0x00);
	ADS1299_WREG(0X0A,0x00);
	ADS1299_WREG(0X0B,0x00);
	ADS1299_WREG(0X0C,0x00);
	//ADS1299_WREG(0X0D,0Xff);
	//ADS1299_WREG(0X0E,0XFF);
	//ADS1299_WREG(0X03,0xE0);//�ر��迹����
	ADS1299_WREG(0X15,0X10);//SRB1�պ�
	//ADS1299_RDATAC();//����ģʽ
	ADS1299_Command(0x12);//�����ȡ����ģʽ
	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi�ٶ�Ϊ11.25Mhz��24L01�����SPIʱ��Ϊ10Mhz,�����һ��û��ϵ�� 
		delay_ms(1);
	//ADS1299_CS0=0;
	return 0;
	}
		
	
	//else return 1;
	//else return 0;
//	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
//	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
//	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
//	if(i!=5)return 1;//���24L01����	
//	return 0;		 //��⵽24L01
}	 

void Set_Sps(u8 i)
{
				ADS1299_START=0;//�Ƚ�ADS1299���ݲ������ص�
				ADS1299_SDATAC();//�˳���������ģʽ���Ա���мĴ���������//��������ģʽ
				ADS1299_WREG(0X01,(0X90|sps));	//���ö�ض�ģʽ��ͨ������250SPS 1k//���ò�����
				ADS1299_Command(0x12);//�����ȡ����ģʽ//�˳�����ģʽ
}

//��ȡ�Ĵ�������
u8 ADS1299_PREG(u8 reg)
{
	  u8 Byte;	
   	ADS1299_CS3=0;                 //ʹ��SPI����
//delay_us(200);
  	SPI2_ReadWriteByte(0X00|0X20);//���ͼĴ�����
	
  	SPI2_ReadWriteByte(0X00);      //д��Ĵ�����ֵ
	Byte=SPI2_ReadWriteByte(0);
//	status=SPI2_ReadWriteByte(0);
	//delay_us(2);
  	ADS1299_CS3=1;                 //��ֹSPI����	   
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
			//delay_us(5);
			//ADS1299_CS3=1;
		}
		//}
			
			//netcam_line_buf0=adc_buf2;
			Num=netcam_fifo_write(adc_buf2);
			if((Num>0)&&(~(tcp_server_flag&1<<7)))
			{
				tcp_server_flag|=(1<<7);//������Ҫ����
				//Num=index1;
				//index1=0;
			}
}
#endif
//�ݲ����м�ֵ 
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
//�˲����м�ֵ
__IO	float  w[MWSPT_NSEC][3];
}IIRTEMP;
 
 
 
IIRTEMP IIRWDATA[30];

#if 1

/*****************************************************************************
������: float IIRFilter(float ADdata,uint8 chnum)
����:�˲�
����:����+ͨ����
���:�˲��������
��д:
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
//�����ĸ����		 
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
//������Ӹ���			 
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
������: float IIRNotching(float Xindata,uint8 chnum,uint8 Ntype)
����:�ݲ��˲���
����:����+ͨ����+�ݲ�����(=0 ��;  =1 50Hz ;=2 60Hz)
���:�˲��������
��д:
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
	
//�����ĸ����		 
	
	
	
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
	 //��ʼ��
	 arm_fir_init_f32(&S,29,(float32_t*)&firCoeffs32BS[0],&res[0],20);
	 //ʵ��FIR�˲�
	 for(i=0;i<32;i++)//������ʽҪ��
	 {
		arm_fir_f32(&S,input[i],output[i],20);
	 }
	 
 }
 #endif
#endif 
//�����˲�����
					  
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
	 //��ʼ��
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
//		if(__HAL_DMA_GET_FLAG(&DMASPIRx_Handler,DMA_FLAG_TCIF3_7))//�ȴ�DMA2_Steam7�������
//         {
//                    __HAL_DMA_CLEAR_FLAG(&DMASPIRx_Handler,DMA_FLAG_TCIF3_7);//���DMA2_Steam7������ɱ�־
//                    HAL_SPI_DMAStop(&SPI2_Handler);      //��������Ժ�رմ���DMA
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
				tcp_server_flag|=(1<<7);//������Ҫ����
				
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
		OSSemPost(Sem_Task_ads1299); // �����ź���,�����������������ϵͳ���ȣ������жϷ�����һ��Ҫ��ࡣ
		//EXTI_ClearITPendingBit(EXTI_Line13); // �����־λ
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
         //HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);//�����жϴ����ú���
		OSIntExit();
	//}
}

u8 err;
//led����
void ads1299_task(void *pdata)
{
	(void) pdata;
	Sem_Task_ads1299 = OSSemCreate(0);
	while(1)
	{
		OSSemPend(Sem_Task_ads1299,0,&err);  // �ȴ��ź���
		Recev_Data();
		OSTimeDlyHMSM(0,0,0,1);  //��ʱ1ms
 	}
}
//�жϷ����������Ҫ��������
//��HAL�������е��ⲿ�жϷ�����������ô˺���
//��ADS1299�����ݽ��д���
//u8 t;
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
////u8 inbyte,i,j,k,n=2;
////adc_buf2[0]=0xa0;
//	//adc_buf2[26]=0xc0;
//	//u32 byteCounter=0,channelData[8];
//	//delay_us(2);//��һ��С��ʱ���Է�ֹ����
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

//#define Sample 2000		//��������
//#define BLOCK_SIZE 20	//����һ��arm_fir_f32����Ĳ��������
//#define NUM_TAPS 28

//u32 blocksize = BLOCK_SIZE;
//u32 numBlocks=	Sample/BLOCK_SIZE;

//void arm_fir_f32bs(void)
//{
//	arm_fir_instance_f32 S;
//	float *inputf32,*outputf32;
//	

























