#include "spi.h"
#include "tcp_server_demo.h" 
#include "ads1299.h"
#include "malloc.h"
#include <string.h>
//#include "openbci.h"
//////////////////////////////////////////////////////////////////////////////////	 

//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//����˵��
//V1.1 20160116
//������SPI2��Ӧ�ô���
////////////////////////////////////////////////////////////////////////////////// 	 

SPI_HandleTypeDef SPI3_Handler;  //SPI5���
SPI_HandleTypeDef SPI2_Handler;  //SPI2���
DMA_HandleTypeDef   DMASPIRx_Handler;        //DMA���
DMA_HandleTypeDef   DMASPITx_Handler;        //DMA���


//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ 						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI5�ĳ�ʼ��
void SPI3_Init(void)
{
    SPI3_Handler.Instance=SPI3;                         //SP5
    SPI3_Handler.Init.Mode=SPI_MODE_MASTER;             //����SPI����ģʽ������Ϊ��ģʽ
    SPI3_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ
    SPI3_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI3_Handler.Init.CLKPolarity=SPI_POLARITY_LOW;    //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI3_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI3_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI3_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI3_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI3_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //�ر�TIģʽ
    SPI3_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//�ر�Ӳ��CRCУ��
    SPI3_Handler.Init.CRCPolynomial=7;                  //CRCֵ����Ķ���ʽ
    HAL_SPI_Init(&SPI3_Handler);
    
    __HAL_SPI_ENABLE(&SPI3_Handler);                    //ʹ��SPI5
   // SPI5_ReadWriteByte(0Xff);                           //��������
}

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ 						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��
void SPI2_Init(void)
{
    SPI2_Handler.Instance=SPI2;                      //SP2
    SPI2_Handler.Init.Mode=SPI_MODE_MASTER;          //����SPI����ģʽ������Ϊ��ģʽ
    SPI2_Handler.Init.Direction=SPI_DIRECTION_2LINES;//����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ
    SPI2_Handler.Init.DataSize=SPI_DATASIZE_8BIT;    //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI2_Handler.Init.CLKPolarity=SPI_POLARITY_LOW; //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI2_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;      //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI2_Handler.Init.NSS=SPI_NSS_SOFT;              //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI2_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI2_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;     //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI2_Handler.Init.TIMode=SPI_TIMODE_DISABLE;     //�ر�TIģʽ
    SPI2_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//�ر�Ӳ��CRCУ��
    SPI2_Handler.Init.CRCPolynomial=7;               //CRCֵ����Ķ���ʽ
    HAL_SPI_Init(&SPI2_Handler);
	
    
	//SPI2->CR2 |= 1<<1  ;	        //���ͻ�����DMAʹ��
	//SPI2->CR2 |= 1<<0  ;	        //���ջ�����DMAʹ��
    __HAL_SPI_ENABLE(&SPI2_Handler);                 //ʹ��SPI2
	
    //SPI2_ReadWriteByte(0Xff);                        //��������
}
//mem0addr:�洢����ַ0  ��Ҫ�洢����ͷ���ݵ��ڴ��ַ(Ҳ�����������ַ)
//mem1addr:�洢����ַ1  ��ֻʹ��mem0addr��ʱ��,��ֵ����Ϊ0
//memblen:�洢��λ��,����Ϊ:DMA_MDATAALIGN_BYTE/DMA_MDATAALIGN_HALFWORD/DMA_MDATAALIGN_WORD
//meminc:�洢��������ʽ,����Ϊ:DMA_MINC_ENABLE/DMA_MINC_DISABLE
void BCI_DMA_Init(u8 mem0addr,u8 mem1addr,u8 memsize)
{ 
    __HAL_RCC_DMA1_CLK_ENABLE();                                    //ʹ��DMA2ʱ��
    __HAL_LINKDMA(&SPI2_Handler,hdmarx,DMASPIRx_Handler);        //��DMA��SPIRX��ϵ����
	__HAL_LINKDMA(&SPI2_Handler,hdmatx,DMASPITx_Handler);        //��DMA��SPITX��ϵ����
	
    DMASPIRx_Handler.Instance=DMA1_Stream3;                          //DMA2������1                     
    DMASPIRx_Handler.Init.Channel=DMA_CHANNEL_0;                     //ͨ��1
    DMASPIRx_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;            //���赽�洢��
    DMASPIRx_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                //���������ģʽ
    DMASPIRx_Handler.Init.MemInc=DMA_MINC_ENABLE;                             //�洢������ģʽ
    DMASPIRx_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;   //�������ݳ���:32λ
    DMASPIRx_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;                  //�洢�����ݳ���:8/16/32λ
    DMASPIRx_Handler.Init.Mode=DMA_CIRCULAR;                         //ʹ��ѭ��ģʽ 
    DMASPIRx_Handler.Init.Priority=DMA_PRIORITY_HIGH;                //�����ȼ�
    DMASPIRx_Handler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;              //ʹ��FIFO
    DMASPIRx_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_HALFFULL; //ʹ��1/2��FIFO 
    DMASPIRx_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                //�洢��ͻ������
    DMASPIRx_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;             //����ͻ�����δ��� 
    HAL_DMA_DeInit(&DMASPIRx_Handler);                               //�������ǰ������
    HAL_DMA_Init(&DMASPIRx_Handler);	                                //��ʼ��DMA
    
	DMASPITx_Handler.Instance=DMA1_Stream4;                          //DMA2������1                     
    DMASPITx_Handler.Init.Channel=DMA_CHANNEL_0;                     //ͨ��1
    DMASPITx_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;            //���赽�洢��
    DMASPITx_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                //���������ģʽ
    DMASPITx_Handler.Init.MemInc=DMA_MINC_ENABLE;                             //�洢������ģʽ
    DMASPITx_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;   //�������ݳ���:32λ
    DMASPITx_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;                  //�洢�����ݳ���:8/16/32λ
    DMASPITx_Handler.Init.Mode=DMA_CIRCULAR;                         //ʹ��ѭ��ģʽ 
    DMASPITx_Handler.Init.Priority=DMA_PRIORITY_HIGH;                //�����ȼ�
    DMASPITx_Handler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;              //ʹ��FIFO
    DMASPITx_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_HALFFULL; //ʹ��1/2��FIFO 
    DMASPITx_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                //�洢��ͻ������
    DMASPITx_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;             //����ͻ�����δ��� 
    HAL_DMA_DeInit(&DMASPITx_Handler);                               //�������ǰ������
    HAL_DMA_Init(&DMASPITx_Handler);	                                //��ʼ��DMA
    //�ڿ���DMA֮ǰ��ʹ��__HAL_UNLOCK()����һ��DMA,��ΪHAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
    //����������һ��ʼҪ��ʹ��__HAL_LOCK()����DMA,������__HAL_LOCK()���жϵ�ǰ��DMA״̬�Ƿ�Ϊ����״̬�������
    //����״̬�Ļ���ֱ�ӷ���HAL_BUSY�������ᵼ�º���HAL_DMA_Statrt()��HAL_DMAEx_MultiBufferStart()������DMA����
    //����ֱ�ӱ�������DMAҲ�Ͳ�������������Ϊ�˱���������������������DMA֮ǰ�ȵ���__HAL_UNLOC()�Ƚ���һ��DMA��
    __HAL_UNLOCK(&DMASPITx_Handler);
//    if(mem1addr==0)    //����DMA����ʹ��˫����
//    {
//        HAL_DMA_Start(&DMASPITx_Handler,(u32)&SPI2->DR,mem0addr,memsize);
//    }
//    else                //ʹ��˫����
   // {
        //HAL_DMAEx_MultiBufferStart(&DMASPIRx_Handler,(u32)&SPI2->DR,mem0addr,mem1addr,memsize);//����˫����
       // __HAL_DMA_ENABLE_IT(&DMASPIRx_Handler,DMA_IT_TC);    //������������ж�
       // HAL_NVIC_SetPriority(DMA1_Stream3_IRQn,4,2);        //DMA�ж����ȼ�
       // HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
    //}
}
void (*spi2_rx_callback)(void);//SPI DMA���ջص�����
//DMA2������1�жϷ�����
//void DMA1_Stream3_IRQHandler(void)
//{
//	//OSIntEnter();
//    if(__HAL_DMA_GET_FLAG(&DMASPIRx_Handler,DMA_FLAG_TCIF3_7)!=RESET)//DMA�������
//    {
//        __HAL_DMA_CLEAR_FLAG(&DMASPIRx_Handler,DMA_FLAG_TCIF3_7);//���DMA��������жϱ�־λ
//        spi2_rx_callback();	//ִ������ͷ���ջص�����,��ȡ���ݵȲ����������洦��
//		//tcp_server_flag|=(1<<7);//������Ҫ����	//netcam_line_buf1д��FIFO 
//		HAL_SPI_DMAStop(&SPI2_Handler);
//    } 
//	//OSIntExit();
//}
void ADS1299_DMA_Start(void)
{
	//HAL_DMA_Start��DMASPIRx_Handler,netcambuf0,netcambuf1
	__HAL_DMA_ENABLE(&DMASPIRx_Handler); //ʹ��DMA
	__HAL_DMA_ENABLE(&DMASPITx_Handler);
}
void ADS1299_DMA_STOP(void)
{
	__HAL_DMA_DISABLE(&DMASPIRx_Handler);
	__HAL_DMA_DISABLE(&DMASPITx_Handler);
}
//SPI5�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_SPI_Init()����
//hspi:SPI���
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
	__HAL_RCC_GPIOB_CLK_ENABLE();       //ʹ��GPIOFʱ��
    __HAL_RCC_GPIOF_CLK_ENABLE();       //ʹ��GPIOFʱ��
	__HAL_RCC_GPIOC_CLK_ENABLE();       //ʹ��GPIOFʱ��
    __HAL_RCC_SPI3_CLK_ENABLE();        //ʹ��SPI3ʱ��
    __HAL_RCC_SPI2_CLK_ENABLE();        //ʹ��SPI2ʱ��
    
	__HAL_RCC_DMA1_CLK_ENABLE();                                    //ʹ��DMA1ʱ��
    //SPI5���ų�ʼ��PF7,8,9
