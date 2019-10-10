#ifndef __TCA6424A_H
#define __TCA6424A_H
#include "sys.h"


//#define MACRO *(unsigned char*)&
	

#define RST_TCA6424A1        PBout(15) //TC6424A1复位控制
//#define RST_TCA6424A2        PCin(13)//TC6424A2复位控制
#define LEDBit1 0X38be00

/************************** I2C Address ***************************************/
#define TCA6424A_ADDRESS1		0x23 		// I2C Address 0100 01 + ADDR + W
//#define TCA6424A_ADDRESS2		0X22		//此处规格书资料有异议
											// ADDR tied to P2.3 of LaunchPad
/************************** I2C Registers *************************************/
//需要设置
#define TCA6424A_OUTPUT_REG0	0x84		// Output register to change state of output BIT set to 1, output set HIGH
#define TCA6424A_OUTPUT_REG1	0x85		// Output register to change state of output BIT set to 1, output set HIGH
#define TCA6424A_OUTPUT_REG2	0x86		// Output register to change state of output BIT set to 1, output set HIGH
#define TCA6424A_CONFIG_REG0   	0x8C		// Configuration register. BIT = '1' sets port to input BIT = '0' sets port to output
#define TCA6424A_CONFIG_REG1   	0x8D		// Configuration register. BIT = '1' sets port to input BIT = '0' sets port to output
#define TCA6424A_CONFIG_REG2   	0x8E		// Configuration register. BIT = '1' sets port to input BIT = '0' sets port to output
#define TCA6424A_POLARITY_REG0 	0x88		// Polarity inversion register. BIT '1' inverts input polarity of register 0x00
#define TCA6424A_POLARITY_REG1 	0x89		// Polarity inversion register. BIT '1' inverts input polarity of register 0x00
#define TCA6424A_POLARITY_REG2 	0x8A		// Polarity inversion register. BIT '1' inverts input polarity of register 0x00
u8 TCA6424A_Init(void);

void TCA6424A_W(u8 ADDR,u8 reg,u32 DataToWrite);
void TCA6424_W_BIT(u8 reg,u8 res);
//void TCA6424A_W(u8 Reg,u32 DataToWrite);//
//void TCA6424_SetIo(void);
//u8 TCA6424A_SetIo(void);
u8 TCA6424A_R(u8);
void TCA6424A_WriteBit(u8 bit,u8 sta);

#endif
//struct TCA6424A_sBit{
//	unsigned char B0:1;
//	unsigned char B1:1;
//	unsigned char B2:1;
//	unsigned char B3:1;
//	unsigned char B4:1;
//	unsigned char B5:1;
//	unsigned char B6:1;
//	unsigned char B7:1;

//};

//union TCA6424_uInput{
//	unsigned char	all;
//	struct TCA6424A_sBit bit;
//};

//struct TCA6424A_sInputP{
//	union TCA6424_uInput P0;
//	union TCA6424_uInput P1;
//	union TCA6424_uInput P2;
//};

//union TCA6424A_uInputA{
//	unsigned long 		all;
//	struct TCA6424A_sInputP	 	Port;
//};



//union TCA6424A_uOutputP0{
//	unsigned char 		all;
//	struct TCA6424A_sBit	bit;
//};


//struct TCA6424A_sOutput{
//	union TCA6424A_uOutputP0	P0;
//	union TCA6424A_uOutputP0	P1;
//	union TCA6424A_uOutputP0	P2;
//};

//union TCA6424A_uOutput{
//	unsigned long	all;
//	struct TCA6424A_sOutput	Port;
//};

//union TCA6424A_uPolarityInversionP0{
//	unsigned char all;
//	struct TCA6424A_sBit bit;
//};

//union  TCA6424A_uPolarityInversionP1{
//	unsigned char	all;
//	struct TCA6424A_sBit bit;
//};

//union  TCA6424A_uPolarityInversionP2{
//	unsigned char	all;
//	struct TCA6424A_sBit bit;
//};

//struct TCA6424A_sPolarityInversion{
//	union TCA6424A_uPolarityInversionP0	P0;
//	union TCA6424A_uPolarityInversionP0	P1;
//	union TCA6424A_uPolarityInversionP0	P2;
//};

//union TCA6424A_uPolarityInversion{
//	unsigned long all;
//	struct TCA6424A_sPolarityInversion Port;
//};

//union TCA6424A_uConfigP0{
//	unsigned char all;
//	struct TCA6424A_sBit bit;
//};

//union  TCA6424A_uConfigP1{
//	unsigned char	all;
//	struct TCA6424A_sBit bit;
//};

//union  TCA6424A_uConfigP2{
//	unsigned char	all;
//	struct TCA6424A_sBit bit;
//};

//struct TCA6424A_sConfig{
//	union TCA6424A_uConfigP0	P0;
//	union TCA6424A_uConfigP0	P1;
//	union TCA6424A_uConfigP0	P2;
//};

//union TCA6424A_uConfig{
//	unsigned long all;
//	struct TCA6424A_sConfig Port;
//};



//typedef struct {
//	union TCA6424A_uInputA 				Input;
//	union TCA6424A_uOutput				Output;
//	union TCA6424A_uPolarityInversion	PolarityInversion;
//	union TCA6424A_uConfig				Config;
//} TCA6424ARegs;

//void TCA6424AWriteConfig(TCA6424ARegs * Regs);
//void TCA6424AWriteOutput(TCA6424ARegs * Regs);
//void TCA6424AWritePolarity(TCA6424ARegs * Regs);

//void TCA6424AInitDefault(TCA6424ARegs* Regs);
//void TCA6424AInitI2CReg(TCA6424ARegs* Regs);
//void TCA6424AReadInputReg(TCA6424ARegs* Regs);
