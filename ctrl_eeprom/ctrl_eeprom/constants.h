#pragma once

#include <stdint.h>

#include <assert.h>

#define EEPROM_SIZE 8191

/*
EEPROM Output
8bit Control word, 8 bits each for ROM0, ROM1, ROM2, ROM3
			7	6	5	4	3	2	1	0		7	6	5	4	3	2	1	0
bit:		31	30	29	28	27	26	25	24		23	22	21	20	19	18	17	16
meaning:	MAW	MW	ME	RAW	RAE	RBW	RBE	IRW		PCW	PCC	PCE	MCR	HLT	SPE	SPW	OUTW

			7	6	5	4	3	2	1	0		7	6	5	4	3	2	1	0
bit:		15	14	13	12	11	10	9	8		7	6	5	4	3	2	1	0
meaning:	AS3	AS2	AS1	AS0	ACR	AMD	AOE	--		--	--	--	--	--	--	--	--
*/

#define MAW	((uint8_t)1 << 31) //Mem Address Write
#define MW	((uint8_t)1 << 30) //Mem Write
#define ME	((uint8_t)1 << 29) //Mem Enable
#define RAW	((uint8_t)1 << 28) //Reg A Write
#define RAE	((uint8_t)1 << 27) //Reg A Enable
#define RBW	((uint8_t)1 << 26) //Reg B Write
#define RBE	((uint8_t)1 << 25) //Reg B Enable
#define IRW	((uint8_t)1 << 24) //Instruction Reg Write

#define PCW	((uint8_t)1 << 23) //Program Counter Write
#define PCC	((uint8_t)1 << 22) //Program Counter Count
#define PCE	((uint8_t)1 << 21) //Program Counter Enable
#define MCR	((uint8_t)1 << 20) //Micro Counter Reset
#define HLT	((uint8_t)1 << 19) //Halt
#define SPE ((uint8_t)1 << 18) //Stack Pointer Enable
#define SPW ((uint8_t)1 << 17) //Stack Pointer Write
#define OUTW ((uint8_t)1 << 16) //Output Write

#define AS3 ((uint8_t)1 << 15) //ALU CTRL 3
#define AS2 ((uint8_t)1 << 14) //ALU CTRL 2
#define AS1 ((uint8_t)1 << 13) //ALU CTRL 1
#define AS0 ((uint8_t)1 << 12) //ALU CTRL 0
#define ACR ((uint8_t)1 << 11) //ALU Carry in
#define AMD ((uint8_t)1 << 10) //ALU Mode
#define AOE ((uint8_t)1 << 9)  //ALU Out Enable

//First two cycles common to all instructions
#define FETCH0 (MAW | PCE)		//Program counter to memory address register
#define FETCH1 (ME | IRW | PCC) //Memory to instruction register, increment program counter 

inline uint8_t flip_active_low_signals(uint8_t ctrl_word)
{
	return ctrl_word ^= 
			(MAW | MW | ME | RAW | RAE | RBW | RBE | IRW | PCW | 
			/*PCC |*/ PCE | MCR | HLT | SPE | SPW | OUTW | AOE);
}


/*
EEPROM Input
13 address lines
bit:		12	11	10	9	8	7	6	5	4	3	2	1	0
meaning:	MC	MC	MC	--	--	<---- instruction code  ---->		
*/

#define MC_STEP0 ((uint16_t)0)
#define MC_STEP1 ((uint16_t)1 << 10)
#define MC_STEP2 ((uint16_t)2 << 10)
#define MC_STEP3 ((uint16_t)3 << 10)
#define MC_STEP4 ((uint16_t)4 << 10)
#define MC_STEP5 ((uint16_t)5 << 10)
#define MC_STEP6 ((uint16_t)6 << 10)
#define MC_STEP7 ((uint16_t)7 << 10)

//return step number from eeprom address
inline uint8_t step_no(uint16_t addr)
{
	return addr >> 10;
}

inline uint16_t make_address(uint16_t mc_step, uint8_t instruction)
{
	return mc_step | instruction;
}

/*
Registers

****MOV-able registers**** 
A	(R|W)
B	(R|W)
ALO (R)
PC	(R|W)
OUT	(W)
 

SP	(R|W)
FLAGS (R)
*/

#define R_A 1		//A Register
#define R_B 2		//B Register
#define R_PC 3		//Program Counter
#define R_OUT 4		//Output
#define R_ALO 5		//ALU Output
#define R_SP 6		//Stack Pointer

//#define IMM 6		//Immediate Value
//#define MEM 7		//Memory Address

inline uint8_t reg_read(uint8_t reg)
{
	switch (reg)
	{
	case(R_A):
		return RAE;
	case(R_B):
		return RBE;		
	case(R_PC):
		return PCE;
	case(R_SP):
		return SPE;
	case(R_ALO):
		return AOE;
	}
	assert(false);
	return 0;
}

