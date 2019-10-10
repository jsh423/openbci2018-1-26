#include "ad5933.h"
#include "myiic.h"
#include "delay.h"
#include "math.h"

#define COMMAND_START_FRE 0X82
//#define COMMAND_
u8 res5[20];
//struct AD5933_Reg_Set {
//	u8 Fre_start1;
//	u8 Fre_Start2;
//	u8 Fre_Start3;
//	u8 Fre_Incre1;
//	u8 Fre_Incre2;
//	u8 Fre_Incre3;
//	u8 Increment1;
//	u8 Increment2;
//	u8 T_creat1;
//	u8 T_creat2;
//	u8 len;
//} AD5933_Reg_Set = {

//}
void AD5933_Set_Point(u8 reg_addr)
{
	IIC_Start();
	
	IIC_Send_Byte(AD5933_ADDR | 0);//����������ַ+д����
	IIC_Wait_Ack();			//�ȴ�Ӧ��
	IIC_Send_Byte(0xb0);	//�������õ�ַ ָ������
	IIC_Wait_Ack();
	IIC_Send_Byte(reg_addr);
	IIC_Wait_Ack();
	IIC_Stop();
}
	//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������

u8 AD5933_Read_Byte(u8 addr)
{
	u8 res;
	AD5933_Set_Point(addr);
	//delay_us(5);
	IIC_Start();
	IIC_Send_Byte((AD5933_ADDR) | 1); //����������ַ+������
	IIC_Wait_Ack();             //�ȴ�Ӧ��
	//IIC_Send_Byte(addr);
	//IIC_Wait_Ack();
	res = IIC_Read_Byte(0);		//������,����nACK  
	IIC_Stop();                 //����һ��ֹͣ����
	return res;
}
/*��AD5933���в��ԣ�ȷ���Ƿ��л�Ӧ*/
u8 reg=0;
u8 AD5933_Init(void)
{
	AD5933_WRITE_COMMAND(0x84,0x99);
	delay_us(5);
	AD5933_WRITE_COMMAND(0x83,0x99);
	delay_us(5);
	AD5933_WRITE_COMMAND(0x82,0x19);
	//��������Ƶ��Ϊ�� �������Ĵ���
	
	delay_us(5);
	AD5933_WRITE_COMMAND(0x87,0x0);
	delay_us(5);
	AD5933_WRITE_COMMAND(0x86,0x0);
	delay_us(5);
	AD5933_WRITE_COMMAND(0X85,0X00);
	
	//����ɨ���е�Ƶ�ʵ���Ϊ10
	//delay_us(5);
	AD5933_WRITE_COMMAND(0X89,0X0A);
	AD5933_WRITE_COMMAND(0x88,0x00);
	
	// program 15 output cycles at each frequency before a adc conversion
	AD5933_WRITE_COMMAND(0X8B,0X0f);
	AD5933_WRITE_COMMAND(0x8A,0x00);
	
	// Transmit to CONTROL register 
   // place the AD5933 in standby mode
	AD5933_WRITE_COMMAND(0X80,0xb0);
  
	
	delay_us(5);
	AD5933_WRITE_COMMAND(0x81,0x10);//��λAD5933
	delay_us(5);
	AD5933_WRITE_COMMAND(0X81,0x08);//�ⲿʱ��
	
	//AD5933_WRITE_COMMAND(0x81,0x00);
	delay_us(5);
	
	AD5933_WRITE_COMMAND(0x80,0x11);//����ʼƵ�ʳ�ʼ��2Vp-p,PGA=X1
	delay_us(30);
	
	//IIC_Start();
//    IIC_Send_Byte(AD5933_ADDR|0X00);//����������ַ+д����
//    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
//    {
//        IIC_Stop();
//        return 1;
//    }
	//IIC_Send_Byte(0XA0);
	//AD5933_WRITE_COMMAND(0x81,0x11);
	//delay_us(5);
	//AD5933_Set_Point(0x81);
	//AD5933_WRITE_COMMAND(0x82,0x11);
	//delay_us(5);
	//reg=AD5933_Read_Byte(0);
	//IIC_Stop();
	delay_us(5);
	//AD5933_WRITE_COMMAND(0x83,0x11);
//AD5933_Set_Point(0x81);
	//AD5933_Read_Data(1);
//	AD5933_WRITE_COMMAND(0x81,0x08);
//	delay_us(5);
//	AD5933_Set_Point(0x81);
//	reg=AD5933_Read_Byte(0x83);
	return 1;
	
	//AD5933_WRITE_COMMAND(COMMAND_START_FRE,0X00);//���ó�ʼ
}
void AD5933_Start(void)
{
	//delay_us(5);
	//������ʼƵ��1kHz��0x199999
	AD5933_WRITE_COMMAND(0x84,0x05);
	delay_us(5);
	AD5933_WRITE_COMMAND(0x83,0xFA);
	delay_us(5);
	AD5933_WRITE_COMMAND(0x82,0x00);
	//��������Ƶ��Ϊ�� �������Ĵ���
	
	delay_us(5);
	AD5933_WRITE_COMMAND(0x87,0x0);
	delay_us(5);
	AD5933_WRITE_COMMAND(0x86,0x0);
	delay_us(5);
	AD5933_WRITE_COMMAND(0X85,0X00);
	
	//����ɨ���е�Ƶ�ʵ���Ϊ10
	//delay_us(5);
	AD5933_WRITE_COMMAND(0X89,0XFF);
	AD5933_WRITE_COMMAND(0x88,0x01);
	
	// program 15 output cycles at each frequency before a adc conversion
	AD5933_WRITE_COMMAND(0X8B,0X00);
	AD5933_WRITE_COMMAND(0x8A,0x00);
	
//	// Transmit to CONTROL register 
//   // place the AD5933 in standby mode
	AD5933_WRITE_COMMAND(0X80,0xb1);
  
	delay_us(5);
	AD5933_WRITE_COMMAND(0X81,0x08);//�ⲿʱ��
	delay_us(5);
	AD5933_WRITE_COMMAND(0x81,0x10);//��λAD5933
	
	
	//AD5933_WRITE_COMMAND(0x81,0x00);
	delay_us(5);
	
	AD5933_WRITE_COMMAND(0x80,0x11);//����ʼƵ�ʳ�ʼ��2Vp-p,PGA=X1
	delay_us(30);
	//AD5933_Set_Point(0x89);
	// Choose range 1 (2vp-p, 1.6v) PGA = x1
	//AD5933_WRITE_COMMAND(0X80,0X01);//PGA=X1
	// program 15 output cycles at each frequency before a adc conversion
	AD5933_WRITE_COMMAND(0X8B,0XFF);
	delay_us(5);
	AD5933_WRITE_COMMAND(0x8A,0x03);
	delay_us(5);
//AD5933_Read_Data(1);
	//reg=AD5933_Read_Byte(0x80);
	AD5933_WRITE_COMMAND(0x80,0x21);//����Ƶ��ɨ��
	delay_us(25);
	
}
u8 reg1;
u8 status;
u8 Tm1,Tm2;
//u32 Tm;
u32 wendu(void)
{
	status=0;
	
	u32 Tm;
	AD5933_WRITE_COMMAND(0X81,0X10);
	delay_us(5);
	AD5933_WRITE_COMMAND(0X81,0X00);
	delay_us(5);
	AD5933_WRITE_COMMAND(0X80,0X91);
	//delay_us(5);
	//AD5933_Set_Point(0x8f);
	//delay_us(5);
	do
	{
		//AD5933_Set_Point(0x8f);
		//delay_us(5);
		//reg1=AD5933_Read_Byte(0x8f);
		status=AD5933_Read_Byte(0x8f);
	}while(!(status&0x01));
//	AD5933_Set_Point(0x89);
	//reg1=AD5933_Read_Byte(0x8F);
	Tm1=AD5933_Read_Byte(0x92);
	//Tm=(Tm1<<8);
//AD5933_Set_Point(0x93);
	Tm2=AD5933_Read_Byte(0x93);
	//Tm=Tm&0x3fff;
	//Tm=Tm/32;
	//Tm+=Tm2;
	reg=AD5933_Read_Byte(0X80);
	reg1=AD5933_Read_Byte(0x8F);
	return Tm1;
	
}
	
