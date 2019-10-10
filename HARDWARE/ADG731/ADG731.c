#include "adg731.h"
#include "spi.h"
#include "delay.h"
#include "usart.h"
#include "stm32f4xx_hal_gpio.h"


#define ADG731_SYNC PCout(9)
#define ADG731_SCLK PCout(10)
#define ADG731_DATA PCout(12)

/*�˲��ֳ���ʵ�ֶ�adg731�ĳ�ʼ���Լ�IO���صĿ���
*/

void Adg731_Init(void)
{
	u8 temp;
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();           //ʹ��GPIOCʱ��
    
    //PF6
    GPIO_Initure.Pin=GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_12;            //PC9
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����         
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //��ʼ��CSpin��
	//SPI3_Init();				//SPI3��ʼ��
	//SPI3_SetSpeed(SPI_BAUDRATEPRESCALER_256); //����Ϊ45Mʱ��,����ģʽ     //W25QXX_TYPE=W25QXX_ReadID();	        //��ȡFLASH ID.
	ADG731_SYNC =1;
	delay_ms(10);
	Adg731_Write_Rg(0x80);
	
}


/*дADG731�Ĵ���*/
void Adg731_Write_Rg(u8 txd)
{
	u8 t;
	ADG731_SCLK=1;
	delay_us(2);
	ADG731_SCLK=0;
	delay_us(2);
	ADG731_SYNC =0;
	//delay_us(2);
	 for(t=0;t<8;t++)
    {   
		
        ADG731_DATA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		ADG731_SCLK=1;
		delay_us(2); 
		ADG731_SCLK=0;	
		delay_us(2);
    }	
//delay_us(2);	
	//SPI3_WriteByte(res);
	ADG731_SYNC = 1;
}

