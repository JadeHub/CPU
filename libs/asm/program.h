#pragma once

#include "source_line.h"
#include "instruction.h"

#include <string>
#include <map>
#include <vector>

namespace Cpu
{

class Program
{
public:
	void AddLine(const SourceLine& line);
	std::vector<uint8_t> MachineCode() const;
private:
	//label to address
	std::map<std::string, uint8_t> mLabels;
	//address to instruction
	std::map<uint8_t, Instruction> mInstructions;
	uint8_t mNextAddress = 0;
};

}