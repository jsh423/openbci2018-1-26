#include "netcamera_demo.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "led.h"
#include "ov5640.h"
#include "dcmi.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "delay.h"
#include "malloc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//网络摄像头程序  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/2/24
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
//网络摄像头任务
//设置任务优先级
#define NETCAMERA_TASK_PRIO       		   2 
//设置任务堆栈大小
#define NETCAMERA_STK_SIZE  		 	   1500
//任务堆栈	
OS_STK NETCAMERA_TASK_STK[NETCAMERA_STK_SIZE];	
//任务函数
void netcamera_task(void *pdata); 

                                        
struct netconn *newconn=0;		            //NET CAM TCP网络连接结构体指针 
u32 *netcam_line_buf0;					    //定义行缓存0  
u32 *netcam_line_buf1;					    //定义行缓存1   

//NET CAM FIFO
vu16 netcamfifordpos=0;					    //FIFO读位置
vu16 netcamfifowrpos=0;					    //FIFO写位置
u32 *netcamfifobuf[NETCAM_FIFO_NUM];	    //定义NETCAM_FIFO_SIZE个接收FIFO			

//读取FIFO
//buf:数据缓存区首地址
//返回值:0,没有数据可读;
//      1,读到了1个数据块
u8 netcam_fifo_read(u32 **buf)
{
	if(netcamfifordpos==netcamfifowrpos)return 0;
	netcamfifordpos++;		//读位置加1
	if(netcamfifordpos>=NETCAM_FIFO_NUM)netcamfifordpos=0;//归零 
	*buf=netcamfifobuf[netcamfifordpos];
	return 1;
}
//写一个FIFO
//buf:数据缓存区首地址
//返回值:0,写入成功;
//      1,写入失败
u8 netcam_fifo_write(u32 *buf)
{
	u16 i;
	u16 temp=netcamfifowrpos;	//记录当前写位置
	netcamfifowrpos++;			//写位置加1
	if(netcamfifowrpos>=NETCAM_FIFO_NUM)netcamfifowrpos=0;//归零  
	if(netcamfifordpos==netcamfifowrpos)
	{
		netcamfifowrpos=temp;	//还原原来的写位置,此次写入失败
		//printf("shit:%d\r\n",temp);
		return 1;	
	}
	for(i=0;i<NETCAM_LINE_SIZE;i++)netcamfifobuf[netcamfifowrpos][i]=buf[i];//拷贝数据
	return 0;
}   


//相应变量的内存申请
//返回值:0 成功；其他 失败
u8 netmem_malloc(void)
{
    u16 t=0;
    netcam_line_buf0=mymalloc(SRAMIN,NETCAM_LINE_SIZE*4);
	netcam_line_buf1=mymalloc(SRAMIN,NETCAM_LINE_SIZE*4);

    //给FIFO申请内存
    for(t=0;t<NETCAM_FIFO_NUM;t++) 
	{
		netcamfifobuf[t]=mymalloc(SRAMEX,NETCAM_LINE_SIZE*4);
	}  
    if(!netcamfifobuf[NETCAM_FIFO_NUM-1]||!netcam_line_buf1||!netcam_line_buf0)//内存申请失败  
    {
        netmem_free();//释放内存
        return 1;
    }
    return 0;
}

//相应变量的内存释放
void netmem_free(void)
{
    u16 t=0;
    myfree(SRAMIN,netcam_line_buf0);
    myfree(SRAMIN,netcam_line_buf1);
    //释放FIFO的内存
    for(t=0;t<NETCAM_FIFO_NUM;t++) 
	{
        myfree(SRAMEX,netcamfifobuf[t]);
	}  
}

//设置网络摄像头分辨率
//resol: 0,320*240
//      1,640*480
//      3,1024*768
//      4,1280*800
void netcam_resolu_set(u8 resol)
{
    switch(resol)
    {
        case 0: 
            OV5640_OutSize_Set(16,4,320,240);	
            break;
        case 1: 
            OV5640_OutSize_Set(16,4,640,480);	
            break;
        case 2: 
            OV5640_OutSize_Set(16,4,1024,768);	
            break;
        case 3: 
            OV5640_OutSize_Set(16,4,1280,800);	
            break;
        default:
            OV5640_OutSize_Set(16,4,1280,800);	//默认为1280*800
            break;               
    }
}    
    
//摄像头jpeg数据接收回调函数
static void netcam_dcmi_rx_callback(void)
{    
	if(DMA2_Stream1->CR&(1<<19))netcam_fifo_write(netcam_line_buf0);	//netcam_line_buf0写入FIFO
	else netcam_fifo_write(netcam_line_buf1);							//netcam_line_buf1写入FIFO 
}

//openbci初始化
void hardware_init(void)
{
	netmem_malloc();
	
	ADS1299_Init();
	
	while(ADS1299_Check()) 									//初始化tcp_server(创建tcp_server程)
	{
		LCD_ShowString(30,110,200,20,16,"ADS1299 Check failed! "); 
		delay_ms(500);
		LCD_Fill(30,110,230,170,WHITE);
		delay_ms(500);
	}
	LCD_ShowString(30,150,200,20,16,"ads1299 check Success!  "); 	//tcp服务器创建成功
	
	// spi2_rx_callback=spi2_rx_dma_callback;//接收数据回调函数
     BCI_DMA_Init((u32)netcam_line_buf0,0,NETCAM_LINE_SIZE,DMA_MDATAALIGN_BYTE,DMA_MINC_ENABLE); 
	//ADS1299_DMA_Config(DMA1_Stream3,DMA_CHANNEL_0);//初始化DMA
	//ADS1299_Start();
	ADS1299_IT();		//中断配置
}






