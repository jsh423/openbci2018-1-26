#include "spi.h"
#include "tcp_server_demo.h" 
#include "ads1299.h"
#include "malloc.h"
#include <string.h>
//#include "openbci.h"
//////////////////////////////////////////////////////////////////////////////////	 

//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//升级说明
//V1.1 20160116
//新增对SPI2的应用代码
////////////////////////////////////////////////////////////////////////////////// 	 

SPI_HandleTypeDef SPI3_Handler;  //SPI5句柄
SPI_HandleTypeDef SPI2_Handler;  //SPI2句柄
DMA_HandleTypeDef   DMASPIRx_Handler;        //DMA句柄
DMA_HandleTypeDef   DMASPITx_Handler;        //DMA句柄


//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化
//这里针是对SPI5的初始化
void SPI3_Init(void)
{
    SPI3_Handler.Instance=SPI3;                         //SP5
    SPI3_Handler.Init.Mode=SPI_MODE_MASTER;             //设置SPI工作模式，设置为主模式
    SPI3_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI3_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI3_Handler.Init.CLKPolarity=SPI_POLARITY_LOW;    //串行同步时钟的空闲状态为高电平
    SPI3_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI3_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI3_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//定义波特率预分频的值:波特率预分频值为256
    SPI3_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI3_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //关闭TI模式
    SPI3_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//关闭硬件CRC校验
    SPI3_Handler.Init.CRCPolynomial=7;                  //CRC值计算的多项式
    HAL_SPI_Init(&SPI3_Handler);
    
    __HAL_SPI_ENABLE(&SPI3_Handler);                    //使能SPI5
   // SPI5_ReadWriteByte(0Xff);                           //启动传输
}