inline uint8_t reg_write(uint8_t reg)
{
	switch (reg)
	{
	case(R_A):
		return RAW;
	case(R_B):
		return RBW;
	case(R_PC):
		return PCW;
	case(R_SP):
		return SPW;
	case(R_OUT):
		return OUTW;
	}
	assert(false);
	return 0;
}

/*
ALU Commands

		S3	S2	S1	S0	M	C
INC		L	L	L	L	L	L
DEC		H	H	H	H	L	H
ADD		H	L	L	H	L	H
ADD CR	H	L	L	H	L	L
SUB		L	H	H	L	L	L
SUB CR	L	H	H	L	L	H
SHIFT	H	H	L	L	L	H
CMP		L	H	H	L	L	H
NOT		L	L	L	L	H	X
AND		H	L	H	H	H	X
OR		H	H	H	L	H	X
XOR		L	H	H	L	H	X
*/

#define ALU_INC ((uint8_t) 0)	//Inc Bus
#define ALU_DEC ((uint8_t) 1)	//Dec Bus
#define ALU_ADD ((uint8_t) 2)	//Bus + B Register
#define ALU_ADC ((uint8_t) 3)	//Bus + B Register + 1
#define ALU_SUB ((uint8_t) 4)	//Bus - B Register
#define ALU_SBC ((uint8_t) 5)	//Bus - B Register - 1
#define ALU_SFT ((uint8_t) 6)	//Shift Bus Left
#define ALU_CMP ((uint8_t) 7)	//Compare Bus to B Register
#define ALU_NOT ((uint8_t) 8)	//!Bus
#define ALU_AND ((uint8_t) 9)	//Bus & B Register
#define ALU_OR  ((uint8_t) 10)	//Bus | B Register
#define ALU_XOR ((uint8_t) 11)	//Bus ^ B Register
//special cases
#define ALU_PUSH ((uint8_t)12)	//Dec SP, Reg -> [SP]
#define ALU_POP  ((uint8_t)13)	//[SP] -> Reg, Inc SP
#define ALU_CALL ((uint8_t)14)	//Push PC, Reg -> PC
#define ALU_RET  ((uint8_t)15)	//Pop PC


//Returns the control signals for the given operation
inline uint32_t alu_ctrl(uint8_t aluOp)
{
	switch (aluOp)
	{
	case(ALU_INC):
		return ((uint32_t)0);
	case(ALU_DEC):
		return ((uint32_t)AS3 | AS2 | AS1 | AS0 | ACR);
	case(ALU_ADD):
		return ((uint32_t)AS3 | AS0 | ACR);
	case(ALU_ADC): 
		return ((uint32_t)AS3 | AS0);
	case(ALU_SUB): 
		return ((uint32_t)AS2 | AS1);
	case(ALU_SBC):
	case(ALU_CMP):
		return ((uint32_t)AS2 | AS1 | ACR);
	case(ALU_SFT): 
		return ((uint32_t)AS3 | AS2 | ACR);
	case(ALU_NOT): 
		return ((uint32_t)AMD);
	case(ALU_AND): 
		return ((uint32_t)AMD | AS3 | AS1 | AS0);
	case(ALU_OR): 
		return ((uint32_t)AMD | AS3 | AS2 | AS1);
	case(ALU_XOR): 
		return ((uint32_t)AMD | AS2 | AS1);
	}
	assert(false);
	return 0;
}


/*
Instructions

MOV
	REG <- REG
	REG <- MEM
	REG <- IMM
	MEM <- REG
CALL	(Push PC)
	IMM
	REG
RET		(Pop PC)
PUSH
	IMM
	REG
POP
	REG
ALU
	REG
JMP		(MOV -> PC)
	IMM
	REG
JZ
	IMM
	REG
JC
	IMM
	REG
HLT
NOP
*/

/*
mov instruction encoing

bit 7:		high
bit 6:		high if src to be dereferenced
bit 5-3:	dest register
bit 2-0:	src register

Jmp instructions are a mov into the program counter
Immediate values are loaded by dereferencing the program counter

bit:		7	6	5	4	3	2	1	0
meaning:	1 DEREF <- DEST ->  <- SRC ->
*/

inline uint8_t make_mov_instruction(uint8_t dest_reg, uint8_t src_reg, bool deref)
{
	return 128 | (deref << 6) | (dest_reg << 3) | src_reg;
}

/*
ALU Instruction encoding

bit 7:		low
bit 6-3:	4 bit operation
bit 2-0:	src register

There are 12 ALU operations and 4 special cases (PUSH, POP, CALL, RET)

bit:		7	6	5	4	3	2	1	0
meaning :	0	<- 4 bit OP->   <- SRC ->

*/
inline uint8_t make_alu_instruction(uint8_t op, uint8_t src_reg)
{
	return 0 | (op << 6) | src_reg;
}