u8 AD5933_WRITE_COMMAND(u8 Command,u8 data)
{
	IIC_Start();
    IIC_Send_Byte((AD5933_ADDR)|0X00);//����������ַ+д����
    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
	delay_us(2);
	IIC_Send_Byte(Command);
	if(IIC_Wait_Ack())          //�ȴ�Ӧ��
		
    {
        IIC_Stop();
        return 1;
    }
	delay_us(2);
	IIC_Send_Byte(data);
	if(IIC_Wait_Ack())          //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
	IIC_Stop();
	return 0;
}







//
int32_t Re,Im;
u32 valu;

#if 1
u32 AD5933_RES(void)
{
	u8 Status=0,x=0,y=0;
	u16 Res;
	u16 tem;
	//u16 valu=0;
	u8 res;
	do
	{
		Status=AD5933_Read_Byte(0x8F);
	}
	while(!(Status&0x02));
	//�����迹��ʵ��
	Re=AD5933_Read_Byte(0x94);
	delay_us(5);
	Re=Re<<8;
	Re+=AD5933_Read_Byte(0X95);
	//�����迹���鲿
	Im=AD5933_Read_Byte(0X96);
	delay_us(5);
	Im=Im<<8;
	Im+=AD5933_Read_Byte(0X97);
	delay_us(5); 
//	tem=AD5933_Read_Byte(0x92);
//	tem=tem<<8;
//	tem+=AD5933_Read_Byte(0X93);
	//res=AD5933_Read_Byte(0X83);
	if(Re&0x8000)
	{
		Re=(Re-0x10000)*(-1);
		x=1;
	}
	if(Im&0x8000)
	{
		Im=(Im-0x10000)*(-1);
		y=1;
	}
	valu=sqrt(Re*Re+Im*Im);
	Res=(u16)(K/valu);
	return Res;
}
#endif	
		
		
		
