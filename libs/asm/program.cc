#include "program.h"

namespace Cpu
{

void Program::AddLine(const SourceLine& line)
{
	const auto& instr = mInstructions.emplace(std::make_pair(mNextAddress, Instruction(line))).first->second;
	if (line.Label())
		mLabels[*line.Label()] = mNextAddress;
	mNextAddress += instr.EncodedLength();
}

std::vector<uint8_t> Program::MachineCode() const
{
	auto labelLookup = [this](const std::string& label)
	{
		auto it = this->mLabels.find(label);
		return it->second;
	};

	std::vector<uint8_t> result;

	for (const auto& instr : mInstructions)
	{
		const auto bytes = instr.second.Encode(labelLookup);
		result.insert(result.end(), bytes.begin(), bytes.end());
	}

	return result;
}


}