//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化
//这里针是对SPI2的初始化
void SPI2_Init(void)
{
    SPI2_Handler.Instance=SPI2;                      //SP2
    SPI2_Handler.Init.Mode=SPI_MODE_MASTER;          //设置SPI工作模式，设置为主模式
    SPI2_Handler.Init.Direction=SPI_DIRECTION_2LINES;//设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI2_Handler.Init.DataSize=SPI_DATASIZE_8BIT;    //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI2_Handler.Init.CLKPolarity=SPI_POLARITY_LOW; //串行同步时钟的空闲状态为高电平
    SPI2_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;      //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI2_Handler.Init.NSS=SPI_NSS_SOFT;              //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI2_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//定义波特率预分频的值:波特率预分频值为256
    SPI2_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;     //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI2_Handler.Init.TIMode=SPI_TIMODE_DISABLE;     //关闭TI模式
    SPI2_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//关闭硬件CRC校验
    SPI2_Handler.Init.CRCPolynomial=7;               //CRC值计算的多项式
    HAL_SPI_Init(&SPI2_Handler);
	
    
	//SPI2->CR2 |= 1<<1  ;	        //发送缓冲区DMA使能
	//SPI2->CR2 |= 1<<0  ;	        //接收缓冲区DMA使能
    __HAL_SPI_ENABLE(&SPI2_Handler);                 //使能SPI2
	
    //SPI2_ReadWriteByte(0Xff);                        //启动传输
}
//mem0addr:存储器地址0  将要存储摄像头数据的内存地址(也可以是外设地址)
//mem1addr:存储器地址1  当只使用mem0addr的时候,该值必须为0
//memblen:存储器位宽,可以为:DMA_MDATAALIGN_BYTE/DMA_MDATAALIGN_HALFWORD/DMA_MDATAALIGN_WORD
//meminc:存储器增长方式,可以为:DMA_MINC_ENABLE/DMA_MINC_DISABLE
void BCI_DMA_Init(u8 mem0addr,u8 mem1addr,u8 memsize)
{ 
    __HAL_RCC_DMA1_CLK_ENABLE();                                    //使能DMA2时钟
    __HAL_LINKDMA(&SPI2_Handler,hdmarx,DMASPIRx_Handler);        //将DMA与SPIRX联系起来
	__HAL_LINKDMA(&SPI2_Handler,hdmatx,DMASPITx_Handler);        //将DMA与SPITX联系起来
	
    DMASPIRx_Handler.Instance=DMA1_Stream3;                          //DMA2数据流1                     
    DMASPIRx_Handler.Init.Channel=DMA_CHANNEL_0;                     //通道1
    DMASPIRx_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;            //外设到存储器
    DMASPIRx_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                //外设非增量模式
    DMASPIRx_Handler.Init.MemInc=DMA_MINC_ENABLE;                             //存储器增量模式
    DMASPIRx_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;   //外设数据长度:32位
    DMASPIRx_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;                  //存储器数据长度:8/16/32位
    DMASPIRx_Handler.Init.Mode=DMA_CIRCULAR;                         //使用循环模式 
    DMASPIRx_Handler.Init.Priority=DMA_PRIORITY_HIGH;                //高优先级
    DMASPIRx_Handler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;              //使能FIFO
    DMASPIRx_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_HALFFULL; //使用1/2的FIFO 
    DMASPIRx_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                //存储器突发传输
    DMASPIRx_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;             //外设突发单次传输 
    HAL_DMA_DeInit(&DMASPIRx_Handler);                               //先清除以前的设置
    HAL_DMA_Init(&DMASPIRx_Handler);	                                //初始化DMA
    
	DMASPITx_Handler.Instance=DMA1_Stream4;                          //DMA2数据流1                     
    DMASPITx_Handler.Init.Channel=DMA_CHANNEL_0;                     //通道1
    DMASPITx_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;            //外设到存储器
    DMASPITx_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                //外设非增量模式
    DMASPITx_Handler.Init.MemInc=DMA_MINC_ENABLE;                             //存储器增量模式
    DMASPITx_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;   //外设数据长度:32位
    DMASPITx_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;                  //存储器数据长度:8/16/32位
    DMASPITx_Handler.Init.Mode=DMA_CIRCULAR;                         //使用循环模式 
    DMASPITx_Handler.Init.Priority=DMA_PRIORITY_HIGH;                //高优先级
    DMASPITx_Handler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;              //使能FIFO
    DMASPITx_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_HALFFULL; //使用1/2的FIFO 
    DMASPITx_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                //存储器突发传输
    DMASPITx_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;             //外设突发单次传输 
    HAL_DMA_DeInit(&DMASPITx_Handler);                               //先清除以前的设置
    HAL_DMA_Init(&DMASPITx_Handler);	                                //初始化DMA
    //在开启DMA之前先使用__HAL_UNLOCK()解锁一次DMA,因为HAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
    //这两个函数一开始要先使用__HAL_LOCK()锁定DMA,而函数__HAL_LOCK()会判断当前的DMA状态是否为锁定状态，如果是
    //锁定状态的话就直接返回HAL_BUSY，这样会导致函数HAL_DMA_Statrt()和HAL_DMAEx_MultiBufferStart()后续的DMA配置
    //程序直接被跳过！DMA也就不能正常工作，为了避免这种现象，所以在启动DMA之前先调用__HAL_UNLOC()先解锁一次DMA。
    __HAL_UNLOCK(&DMASPITx_Handler);
//    if(mem1addr==0)    //开启DMA，不使用双缓冲
//    {
//        HAL_DMA_Start(&DMASPITx_Handler,(u32)&SPI2->DR,mem0addr,memsize);
//    }
//    else                //使用双缓冲
   // {
        //HAL_DMAEx_MultiBufferStart(&DMASPIRx_Handler,(u32)&SPI2->DR,mem0addr,mem1addr,memsize);//开启双缓冲
       // __HAL_DMA_ENABLE_IT(&DMASPIRx_Handler,DMA_IT_TC);    //开启传输完成中断
       // HAL_NVIC_SetPriority(DMA1_Stream3_IRQn,4,2);        //DMA中断优先级
       // HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
    //}
}
void (*spi2_rx_callback)(void);//SPI DMA接收回调函数
//DMA2数据流1中断服务函数
//void DMA1_Stream3_IRQHandler(void)
//{
//	//OSIntEnter();
//    if(__HAL_DMA_GET_FLAG(&DMASPIRx_Handler,DMA_FLAG_TCIF3_7)!=RESET)//DMA传输完成
//    {
//        __HAL_DMA_CLEAR_FLAG(&DMASPIRx_Handler,DMA_FLAG_TCIF3_7);//清除DMA传输完成中断标志位
//        spi2_rx_callback();	//执行摄像头接收回调函数,读取数据等操作在这里面处理
//		//tcp_server_flag|=(1<<7);//有数据要发送	//netcam_line_buf1写入FIFO 
//		HAL_SPI_DMAStop(&SPI2_Handler);
//    } 
//	//OSIntExit();
//}
void ADS1299_DMA_Start(void)
{
	//HAL_DMA_Start（DMASPIRx_Handler,netcambuf0,netcambuf1
	__HAL_DMA_ENABLE(&DMASPIRx_Handler); //使能DMA
	__HAL_DMA_ENABLE(&DMASPITx_Handler);
}
void ADS1299_DMA_STOP(void)
{
	__HAL_DMA_DISABLE(&DMASPIRx_Handler);
	__HAL_DMA_DISABLE(&DMASPITx_Handler);
}
//SPI5底层驱动，时钟使能，引脚配置
//此函数会被HAL_SPI_Init()调用
//hspi:SPI句柄
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
	__HAL_RCC_GPIOB_CLK_ENABLE();       //使能GPIOF时钟
    __HAL_RCC_GPIOF_CLK_ENABLE();       //使能GPIOF时钟
	__HAL_RCC_GPIOC_CLK_ENABLE();       //使能GPIOF时钟
    __HAL_RCC_SPI3_CLK_ENABLE();        //使能SPI3时钟
    __HAL_RCC_SPI2_CLK_ENABLE();        //使能SPI2时钟
    
	__HAL_RCC_DMA1_CLK_ENABLE();                                    //使能DMA1时钟
    //SPI5引脚初始化PF7,8,9
