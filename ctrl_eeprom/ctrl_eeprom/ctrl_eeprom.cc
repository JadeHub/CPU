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
	uint8_t instr = make_mov_instruction(encode_dest_reg(R_PC, true), encode_source_reg(source_reg, false));
	eeprom_values[make_address(MC_STEP2, instr)] = MAW | reg_read(R_PC); //program counter to address reg
	eeprom_values[make_address(MC_STEP3, instr)] = reg_read(source_reg) | MW | PCC | MCR; //source_reg to mem, pcc
	print_mov_instruction(instr, "[42]", source_reg_name(source_reg, false), "Move to litteral address");
}

//MOV DEST, [address]
//eg MOV A, [12]
void make_load_immediate_addr_instr(uint8_t dest_reg)
{
	uint8_t instr = make_mov_instruction(encode_dest_reg(dest_reg, false), encode_source_reg(R_PC, true));
	eeprom_values[make_address(MC_STEP2, instr)] = MAW | reg_read(R_PC); //program counter to address reg
	eeprom_values[make_address(MC_STEP3, instr)] = ME | MAW | PCC; //memory to address reg, PCC
	eeprom_values[make_address(MC_STEP4, instr)] = reg_write(dest_reg) | ME | MCR; //memory to dest_reg
	print_mov_instruction(instr, dest_reg_name(dest_reg, false), "[42]", "Move from litteral address");
}

//MOV DEST, immediate
//eg MOV A, 12
void make_load_immediate_instr(uint8_t dest_reg)
{
	uint8_t instr = make_mov_instruction(encode_dest_reg(dest_reg, false), encode_source_reg(R_PC, false));
	eeprom_values[make_address(MC_STEP2, instr)] = MAW | reg_read(R_PC); //program counter to address reg
	eeprom_values[make_address(MC_STEP3, instr)] = reg_write(dest_reg) | ME | PCC | MCR; //mem to dest_reg, PCC
	print_mov_instruction(instr, dest_reg_name(dest_reg, false), "42", "Move litteral");
}

