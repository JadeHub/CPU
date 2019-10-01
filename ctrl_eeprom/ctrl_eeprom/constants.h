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

#define MAW	((uint32_t)1 << 31) //Mem Address Write
#define MW	((uint32_t)1 << 30) //Mem Write
#define ME	((uint32_t)1 << 29) //Mem Enable
#define RAW	((uint32_t)1 << 28) //Reg A Write
#define RAE	((uint32_t)1 << 27) //Reg A Enable
#define RBW	((uint32_t)1 << 26) //Reg B Write
#define RBE	((uint32_t)1 << 25) //Reg B Enable
#define IRW	((uint32_t)1 << 24) //Instruction Reg Write

#define PCW	((uint32_t)1 << 23) //Program Counter Write
#define PCC	((uint32_t)1 << 22) //Program Counter Count
#define PCE	((uint32_t)1 << 21) //Program Counter Enable
#define MCR	((uint32_t)1 << 20) //Micro Counter Reset
#define HLT	((uint32_t)1 << 19) //Halt
#define SPE ((uint32_t)1 << 18) //Stack Pointer Enable
#define SPW ((uint32_t)1 << 17) //Stack Pointer Write
#define OUTW ((uint32_t)1 << 16) //Output Write

#define AS3 ((uint32_t)1 << 15) //ALU CTRL 3
#define AS2 ((uint32_t)1 << 14) //ALU CTRL 2
#define AS1 ((uint32_t)1 << 13) //ALU CTRL 1
#define AS0 ((uint32_t)1 << 12) //ALU CTRL 0
#define ACR ((uint32_t)1 << 11) //ALU Carry in
#define AMD ((uint32_t)1 << 10) //ALU Mode
#define AOE ((uint32_t)1 << 9)  //ALU Out Enable

//First two cycles common to all instructions
#define FETCH0 (MAW | PCE)		//Program counter to memory address register
#define FETCH1 (ME | IRW | PCC) //Memory to instruction register, increment program counter 

inline uint32_t flip_active_low_signals(uint32_t ctrl_word)
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

REG   R   [R]   W   [W]
-----------------------
A     Y    Y    Y    Y
B     Y    Y    Y    Y
ALO   Y    Y    N    Y
PC    N    Y    Y    N (Read Immediate, JMP)
OUT   N    N    Y    N
SP    N    Y    N    Y (Push, Pop)
*/

#define R_A			1		//A Register
#define R_B			2		//B Register
#define R_PC		3		//Program Counter
#define R_OUT		4		//Output
#define R_ALO		5		//ALU Output
#define R_SP		6		//Stack Pointer

inline uint32_t reg_read(uint8_t reg)
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

inline uint32_t reg_write(uint8_t reg)
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
#define ALU_AND ((uint8_t) 9)	//Bus & xB Register
#define ALU_OR  ((uint8_t) 10)	//Bus | B Register
#define ALU_XOR ((uint8_t) 11)	//Bus ^ B Register

const char* alu_op_name(uint8_t aluOp)
{
	switch (aluOp)
	{
	case(ALU_INC):
		return "INC";
	case(ALU_DEC):
		return "DEC";
	case(ALU_ADD):
		return "ADD";
	case(ALU_ADC):
		return "ADC";
	case(ALU_SUB):
		return "SUB";
	case(ALU_SBC):
		return "SBC";
	case(ALU_CMP):
		return "CMP";
	case(ALU_SFT):
		return "SHIFT";
	case(ALU_NOT):
		return "NOT";
	case(ALU_AND):
		return "AND";
	case(ALU_OR):
		return "OR";
	case(ALU_XOR):
		return "XOR";
	}
	return "";
}

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
instruction encoding
bit:				7	6	5	4	3	2	1	0
mov instructions:	1   0   <- DEST->   <- SRC ->
alu instructions:	0	<- 4 bit OP->   <- SRC ->
ancillary insts:	1	1	X	X	X	X	X	X
	Call:			1	1	0	0	0	<- SRC ->		0xC0
	JZ:				1	1	0	0	1	<- SRC ->		0xC8
	JE:				1	1	0	1	0	<- SRC ->		0xD0
	JN:				1	1	1	0	0	<- SRC ->		0xE0
	JC:				1	1	0	1	1	<- SRC ->		0xD8
	JMP:			1	1	1	0	1	<- SRC ->		0xE8
	Push:			1	1	1	1	0	<- SRC ->		0xF0
	Pop:			1	1	1	1	1	<- SRC ->		0xF8
	Halt:			1	1	1	1	1	0	0	1		0xF9
	Ret:			1	1	1	1	1	0	1	0		0xFA
	No op:			1	1	1	1	1	1	1	1		0xFF
	

* Jmp instructions are a mov into the program counter
* Immediate values are loaded by dereferencing the program counter

Source register encoding
reg		value
-------------
 A		0
[A]		1
 B		2
[B]		3
ALO		4
[ALO]	5
[PC]	6	(Load an immediate)
[[PC]]	7	(Load from an immediate address)

Destination register encoding
reg		value
-------------
 A		0
[A]		1
 B		2
[B]		3
[ALO]	4
OUT		5
[[PC]]	6	(Write to an immediate address)

*/

inline char* source_reg_name(uint8_t reg, bool deref)
{
	switch (reg)
	{
	case(R_A):
		return deref ? "[A]" : "A";
	case(R_B):
		return deref ? "[B]" : "B";
	case(R_ALO):
		return deref ? "[ALO]" : "ALO";
	case(R_PC):
		return deref ? "[[PC]]" : "[PC]";
	}
	return "";
}

inline uint8_t encode_source_reg(uint8_t reg, bool deref)
{
	switch (reg)
	{
	case(R_A):
		return deref ? 1 : 0;
	case(R_B):
		return deref ? 3 : 2;
	case(R_ALO):
		return deref ? 5 : 4;
	case(R_PC):
		return deref ? 7 : 6;
	}
	assert(false);
	return 0;
}


inline char* dest_reg_name(uint8_t reg, bool deref)
{
	switch (reg)
	{
	case(R_A):
		return deref ? "[A]" : "A";
	case(R_B):
		return deref ? "[B]" : "B";
	case(R_ALO):
		return "ALO";
	case(R_OUT):
		return "OUT";
	case(R_PC):
		return "[PC]";
	}
	return "";
}

inline uint8_t encode_dest_reg(uint8_t reg, bool deref)
{
	switch (reg)
	{
	case(R_A):
		return deref ? 1 : 0;
	case(R_B):
		return deref ? 3 : 2;
	case(R_ALO):
		assert(deref);
		return  4;
	case(R_OUT):
		return 5;
	case(R_PC):
		assert(deref);
		return 6;
	}
	assert(false);
	return 0;
}

inline uint8_t make_mov_instruction(uint8_t dest_reg, uint8_t src_reg)
{
	return 128 | (dest_reg << 3) | src_reg;
}

/*
ALU Instruction encoding

bit 7:		low
bit 6-3:	4 bit operation
bit 2-0:	src register

bit:		7	6	5	4	3	2	1	0
meaning :	0	<- 4 bit OP->   <- SRC ->

*/
inline uint8_t make_alu_instruction_code(uint8_t op, uint8_t src_reg)
{
	return 0 | (op << 3) | src_reg;
}
