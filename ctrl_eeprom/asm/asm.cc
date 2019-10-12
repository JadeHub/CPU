#include "instruction.h"

#include <iostream>
#include <string>

int main(int argc, char** args)
{
	Cpu::Instruction::Parse("label: MOV A, B		;blah");
	Cpu::Instruction::Parse("MOV A, B");
	Cpu::Instruction::Parse("RET");
	Cpu::Instruction::Parse("RET ;ret");
	Cpu::Instruction::Parse("_R: RET ;ret");
	Cpu::Instruction::Parse("PUSH [B]");	
	Cpu::Instruction::Parse("POP [B]	; comment");
/*
	while (!std::cin.eof())
	{
		std::string s;
		std::getline(std::cin, s);

		std::cout << s << std::endl;
	}*/
    return 0;
}