//    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
//    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
//    GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
//    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //快速            
//    GPIO_Initure.Alternate=GPIO_AF5_SPI5;           //复用为SPI5
//    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
	
	//SPI2引脚初始化PC2,3
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //快速            
    GPIO_Initure.Alternate=GPIO_AF5_SPI2;           //复用为SPI3
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    //SPI2引脚初始化PB10
    GPIO_Initure.Pin=GPIO_PIN_10;   
	 GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //快速    
    GPIO_Initure.Alternate=GPIO_AF5_SPI2;           //复用为SPI2
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);             //初始化
	
	
	 /* Configure the DMA handler for Transmission process */
  DMASPITx_Handler.Instance                 = DMA1_Stream4;
  
  DMASPITx_Handler.Init.Channel             = DMA_CHANNEL_0;
  DMASPITx_Handler.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  DMASPITx_Handler.Init.PeriphInc           = DMA_PINC_DISABLE;
  DMASPITx_Handler.Init.MemInc              = DMA_MINC_ENABLE;
  DMASPITx_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  DMASPITx_Handler.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  DMASPITx_Handler.Init.Mode                = DMA_NORMAL;
  DMASPITx_Handler.Init.Priority            = DMA_PRIORITY_LOW;
  DMASPITx_Handler.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
  DMASPITx_Handler.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  DMASPITx_Handler.Init.MemBurst            = DMA_MBURST_INC4;
  DMASPITx_Handler.Init.PeriphBurst         = DMA_PBURST_INC4;
  
  HAL_DMA_Init(&DMASPITx_Handler);   
  
  /* Associate the initialized DMA handle to the the SPI handle */
  __HAL_LINKDMA(hspi, hdmatx, DMASPITx_Handler);
    
  /* Configure the DMA handler for Transmission process */
  DMASPIRx_Handler.Instance                 = DMA1_Stream3;
  
  DMASPIRx_Handler.Init.Channel             = DMA_CHANNEL_0;
  DMASPIRx_Handler.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  DMASPIRx_Handler.Init.PeriphInc           = DMA_PINC_DISABLE;
  DMASPIRx_Handler.Init.MemInc              = DMA_MINC_ENABLE;
  DMASPIRx_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  DMASPIRx_Handler.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  DMASPIRx_Handler.Init.Mode                = DMA_NORMAL;
  DMASPIRx_Handler.Init.Priority            = DMA_PRIORITY_HIGH;
  DMASPIRx_Handler.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
  DMASPIRx_Handler.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  DMASPIRx_Handler.Init.MemBurst            = DMA_MBURST_INC4;
  DMASPIRx_Handler.Init.PeriphBurst         = DMA_PBURST_INC4; 

  HAL_DMA_Init(&DMASPIRx_Handler);
    
  /* Associate the initialized DMA handle to the the SPI handle */
  __HAL_LINKDMA(hspi, hdmarx, DMASPIRx_Handler);
    
  /*##-4- Configure the NVIC for DMA #########################################*/ 
  /* NVIC configuration for DMA transfer complete interrupt (SPI3_TX) */
  HAL_NVIC_SetPriority(SPIx_DMA_TX_IRQn, 2, 1);
  HAL_NVIC_EnableIRQ(SPIx_DMA_TX_IRQn);
    
  /* NVIC configuration for DMA transfer complete interrupt (SPI3_RX) */
  HAL_NVIC_SetPriority(SPIx_DMA_RX_IRQn, 2, 0);   
  HAL_NVIC_EnableIRQ(SPIx_DMA_RX_IRQn);
	
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  


  /*##-1- Reset peripherals ##################################################*/
  SPIx_FORCE_RESET();
  SPIx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* Configure SPI SCK as alternate function  */
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);
  /* Configure SPI MISO as alternate function  */
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2);
  /* Configure SPI MOSI as alternate function  */
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3);
   
  /*##-3- Disable the DMA Streams ############################################*/
  /* De-Initialize the DMA Stream associate to transmission process */
  HAL_DMA_DeInit(&DMASPITx_Handler); 
  /* De-Initialize the DMA Stream associate to reception process */
  HAL_DMA_DeInit(&DMASPIRx_Handler);
  
  /*##-4- Disable the NVIC for DMA ###########################################*/
  HAL_NVIC_DisableIRQ(SPIx_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(SPIx_DMA_RX_IRQn);
}
//SPI速度设置函数
//SPI速度=fAPB1/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
//fAPB1时钟一般为45Mhz：
void SPI3_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&SPI3_Handler);            //关闭SPI
    SPI3_Handler.Instance->CR1&=0XFFC7;          //位3-5清零，用来设置波特率
    SPI3_Handler.Instance->CR1|=SPI_BaudRatePrescaler;//设置SPI速度
    __HAL_SPI_ENABLE(&SPI3_Handler);             //使能SPI
    
}