//    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
//    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //�����������
//    GPIO_Initure.Pull=GPIO_PULLUP;                  //����
//    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //����            
//    GPIO_Initure.Alternate=GPIO_AF5_SPI5;           //����ΪSPI5
//    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
	
	//SPI2���ų�ʼ��PC2,3
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //�����������
    GPIO_Initure.Pull=GPIO_PULLUP;                  //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //����            
    GPIO_Initure.Alternate=GPIO_AF5_SPI2;           //����ΪSPI3
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    //SPI2���ų�ʼ��PB10
    GPIO_Initure.Pin=GPIO_PIN_10;   
	 GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //�����������
    GPIO_Initure.Pull=GPIO_PULLUP;                  //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;             //����    
    GPIO_Initure.Alternate=GPIO_AF5_SPI2;           //����ΪSPI2
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);             //��ʼ��
	
	
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
//SPI�ٶ����ú���
//SPI�ٶ�=fAPB1/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
//fAPB1ʱ��һ��Ϊ45Mhz��
void SPI3_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
    __HAL_SPI_DISABLE(&SPI3_Handler);            //�ر�SPI
    SPI3_Handler.Instance->CR1&=0XFFC7;          //λ3-5���㣬�������ò�����
    SPI3_Handler.Instance->CR1|=SPI_BaudRatePrescaler;//����SPI�ٶ�
    __HAL_SPI_ENABLE(&SPI3_Handler);             //ʹ��SPI
    
}

