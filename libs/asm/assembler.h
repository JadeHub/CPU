#pragma once

#include "instruction.h"

#include <string>
#include <map>
#include <vector>

namespace Cpu
{

class Assembler
{
public:

	void AddLine(const std::string& line);
	std::map<uint8_t, uint8_t> MachineCode() const;
private:
	void ResolveLabels();
	//label to address
	std::map<std::string, uint8_t> mLabels;
	//address to instruction
	std::map<uint8_t, Instruction> mInstructions;
	uint8_t mNextInstruction = 0;
};

}