// ctrl_eeprom.cpp : Defines the entry point for the console application.
//

#include "constants.h"
#include <iostream>
#include <map>

std::map<uint16_t, uint32_t> eeprom_values;

void print_mov_instruction(uint8_t instr, const char* arg1, const char* arg2, const char* comment)
{
	std::cout << unsigned(instr) << "\t\tMOV " << arg1 << ", " << arg2 << "\t\t;" << comment << std::endl;
}

//MOV [address],SRC
//eg MOV [12], A
void make_store_immediate_instr(uint8_t source_reg)
{
	uint8_t instr = make_mov_instruction_code(encode_dest_reg(R_PC, true), encode_source_reg(source_reg, false));
	eeprom_values[make_address(MC_STEP2, instr)] = MAW | reg_read(R_PC); //program counter to address reg
	eeprom_values[make_address(MC_STEP3, instr)] = reg_read(source_reg) | MW | PCC | MCR; //source_reg to mem, pcc
	print_mov_instruction(instr, "[42]", source_reg_name(source_reg, false), "Move to litteral address");
}

//MOV DEST, [address]
//eg MOV A, [12]
void make_load_immediate_addr_instr(uint8_t dest_reg)
{
	uint8_t instr = make_mov_instruction_code(encode_dest_reg(dest_reg, false), encode_source_reg(R_PC, true));
	eeprom_values[make_address(MC_STEP2, instr)] = MAW | reg_read(R_PC); //program counter to address reg
	eeprom_values[make_address(MC_STEP3, instr)] = ME | MAW | PCC; //memory to address reg, PCC
	eeprom_values[make_address(MC_STEP4, instr)] = reg_write(dest_reg) | ME | MCR; //memory to dest_reg
	print_mov_instruction(instr, dest_reg_name(dest_reg, false), "[42]", "Move from litteral address");
}

//MOV DEST, immediate
//eg MOV A, 12
void make_load_immediate_instr(uint8_t dest_reg)
{
	uint8_t instr = make_mov_instruction_code(encode_dest_reg(dest_reg, false), encode_source_reg(R_PC, false));
	eeprom_values[make_address(MC_STEP2, instr)] = MAW | reg_read(R_PC); //program counter to address reg
	eeprom_values[make_address(MC_STEP3, instr)] = reg_write(dest_reg) | ME | PCC | MCR; //mem to dest_reg, PCC
	print_mov_instruction(instr, dest_reg_name(dest_reg, false), "42", "Move litteral");
}

void make_reg_reg_mov_instr(uint8_t src_reg, bool deref_src, uint8_t dest_reg, bool deref_dest)
{
	assert(!(deref_dest && deref_src));
	uint8_t instr = make_mov_instruction_code(encode_dest_reg(dest_reg, deref_dest), encode_source_reg(src_reg, deref_src));

	print_mov_instruction(instr, dest_reg_name(dest_reg, deref_dest), source_reg_name(src_reg, deref_src), "\t;Move");
	if (deref_src)
	{
		//mov [src] to dest
		instr = make_mov_instruction_code(encode_dest_reg(dest_reg, deref_dest), encode_source_reg(src_reg, deref_src));
		eeprom_values[make_address(MC_STEP2, instr)] = MAW | reg_read(src_reg); //src reg to address reg
		eeprom_values[make_address(MC_STEP3, instr)] = reg_write(dest_reg) | ME | MCR; //mem to reg
	}
	else if (deref_dest)
	{
		//mov src to [dest]
		eeprom_values[make_address(MC_STEP2, instr)] = MAW | reg_read(dest_reg); //dest reg to address reg
		eeprom_values[make_address(MC_STEP3, instr)] = reg_read(src_reg) | MW | MCR; //src to mem
	}
	else
	{
		//mov src to dest
		instr = make_mov_instruction_code(encode_dest_reg(dest_reg, false), encode_source_reg(src_reg, false));
		eeprom_values[make_address(MC_STEP2, instr)] = reg_write(dest_reg) | reg_read(src_reg) | MCR;
	}
}

