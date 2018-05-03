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
//#include "arm_math.h"
//#include "dma.h"
//////////////////////////////////////////////////////////////////////////////////	 
//
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
u8 check=0;
u8 TxData0[28]={0x12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //���͵�ַ
//u8 TxData1[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//u8 RxData[4][4]; //���ܵ�ַ
u8 sps=0x05;
u8 parameter[10];
u8 Flag_adc;
//u32 stat;
u8 index1=0;
u8 res3;
u8 adc_buf2[101];
u8 stat[16];
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

//��ȡ�Ĵ�������
u8 ADS1299_PREG(u8 reg)
{
	  u8 Byte;	
   	ADS1299_CS1=0;                 //ʹ��SPI����
//delay_us(200);
  	SPI2_ReadWriteByte(0X00|0X20);//���ͼĴ�����
	
  	SPI2_ReadWriteByte(0X00);      //д��Ĵ�����ֵ
	Byte=SPI2_ReadWriteByte(0);
//	status=SPI2_ReadWriteByte(0);
	//delay_us(2);
  	ADS1299_CS1=1;                 //��ֹSPI����	   
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
void ADS1299_WREG_Single(u8 n,u8 address,u8 value)
{
	u8 i;
	
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
//��ʼ��24L01��IO��
void ADS1299_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOG_CLK_ENABLE();			//����GPIOGʱ��
	__HAL_RCC_GPIOD_CLK_ENABLE();			//����GPIOIʱ��

	GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5; //PG2��3��4��5ADS1299Ƭѡ�ź�
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(GPIOG,&GPIO_Initure);     //��ʼ��

	GPIO_Initure.Pin=GPIO_PIN_12; //PD12 ADS1299��ʼ�ź�
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);     //��ʼ��
	//     

	SPI2_Init();    		                //��ʼ��SPI2  /
	delay_ms(50);

	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_16); //spi�ٶ�Ϊ11.25Mhz��24L01�����SPIʱ��Ϊ10Mhz,�����һ��û��ϵ�� 
	ADS1299_START=0;
	ADS1299_CS0=1; //���ε�һƬADS1299,���Ե�оƬ
	ADS1299_CS1=1;
	ADS1299_CS2=1;
	ADS1299_CS3=1;
	ADS1299_Command(_WAKEUP);
	ADS1299_Command(_RESET);
	
	delay_ms(5); 
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
	ADS1299_WREG(CONFIG3,0xe0);//�����ڲ���׼
	
	delay_ms(10);
	ADS1299_WREG(CONFIG1,(0XD0|sps));	//���ö�ض�ģʽ��ͨ������250SPS 1k	 
	
	delay_ms(5);
}
		
	
void ADS1299_IT(void)
{
		  GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOIʱ��
	
	
	GPIO_Initure.Pin=GPIO_PIN_12;               //PB12
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;
	//GPIO_MODE_IT_FALLING;     //�½��ش���
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	 //�ж���13-PB12
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,1,1);   //��ռ���ȼ�Ϊ2�������ȼ�Ϊ1
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //ʹ���ж���13 
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
//���ADS1299�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
u8 ADS1299_Check(void)
{
	
	check=ADS1299_PREG(ID);

	//if(check==0x3e) 
	{

		ADS1299_WREG(CH1SET,0X60);//��һͨ�����ö�·������ϵͳ����
		ADS1299_WREG(CONFIG2,0XD1);//�����ź����ڲ�����
		//ADS1299_WREG(CONFIG3,0XFE);//�����ڲ���׼
		ADS1299_WREG_Single(0,CONFIG3,0XFE);
		
		ADS1299_WREG(LOFF,0x00);//DC-Lead-off���
		//ADS1299_START=1;
		ADS1299_WREG(CH2SET,0x60);
		ADS1299_WREG(CH3SET,0x60);
		ADS1299_WREG(CH4SET,0X60);//��һͨ��������ͨ����
		ADS1299_WREG(CH5SET,0x60);
		ADS1299_WREG(CH6SET,0x60);
		ADS1299_WREG(CH7SET,0x60);
		ADS1299_WREG(CH8SET,0x60);
		ADS1299_WREG(BIAS_SENSP,0Xff);//����ֱ��ƫ��
		ADS1299_WREG(BIAS_SENSN,0XFF);
		
		ADS1299_WREG(LOFF_SENSP,0xff);//�ر��迹����
		ADS1299_WREG(LOFF_SENSP,0xff);
		ADS1299_WREG_Single(0,MISC1,0X20);
		ADS1299_WREG_Single(1,MISC1,0X20);
		ADS1299_WREG_Single(2,MISC1,0X20);
		
		//ADS1299_WREG(MISC1,0X20);//SRB1�պ�
		ADS1299_WREG(CONFIG4,2);//����leadoff�Ƚ���
		//ADS1299_RDATAC();//����ģʽ
		ADS1299_Command(_RDATA);//�����ȡ����ģʽ
		SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi�ٶ�Ϊ11.25Mhz��24L01�����SPIʱ��Ϊ10Mhz,�����һ��û��ϵ�� 
			delay_ms(1);
		//ADS1299_CS0=0;
		return 0;
	}
		
	
	//else return 1;

}	 

void Set_Sps(u8 i)
{
				ADS1299_START=0;//�Ƚ�ADS1299���ݲ������ص�
				ADS1299_SDATAC();//�˳���������ģʽ���Ա���мĴ���������//��������ģʽ
				ADS1299_WREG(0X01,(0X90|sps));	//���ö�ض�ģʽ��ͨ������250SPS 1k//���ò�����
				ADS1299_Command(0x12);//�����ȡ����ģʽ//�˳�����ģʽ
}

//u8 regdata[18];

				  

//�ݲ����м�ֵ 
 

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

//#endif

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
	//tcp_server_sendbuf=buf3;'
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
		{
		
		memcpy(&adc_buf2[k*24+4],&buf3[4],24);
		memcpy(&stat[k*3],&buf3[1],3);
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
			Num=netcam_fifo_write(&adc_buf2[0]);
			if((Num>0)&&(~(tcp_server_flag&1<<7)))
			{
				tcp_server_flag|=(1<<7);//������Ҫ����
				
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
		//OSSemPost(Sem_Task_ads1299); // �����ź���,�����������������ϵͳ���ȣ������жϷ�����һ��Ҫ��ࡣ
		//EXTI_ClearITPendingBit(EXTI_Line13); // �����־λ
		Recev_Data();
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
         //HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);//�����жϴ����ú���
		//OSIntExit();
	//}
}

u8 err;
//led����
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
//		//OSSemPend(Sem_Task_ads1299,0,&err);  // �ȴ��ź���
//		//Recev_Data();
//		
//			
//		OSTimeDlyHMSM(0,0,0,20);  //��ʱ500ms
// 	}
//}
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

		
			
	
}
}

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

























