#ifndef __AD5933_H
#define __AD5933_H
#include "sys.h"


#define AD5933_ADDR 0X1A
#define K 1111000

u8 AD5933_Init(void);
void AD5933_Start(void);
u8 AD5933_WRITE_COMMAND(u8 Command,u8 data);
u32 AD5933_RES(void);
u32 wendu(void);
void AD5933_Set_Point(u8 reg_addr);
void AD5933_Read_Data(u8 len);
#endif