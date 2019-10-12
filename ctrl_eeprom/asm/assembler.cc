#include "assembler.h"

namespace Cpu
{

void Assembler::AddLine(const std::string& line)
{
	uint8_t address = mNextInstruction;
	mInstructions[address] = Instruction::Parse(line);
	mNextInstruction = address + mInstructions[address].Size();
}

std::map<uint8_t, uint8_t> Assembler::MachineCode() const
{
	std::map<uint8_t, uint8_t> result;

	return result;
}

void Assembler::ResolveLabels()
{

}

}