void make_mov_instructions()
{
	//From A
	make_reg_reg_mov_instr(R_A, false, R_B, false);		//A to B
	make_reg_reg_mov_instr(R_A, true, R_B, false);		//[A] to B
	make_reg_reg_mov_instr(R_A, false, R_B, true);		//A to [B]
	make_reg_reg_mov_instr(R_A, false, R_ALO, true);	//A to ALO
	make_reg_reg_mov_instr(R_A, false, R_OUT, false);	//A to OUT
	make_reg_reg_mov_instr(R_A, true, R_OUT, false);	//[A] to out	
	make_store_immediate_instr(R_A);					//A to [[PC]] (write to immediate addr)
	
	//From B
	make_reg_reg_mov_instr(R_B, false, R_A, false);		//B to A
	make_reg_reg_mov_instr(R_B, true, R_A, false);		//[B] to A
	make_reg_reg_mov_instr(R_B, false, R_A, true);		//B to [A]
	make_reg_reg_mov_instr(R_B, false, R_ALO, true);	//B to ALO
	make_reg_reg_mov_instr(R_B, false, R_OUT, false);	//B to OUT
	make_reg_reg_mov_instr(R_B, true, R_OUT, false);	//[B] to out
	make_store_immediate_instr(R_B);					//B to [[PC]] (write to immediate addr)

    //From ALO
	make_reg_reg_mov_instr(R_ALO, false, R_A, false);	//ALO to A
	make_reg_reg_mov_instr(R_ALO, true, R_A, false);	//[ALO] to A
	make_reg_reg_mov_instr(R_ALO, false, R_A, true);	//ALO to [A]
	make_reg_reg_mov_instr(R_ALO, false, R_B, false);	//ALO to B
	make_reg_reg_mov_instr(R_ALO, true, R_B, false);	//[ALO] to B
	make_reg_reg_mov_instr(R_ALO, false, R_B, true);	//ALO to [B]
	make_reg_reg_mov_instr(R_ALO, false, R_OUT, false);	//ALO to OUT
	make_reg_reg_mov_instr(R_ALO, true, R_OUT, false);	//[ALO] to out
	make_store_immediate_instr(R_ALO);					//ALO to [[PC]] write to immediate addr

	//Load immediate (eg mov a, 42)
	make_load_immediate_instr(R_A);
	make_load_immediate_instr(R_B);
	make_load_immediate_instr(R_OUT);

	//Load from immediate address (eg mov a, [42])
	make_load_immediate_addr_instr(R_A);
	make_load_immediate_addr_instr(R_B);
	make_load_immediate_addr_instr(R_OUT);
}

void make_alu_instructions()
{
	uint8_t instr;
	for (uint8_t aluOp = ALU_INC; aluOp <= ALU_XOR; aluOp++)
	{
		const char* opName = alu_op_name(aluOp);
		//eg ADD a
		instr = make_alu_instruction_code(aluOp, encode_source_reg(R_A, false));
		eeprom_values[make_address(MC_STEP2, instr)] = reg_read(R_A) | alu_ctrl(aluOp) | MCR; //Reg A, operation
		std::cout << unsigned(instr) << "\t\t" << opName << " A" << std::endl;
		//eg ADD [a]
		instr = make_alu_instruction_code(aluOp, encode_source_reg(R_A, true));
		eeprom_values[make_address(MC_STEP2, instr)] = reg_read(R_A) | MAW;	//Reg to to address
		eeprom_values[make_address(MC_STEP3, instr)] = ME | alu_ctrl(aluOp) | MCR; //Mem, operation
		std::cout << unsigned(instr) << "\t\t" << alu_op_name(aluOp) << " [A]" << std::endl;
		//Eg ADD 12
		instr = make_alu_instruction_code(aluOp, encode_source_reg(R_PC, false));
		eeprom_values[make_address(MC_STEP2, instr)] = reg_read(R_PC) | MAW;	//PC to to address
		eeprom_values[make_address(MC_STEP3, instr)] = ME | alu_ctrl(aluOp) | PCC | MCR; //Mem, operation
		std::cout << unsigned(instr) << "\t\t" << alu_op_name(aluOp) << " 42" << std::endl;

		//Eg ADD [12]
		//todo
	}		
}


/*
JZ:				1	1	0	0	1	<- SRC -> 0xC8
JE:				1	1	0	1	0	<- SRC -> xD0
JN:				1	1	1	0	0	<- SRC -> xE0
JC:				1	1	0	1	1	<- SRC -> xD8
JMP:			1	1	1	0	1	<- SRC -> xE8
*/

inline const char* jump_instr_name(uint8_t instr)
{
	if ((instr & 0xF8) == 0xC8)
		return "JZ";
	if ((instr & 0xF8) == 0xD0)
		return "JE";
	if ((instr & 0xF8) == 0xE0)
		return "JN";
	if ((instr & 0xF8) == 0xD8)
		return "JC";
	if ((instr & 0xF8) == 0xE8)
		return "JMP";
	return "";
}