/*
��AD5933���п�д��

*/
/*
void AD5933_Write_Data(u8 addr,u8 len)
{
	u8 i;
	u8 res[len];
	IIC_Start();
	IIC_Send_Byte((addr << 1) | 0);//����������ַ+дָ��
		IIC_Wait_Ack();
	IIC_Send_Byte(0xa0);
	IIC_Wait_Ack();
	IIC_Send_Byte(len);

	IIC_Wait_Ack();
	for (i=0,i<len,i++)
	{
		IIC_Send_Byte(p->i);
	}
}



/*
��AD5933���ݽ��п��ȡ����
*/

void AD5933_Read_Data(u8 len)
{
	
	u8 i;

	IIC_Start();
	IIC_Send_Byte((AD5933_ADDR) | 0);
	IIC_Wait_Ack();
	IIC_Send_Byte(0xa1);//���Ϳ��ȡָ��
	IIC_Wait_Ack();
	IIC_Send_Byte(len);//���ߴӻ�Ҫ��ȡ���ٸ��ֽ�
	IIC_Wait_Ack();
	IIC_Stop();
	
	IIC_Start();
	IIC_Send_Byte((AD5933_ADDR) | 1);//����������ȡָ��	
	for (i=0;i<len-1;i++)
	{
		//IIC_Wait_Ack();
		res5[i] = IIC_Read_Byte(1);
		
	}
	res5[len-1]=IIC_Read_Byte(0);
	//IIC_Wait_Ack();
	IIC_Stop();
}
/*	
u16 ReadData(u8 addr)
{
	u8 data_a;
	 
	AD5933_WRITE_COMMAND(0XB0,addr);
	delay_us(5);
	data_a=


*/