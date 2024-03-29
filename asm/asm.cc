#include <asm/source_line.h>
#include <asm/program.h>

#include <iostream>
#include <string>

int main(int argc, char** args)
{
	Cpu::Program p;
	while (!std::cin.eof())
	{
		std::string s;
		std::getline(std::cin, s);

		p.AddLine(Cpu::SourceLine::Parse(s));
	}

	const auto& mc = p.MachineCode();

	for (const auto b : mc)
	{
		std::cout << (uint32_t)b << std::endl;
	}

    return 0;
}