//Non-conditional jmp
void make_reg_jmp_instr(uint8_t jmp, uint8_t src_reg)
{
	uint8_t instr = make_ancillory_instruction_code(jmp, encode_source_reg(src_reg, false));
	eeprom_values[make_address(MC_STEP2, instr)] = reg_read(src_reg) | reg_write(R_PC) | MCR;	//Reg to PC
	std::cout << unsigned(instr) << "\t\t" << jump_instr_name(jmp) << " " << source_reg_name(src_reg, false) << std::endl;
}

void make_immediate_jmp_instr(uint8_t jmp)
{
	uint8_t instr = make_ancillory_instruction_code(jmp, encode_source_reg(R_PC, false));
	eeprom_values[make_address(MC_STEP2, instr)] = reg_read(R_PC) | MAW;	//PC to to address
	eeprom_values[make_address(MC_STEP3, instr)] = ME | reg_write(R_PC) | MCR; //Mem to PC
	std::cout << unsigned(instr) << "\t\t" << jump_instr_name(jmp) << " [address]" << std::endl;
}

void make_cond_reg_jmp_instr(uint8_t jmp, uint8_t src_reg, uint16_t cond)
{
	uint8_t instr = make_ancillory_instruction_code(jmp, encode_source_reg(src_reg, false));
	eeprom_values[make_address(MC_STEP2, instr) | cond] = reg_read(src_reg) | reg_write(R_PC) | MCR;	//Reg to PC
	//cond can be CND_JMP or CND_CR, in either case the condition should still be true if both are set
	eeprom_values[make_address(MC_STEP2, instr) | CND_CR | CND_JMP] = eeprom_values[make_address(MC_STEP2, instr) | cond];

	std::cout << unsigned(instr) << "\t\t" << jump_instr_name(jmp) << " " << source_reg_name(src_reg, false) << std::endl;
}

void make_cond_immediate_jmp_instr(uint8_t jmp, uint16_t cond)
{
	uint16_t opposite_cond = cond == CND_CR ? CND_JMP : CND_CR;
	uint8_t instr = make_ancillory_instruction_code(jmp, encode_source_reg(R_PC, false));
	eeprom_values[make_address(MC_STEP2, instr) | cond] = reg_read(R_PC) | MAW;	//PC to to address
	eeprom_values[make_address(MC_STEP3, instr) | cond] = ME | reg_write(R_PC) | MCR; //Mem to PC
	//same if other condition is set
	eeprom_values[make_address(MC_STEP2, instr) | opposite_cond | cond] = reg_read(R_PC) | MAW;	//PC to to address
	eeprom_values[make_address(MC_STEP3, instr) | opposite_cond | cond] = ME | reg_write(R_PC) | MCR; //Mem to PC
	//if no condition, or just the opposite condition, increment the PCC to skip the param
	eeprom_values[make_address(MC_STEP2, instr) | opposite_cond] = PCC | MCR;
	eeprom_values[make_address(MC_STEP3, instr)] = PCC | MCR;
	std::cout << unsigned(instr) << "\t\t" << jump_instr_name(instr) << " [address]" << std::endl;
}

void make_jmp_instructions()
{
	uint8_t jmp = INSTR_JMP;

	//Non-conditional jmp
	make_reg_jmp_instr(INSTR_JMP, R_A);
	make_reg_jmp_instr(INSTR_JMP, R_B);
	make_reg_jmp_instr(INSTR_JMP, R_ALO);
	make_immediate_jmp_instr(INSTR_JMP);

	//For each existing non-conditional operation make equivilent versions for each condition
	std::map<uint16_t, uint32_t> insert;
	for (const auto p : eeprom_values)
	{
		insert[CND_CR | p.first] = p.second;
		insert[CND_JMP | p.first] = p.second;
		insert[CND_CR | CND_JMP | p.first] = p.second;		
	}
	eeprom_values.insert(insert.begin(), insert.end());

	//Conditional jumps
	//JZ
	make_cond_reg_jmp_instr(INSTR_JZ, R_A, CND_JMP);
	make_cond_reg_jmp_instr(INSTR_JZ, R_B, CND_JMP);
	make_cond_reg_jmp_instr(INSTR_JZ, R_ALO, CND_JMP);
	make_cond_immediate_jmp_instr(INSTR_JZ, CND_JMP);

	//JE
	make_cond_reg_jmp_instr(INSTR_JE, R_A, CND_JMP);
	make_cond_reg_jmp_instr(INSTR_JE, R_B, CND_JMP);
	make_cond_reg_jmp_instr(INSTR_JE, R_ALO, CND_JMP);
	make_cond_immediate_jmp_instr(INSTR_JE, CND_JMP);

	//JN
	make_cond_reg_jmp_instr(INSTR_JN, R_A, CND_JMP);
	make_cond_reg_jmp_instr(INSTR_JN, R_B, CND_JMP);
	make_cond_reg_jmp_instr(INSTR_JN, R_ALO, CND_JMP);
	make_cond_immediate_jmp_instr(INSTR_JN, CND_JMP);

	//JC
	make_cond_reg_jmp_instr(INSTR_JC, R_A, CND_CR);
	make_cond_reg_jmp_instr(INSTR_JC, R_B, CND_CR);
	make_cond_reg_jmp_instr(INSTR_JC, R_ALO, CND_CR);
	make_cond_immediate_jmp_instr(INSTR_JC, CND_CR);
}