//网络摄像头初始化
u8 camera_init(void)
{
    u8 rval=0;
    
    netmem_malloc();//申请内存
    //配置OV5640
    if(OV5640_Init())	    //初始化OV5640
	{
		printf("ov5640 error\r\n");
		delay_ms(500);  
        rval=1;
	}  
    if(rval==0)
    {
        OV5640_RGB565_Mode();	//RGB565模式 
        OV5640_Focus_Init();    //自动对焦初始化
        OV5640_Light_Mode(0);	//自动模式
        OV5640_Color_Saturation(3);//色彩饱和度0
        OV5640_Brightness(4);	//亮度0
        OV5640_Contrast(3);		//对比度0
        OV5640_Sharpness(33);	//自动锐度
        OV5640_Focus_Constant();//启动持续对焦
        DCMI_Init();			//DCMI配置
        dcmi_rx_callback=netcam_dcmi_rx_callback;//JPEG接收数据回调函数
        DCMI_DMA_Init((u32)netcam_line_buf0,(u32)netcam_line_buf1,NETCAM_LINE_SIZE,DMA_MDATAALIGN_WORD,DMA_MINC_ENABLE);  
        OV5640_JPEG_Mode();		//JPEG模式
        //改为15帧
//        OV5640_WR_Reg(0X3035,0X11);
//        OV5640_WR_Reg(0x3824,0X04);
//        OV5640_ImageWin_Set(0,0,1280,800);

        OV5640_OutSize_Set(16,4,1280,800);		//设置输出尺寸(1280*800)  
        printf("开启DCMI\r\n");
        DCMI_Start(); 		//启动传输         
        delay_ms(100);
        printf("初始化完成\r\n");
    } 
    return rval;
}

//网络摄像头复位
void camera_deinit(void)
{
    netmem_free();      //释放内存
    netcamfifordpos=0;  //读位置清零
    netcamfifowrpos=0;  //写位置清零
    DCMI_Stop();        //关闭传输
    HAL_DCMI_DeInit(&DCMI_Handler); //复位DCMI
    HAL_DMA_DeInit(&DMADMCI_Handler);//复位DMA
}
    
//网络摄像头任务
void netcamera_task(void *arg)
{
	err_t err;
	struct netconn *conn;
    static ip_addr_t ipaddr;
    u8 remot_addr[4];
	static u16_t port;
    
    u8 res=0;
	u32 *tbuf;
    
	LWIP_UNUSED_ARG(arg);

	conn = netconn_new(NETCONN_TCP);  //创建一个TCP链接
	netconn_bind(conn,IP_ADDR_ANY,8088);  //绑定端口 8088号端口
	netconn_listen(conn);  		//进入监听模式
    
    while(1)    //等待连接
    {
        err=netconn_accept(conn,&newconn);  //接收连接请求
			
        if(err==ERR_OK) //成功检测到连接
        {
            netconn_getaddr(newconn,&ipaddr,&port,0); //获取远端IP地址和端口号
            remot_addr[3]=(uint8_t)(ipaddr.addr>>24); 
            remot_addr[2]=(uint8_t)(ipaddr.addr>>16);
            remot_addr[1]=(uint8_t)(ipaddr.addr>>8);
            remot_addr[0]=(uint8_t)(ipaddr.addr);
            printf("主机%d.%d.%d.%d连接上网络摄像头服务器,主机端口号为:%d\r\n",\
                    remot_addr[0],remot_addr[1],remot_addr[2],remot_addr[3],port);
            camera_init();
            delay_ms(1000); //此延时一定要加！！
            while(1)        //开始视频传输
            {
                res=netcam_fifo_read(&tbuf);    //读取FIFO中的数据
                if(res)     //有数据要发送
                {
                    err=netconn_write(newconn,tbuf,NETCAM_LINE_SIZE*4,NETCONN_COPY);//发送数据
                    if((err==ERR_CLSD)||(err==ERR_RST))//关闭连接,或者重启网络 
                    {
                        camera_deinit();//复位网络摄像头
                        netconn_close(newconn);
                        netconn_delete(newconn);
                        printf("主机:%d.%d.%d.%d断开与视频服务器的连接\r\n",\
                            remot_addr[0], remot_addr[1],remot_addr[2],remot_addr[3]);
                        break;
                    }	                    
                }else
                { 
                    OSTimeDlyHMSM(0,0,0,2);//延时2ms
                }	
            }
        }
    }
   
}

//创建网络摄像头线程
//返回值:0 创建成功
//		其他 创建失败
INT8U netcamera_init(void)
{
	INT8U res;
	OS_CPU_SR cpu_sr;

	OS_ENTER_CRITICAL();	//关中断
	res = OSTaskCreate(netcamera_task,(void*)0,(OS_STK*)&NETCAMERA_TASK_STK[NETCAMERA_STK_SIZE-1],NETCAMERA_TASK_PRIO); //创建网络摄像头线程
	OS_EXIT_CRITICAL();		//开中断
	
	return res;
}