//SPI�ٶ����ú���
//SPI�ٶ�=fAPB1/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
//fAPB1ʱ��һ��Ϊ45Mhz��
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
    __HAL_SPI_DISABLE(&SPI2_Handler);            //�ر�SPI
    SPI2_Handler.Instance->CR1&=0XFFC7;          //λ3-5���㣬�������ò�����
    SPI2_Handler.Instance->CR1|=SPI_BaudRatePrescaler;//����SPI�ٶ�
    __HAL_SPI_ENABLE(&SPI2_Handler);             //ʹ��SPI
    
}

//SPI5 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI3_WriteByte(u8 TxData)
{
    u8 Rxdata;
	HAL_SPI_Transmit(&SPI3_Handler,&TxData,1,1000);
    //HAL_SPI_TransmitReceive(&SPI3_Handler,&TxData,&Rxdata,1, 1000);       
 	return Rxdata;          		    //�����յ�������		
}

//SPI2 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{
    u8 Rxdata;
	//u32 res;
    HAL_SPI_TransmitReceive(&SPI2_Handler,&TxData,&Rxdata,1, 1000);      
		//res=(u32)Rxdata;
 	return Rxdata;          		    //�����յ�������		
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
//	tcp_server_flag|=(1<<7);//������Ҫ����
//	tcp_server_sendbuf=netcamfifobuf;
//	len=28;
//	HAL_SPI_IRQHandler(&SPI2_Handler);
//  /* USER CODE BEGIN SPI2_IRQn 1 */

//  /* USER CODE END SPI2_IRQn 1 */
//}

////SPI2 ��дһ���ֽ�
////TxData:Ҫд����ֽ�
////����ֵ:��ȡ�����ֽ�
//u8 SPI2_ReadWriteData(u8 *TxData)
//{
//    u8 Rxdata;
//    HAL_SPI_TransmitReceive(&SPI2_Handler,&TxData,&Rxdata,1, 1000);       
// 	return Rxdata;          		    //�����յ�������		
//}
////SPI2 DMA����һ������
////RxData :���ܵ��ֽ�
////len:���ܵ����ֽڳ���
//u8 SPI2_DMA_ReadData(u8 RxData,u8 len)
//{
//	u8 TxData;
//	HAL_SPI_TransmitReceive_DMA(&SPI2_Handler,&TxData,&RxData,1);
//	return RxData;
//}