void make_push_pop_instr(uint8_t reg)
{
	//push
	//sp on bus, dec
	//alo to sp and address
	//read source reg and write mem
	uint8_t instr = make_ancillory_instruction_code(INSTR_PUSH, encode_source_reg(reg, false));
	eeprom_values[make_address(MC_STEP2, instr)] = reg_read(R_SP) | alu_ctrl(ALU_DEC);	//SP--
	eeprom_values[make_address(MC_STEP3, instr)] = reg_read(R_ALO) | SPW | MAW;	//ALO to SP and Address
	eeprom_values[make_address(MC_STEP4, instr)] = reg_read(reg) | MW | MCR; //Write src_reg to memory
	std::cout << unsigned(instr) << "\t\tPUSH " << source_reg_name(reg, false) << std::endl;

	//pop
	//sp to address
	//mem to dest
	//sp to bus, inc
	//alo to sp
	instr = make_ancillory_instruction_code(INSTR_POP, encode_source_reg(reg, false));
	eeprom_values[make_address(MC_STEP2, instr)] = reg_read(R_SP) | MAW;	//SP to Address
	eeprom_values[make_address(MC_STEP3, instr)] = ME | encode_dest_reg(reg, false);	//Memory to dest
	eeprom_values[make_address(MC_STEP4, instr)] = reg_read(R_SP) | alu_ctrl(ALU_INC); //SP++
	eeprom_values[make_address(MC_STEP5, instr)] = reg_read(R_ALO) | reg_write(R_SP) | MCR; //ALO to SP
	std::cout << unsigned(instr) << "\t\tPOP " << dest_reg_name(reg, false) << std::endl;
}

void make_call_instr(uint8_t src_reg, bool deref)
{
	uint8_t instr = make_ancillory_instruction_code(INSTR_CALL, encode_source_reg(src_reg, false));
	std::cout << unsigned(instr) << "\t\tCALL " << source_reg_name(src_reg, deref) << std::endl;

	//push PC
	eeprom_values[make_address(MC_STEP2, instr)] = reg_read(R_SP) | alu_ctrl(ALU_DEC);	//SP--
	eeprom_values[make_address(MC_STEP3, instr)] = reg_read(R_ALO) | SPW | MAW;	//ALO to SP and Address
	eeprom_values[make_address(MC_STEP4, instr)] = reg_read(R_PC) | MW; //Write PC to memory
	//load into pc
	if (deref || src_reg == R_PC)
	{
		//dereference a register, or load an immediate from R_PC
		eeprom_values[make_address(MC_STEP5, instr)] = reg_read(src_reg) | MAW;
		eeprom_values[make_address(MC_STEP6, instr)] = ME | reg_write(R_PC) | MCR;
	}
	else
	{
		eeprom_values[make_address(MC_STEP5, instr)] = reg_read(src_reg) | reg_write(R_PC) | MCR;
	}
}