void make_reg_reg_mov_instr(uint8_t src_reg, bool deref_src, uint8_t dest_reg, bool deref_dest)
{
	assert(!(deref_dest && deref_src));
	uint8_t instr = make_mov_instruction(encode_dest_reg(dest_reg, deref_dest), encode_source_reg(src_reg, deref_src));

	print_mov_instruction(instr, dest_reg_name(dest_reg, deref_dest), source_reg_name(src_reg, deref_src), "\t;Move");
	if (deref_src)
	{
		//mov [src] to dest
		instr = make_mov_instruction(encode_dest_reg(dest_reg, deref_dest), encode_source_reg(src_reg, deref_src));
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
		instr = make_mov_instruction(encode_dest_reg(dest_reg, false), encode_source_reg(src_reg, false));
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
JZ:				1	1	0	0	1	<- SRC ->
JE:				1	1	0	1	0	<- SRC -> xD0
JN:				1	1	1	0	0	<- SRC -> xE0
JC:				1	1	0	1	1	<- SRC -> xD8
JMP:			1	1	1	0	1	<- SRC -> xE8
*/

void make_jmp_instructions(uint8_t source_reg, bool deref)
{
	uint8_t jz = 0xC8;
	uint8_t je = 0xD0;
	uint8_t jn = 0xE0;
	uint8_t jc = 0xD8;
	uint8_t jmp = 0xE8;
	
	uint8_t instr = jz & encode_source_reg(source_reg, deref);

}

void make_push_pop_instr(uint8_t reg)
{
	uint8_t push = 0xF0;
	uint8_t pop = 0xF8;

	//push
	//sp on bus, dec
	//alo to sp and address
	//read source reg and write mem
	uint8_t instr = push & encode_source_reg(reg, false);
	eeprom_values[make_address(MC_STEP2, instr)] = reg_read(R_SP) | alu_ctrl(ALU_DEC);	//SP--
	eeprom_values[make_address(MC_STEP3, instr)] = reg_read(R_ALO) | SPW | MAW;	//ALO to SP and Address
	eeprom_values[make_address(MC_STEP4, instr)] = reg_read(reg) | MW | MCR; //Write src_reg to memory
	std::cout << unsigned(instr) << "\t\tPUSH " << source_reg_name(reg, false) << std::endl;

	//pop
	//sp to address
	//mem to dest
	//sp to bus, inc
	//alo to sp
	instr = pop & encode_dest_reg(reg, false);
	eeprom_values[make_address(MC_STEP2, instr)] = reg_read(R_SP) | MAW;	//SP to Address
	eeprom_values[make_address(MC_STEP3, instr)] = ME | encode_dest_reg(reg, false);	//Memory to dest
	eeprom_values[make_address(MC_STEP4, instr)] = reg_read(R_SP) | alu_ctrl(ALU_INC); //SP++
	eeprom_values[make_address(MC_STEP5, instr)] = reg_read(R_ALO) | reg_write(R_SP) | MCR; //ALO to SP
	std::cout << unsigned(instr) << "\t\tPOP " << dest_reg_name(reg, false) << std::endl;
}

void make_call_instr(uint8_t src_reg, bool deref)
{
	//call
	uint8_t call = 0xC0;
	uint8_t instr = call & encode_source_reg(src_reg, deref);
	std::cout << unsigned(instr) << "\t\tCALL " << source_reg_name(src_reg, deref) << std::endl;

	//push PC
	eeprom_values[make_address(MC_STEP2, instr)] = reg_read(R_SP) | alu_ctrl(ALU_DEC);	//SP--
	eeprom_values[make_address(MC_STEP3, instr)] = reg_read(R_ALO) | SPW | MAW;	//ALO to SP and Address
	eeprom_values[make_address(MC_STEP4, instr)] = reg_read(R_PC) | MW; //Write PC to memory
	//load pc
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
	uint8_t instr = 0xFA;
	eeprom_values[make_address(MC_STEP2, instr)] = reg_read(R_SP) | MAW;	//SP to Address
	eeprom_values[make_address(MC_STEP3, instr)] = ME | reg_write(R_PC);	//Memory to dest
	eeprom_values[make_address(MC_STEP4, instr)] = reg_read(R_SP) | alu_ctrl(ALU_INC); //SP++
	eeprom_values[make_address(MC_STEP5, instr)] = reg_read(R_ALO) | reg_write(R_SP) | MCR; //ALO to SP
	std::cout << unsigned(instr) << "\t\tRET" << std::endl;

	//halt
	instr = 0xF9;
	eeprom_values[make_address(MC_STEP2, instr)] = HLT;
	std::cout << unsigned(instr) << "\t\tHLT" << std::endl;

	//no op
	instr = 0xFF;
	eeprom_values[make_address(MC_STEP2, instr)] = MCR;
	std::cout << unsigned(instr) << "\t\tNOOP" << std::endl;

}

void make_instructions()
{
	make_mov_instructions();
	make_alu_instructions();
	make_ancillary_instructions();
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

void generate_arduino_code(uint8_t eeprom)
{
	std::cout << "/***********************************/" << std::endl;
	std::cout << eeprom_values.size() << " values" << std::endl;
	std::cout << "// STEP0 = " << unsigned(get_eeprom_value(FETCH0, eeprom)) << std::endl;
	std::cout << "// STEP1 = " << unsigned(get_eeprom_value(FETCH1, eeprom)) << std::endl;
	for (const auto p : eeprom_values)
	{
		std::cout << "writeEEPROM(" << p.first << ", " << unsigned(get_eeprom_value(p.second, eeprom)) << ");" << std::endl;
	}
}

int main()
{
	make_instructions();
//	make_eeprom(0);
	generate_arduino_code(2);
    return 0;
}