//SPI速度设置函数
//SPI速度=fAPB1/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
//fAPB1时钟一般为45Mhz：
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&SPI2_Handler);            //关闭SPI
    SPI2_Handler.Instance->CR1&=0XFFC7;          //位3-5清零，用来设置波特率
    SPI2_Handler.Instance->CR1|=SPI_BaudRatePrescaler;//设置SPI速度
    __HAL_SPI_ENABLE(&SPI2_Handler);             //使能SPI
    
}

//SPI5 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI3_WriteByte(u8 TxData)
{
    u8 Rxdata;
	HAL_SPI_Transmit(&SPI3_Handler,&TxData,1,1000);
    //HAL_SPI_TransmitReceive(&SPI3_Handler,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;          		    //返回收到的数据		
}

//SPI2 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
	//u32 res;
    HAL_SPI_TransmitReceive(&SPI2_Handler,&TxData,&Rxdata,1, 1000);      
		//res=(u32)Rxdata;
 	return Rxdata;          		    //返回收到的数据		
}


	//flag_wifi=1;
	//len_wifi=28;
//}

 void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  /* Turn LED5 on: Transfer error in reception/transmission process */
  //BSP_LED_On(LED5); 
}

//void SPI2_IRQHandler(void)
//{
//  /* USER CODE BEGIN SPI2_IRQn 0 */

//  /* USER CODE END SPI2_IRQn 0 */
//  
//	ADS1299_CS0=1;
//	tcp_server_flag|=(1<<7);//有数据要发送
//	tcp_server_sendbuf=netcamfifobuf;
//	len=28;
//	HAL_SPI_IRQHandler(&SPI2_Handler);
//  /* USER CODE BEGIN SPI2_IRQn 1 */

//  /* USER CODE END SPI2_IRQn 1 */
//}

////SPI2 读写一个字节
////TxData:要写入的字节
////返回值:读取到的字节
//u8 SPI2_ReadWriteData(u8 *TxData)
//{
//    u8 Rxdata;
//    HAL_SPI_TransmitReceive(&SPI2_Handler,&TxData,&Rxdata,1, 1000);       
// 	return Rxdata;          		    //返回收到的数据		
//}
////SPI2 DMA接受一串数据
////RxData :接受的字节
////len:接受到的字节长度
//u8 SPI2_DMA_ReadData(u8 RxData,u8 len)
//{
//	u8 TxData;
//	HAL_SPI_TransmitReceive_DMA(&SPI2_Handler,&TxData,&RxData,1);
//	return RxData;
//}
