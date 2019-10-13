#include "gtest/gtest.h"
#include <asm/instruction.h>

TEST(Parse, add)
{
	Cpu::Instruction::Parse("label: MOV A, B		;blah");
	Cpu::Instruction::Parse("MOV A, B");
	Cpu::Instruction::Parse("RET");
	Cpu::Instruction::Parse("RET ;ret");
	Cpu::Instruction::Parse("_R: RET ;ret");
	Cpu::Instruction::Parse("PUSH [B]");
	Cpu::Instruction::Parse("POP [B]	; comment");
}
