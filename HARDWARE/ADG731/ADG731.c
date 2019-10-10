#include "adg731.h"
#include "spi.h"
#include "delay.h"
#include "usart.h"
#include "stm32f4xx_hal_gpio.h"


#define ADG731_SYNC PCout(9)
#define ADG731_SCLK PCout(10)
#define ADG731_DATA PCout(12)

/*此部分程序实现对adg731的初始化以及IO开关的控制
*/

void Adg731_Init(void)
{
	u8 temp;
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();           //使能GPIOC时钟
    
    //PF6
    GPIO_Initure.Pin=GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_12;            //PC9
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速         
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //初始化CSpin脚
	//SPI3_Init();				//SPI3初始化
	//SPI3_SetSpeed(SPI_BAUDRATEPRESCALER_256); //设置为45M时钟,高速模式     //W25QXX_TYPE=W25QXX_ReadID();	        //读取FLASH ID.
	ADG731_SYNC =1;
	delay_ms(10);
	Adg731_Write_Rg(0x80);
	
}


/*写ADG731寄存器*/
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
		delay_us(2);   //对TEA5767这三个延时都是必须的
		ADG731_SCLK=1;
		delay_us(2); 
		ADG731_SCLK=0;	
		delay_us(2);
    }	
//delay_us(2);	
	//SPI3_WriteByte(res);
	ADG731_SYNC = 1;
}

