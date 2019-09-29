// ctrl_eeprom.cpp : Defines the entry point for the console application.
//

#include "constants.h"

#include <iostream>
#include <map>

std::map<uint16_t, uint32_t> eeprom_values;

void make_gp_reg_movs(uint8_t reg)
{
/*	uint8_t instr;
	for (uint8_t src = R_A; src <= R_B; src++)
	{
		if (src != reg)
		{
			//src -> reg
			instr = make_mov_instruction(reg, src);
			eeprom_values[make_address(MC_STEP2, instr)] = reg_write(reg) | reg_read(src) | MCR; //src reg to reg
		}

		//[src] -> reg
		instr = make_mov_instruction(reg, src| MEM);
		eeprom_values[make_address(MC_STEP2, instr)] = MAW | reg_read(src); //src reg to address reg
		eeprom_values[make_address(MC_STEP3, instr)] = reg_write(reg) | ME | MCR; //mem to reg
	}

	//IMM -> reg
	instr = make_mov_instruction(reg, IMM);
	eeprom_values[make_address(MC_STEP2, instr)] = MAW | PCE; //prog counter to address reg
	eeprom_values[make_address(MC_STEP3, instr)] = reg_write(reg) | ME | PCC | MCR; //mem to reg, proc counter inc
	*/
	}

void make_instructions()
{/*
	uint8_t instr;

	//mov reg <- reg
	for (uint8_t gp_reg = R_A; gp_reg <= R_B; gp_reg++)
	{
		make_gp_reg_movs(gp_reg);
	}
	
	//jmp reg
	for (uint8_t src = R_A; src <= R_B; src++)
	{
		//src -> prog counter
		instr = make_mov_instruction(R_PC, src);
		eeprom_values[make_address(MC_STEP2, instr)] = reg_write(R_PC) | reg_read(src) | MCR; //src reg to prog counter
	}
	//jmp imm
	instr = make_mov_instruction(R_PC, IMM);
	eeprom_values[make_address(MC_STEP2, instr)] = MAW | PCE; //prog counter to address reg
	eeprom_values[make_address(MC_STEP3, instr)] = reg_write(R_PC) | ME | MCR; //mem to proc counter

	//hlt
	instr = 7;
	eeprom_values[make_address(MC_STEP2, instr)] = HLT;
*/}

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

int main()
{
	auto i = make_mov_instruction(R_A, R_B, true);
	make_instructions();

	make_eeprom(0);

    return 0;
}