void make_ancillary_instructions()
{
	make_push_pop_instr(R_A);
	make_push_pop_instr(R_B);

	make_call_instr(R_A, false);
	make_call_instr(R_B, false);
	make_call_instr(R_ALO, false);
	make_call_instr(R_PC, false); //call immediate
	
	//ret
	eeprom_values[make_address(MC_STEP2, INSTR_RET)] = reg_read(R_SP) | MAW;	//SP to Address
	eeprom_values[make_address(MC_STEP3, INSTR_RET)] = ME | reg_write(R_PC);	//Memory to dest
	eeprom_values[make_address(MC_STEP4, INSTR_RET)] = reg_read(R_SP) | alu_ctrl(ALU_INC); //SP++
	eeprom_values[make_address(MC_STEP5, INSTR_RET)] = reg_read(R_ALO) | reg_write(R_SP) | MCR; //ALO to SP
	std::cout << unsigned(INSTR_RET) << "\t\tRET" << std::endl;

	//halt
	eeprom_values[make_address(MC_STEP2, INSTR_HALT)] = HLT;
	std::cout << unsigned(INSTR_HALT) << "\t\tHLT" << std::endl;

	//no op
	eeprom_values[make_address(MC_STEP2, INSTR_NOOP)] = MCR;
	std::cout << unsigned(INSTR_NOOP) << "\t\tNOOP" << std::endl;
}

void make_instructions()
{
	make_mov_instructions();
	make_alu_instructions();
	make_ancillary_instructions();
	make_jmp_instructions();
}

uint8_t get_eeprom_value(uint32_t ctrl_word, uint8_t eeprom)
{
	return flip_active_low_signals(ctrl_word) >> (24 - (eeprom * 8));
}

void write_value(uint8_t eeprom, uint16_t address, uint32_t ctrl_word)
{
	uint8_t val = flip_active_low_signals(ctrl_word) >> (24 - (eeprom * 8));

	std::cout << address << " : " << unsigned(val) << std::endl;
}

void make_eeprom(uint8_t eeprom)
{
	uint32_t nop = 0;

	for (uint16_t addr = 0; addr < EEPROM_SIZE; addr++)
	{
		if (step_no(addr) == 0)
			write_value(eeprom, addr, FETCH0);
		if (step_no(addr) == 1)
			write_value(eeprom, addr, FETCH1);

		auto it = eeprom_values.find(addr);
		if (it != eeprom_values.end())
			write_value(eeprom, addr, it->second);
	}
}

void generate_arduino_code()
{
	std::cout << "/***********************************/" << std::endl;
	std::cout << "#define CHIP0_NULL " << unsigned(get_eeprom_value(0, 0)) << std::endl;
	std::cout << "#define CHIP1_NULL " << unsigned(get_eeprom_value(0, 1)) << std::endl;
	std::cout << "#define CHIP2_NULL " << unsigned(get_eeprom_value(0, 2)) << std::endl;
	std::cout << "#define CHIP0_STEP0 " << unsigned(get_eeprom_value(FETCH0, 0)) << std::endl;
	std::cout << "#define CHIP0_STEP1 " << unsigned(get_eeprom_value(FETCH1, 0)) << std::endl;
	std::cout << "#define CHIP1_STEP0 " << unsigned(get_eeprom_value(FETCH0, 1)) << std::endl;
	std::cout << "#define CHIP1_STEP1 " << unsigned(get_eeprom_value(FETCH1, 1)) << std::endl;
	std::cout << "#define CHIP2_STEP0 " << unsigned(get_eeprom_value(FETCH0, 2)) << std::endl;
	std::cout << "#define CHIP2_STEP1 " << unsigned(get_eeprom_value(FETCH1, 2)) << std::endl;
	std::cout << "//\t\t" << eeprom_values.size() << " values" << std::endl;
	std::cout << "/***********************************/" << std::endl;

	std::cout << "#ifdef CHIP0" << std::endl;
	for (const auto p : eeprom_values)
		std::cout << "writeEEPROM(" << p.first << ", " << unsigned(get_eeprom_value(p.second, 0)) << ");" << std::endl;
	std::cout << "#endif //CHIP0" << std::endl;

	std::cout << "#ifdef CHIP1" << std::endl;
	for (const auto p : eeprom_values)
		std::cout << "writeEEPROM(" << p.first << ", " << unsigned(get_eeprom_value(p.second, 1)) << ");" << std::endl;
	std::cout << "#endif //CHIP1" << std::endl;

	std::cout << "#ifdef CHIP2" << std::endl;
	for (const auto p : eeprom_values)
		std::cout << "writeEEPROM(" << p.first << ", " << unsigned(get_eeprom_value(p.second, 2)) << ");" << std::endl;
	std::cout << "#endif //CHIP2" << std::endl;
}

int main()
{
	make_instructions();
	uint32_t val = flip_active_low_signals( eeprom_values[2104]);
	uint8_t j = val >> 8;
//	make_eeprom(0);
	generate_arduino_code();
    return 0;
}

