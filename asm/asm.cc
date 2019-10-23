#include <asm/source_line.h>

#include <iostream>
#include <string>

int main(int argc, char** args)
{
	while (!std::cin.eof())
	{
		std::string s;
		std::getline(std::cin, s);

		Cpu::SourceLine line = Cpu::SourceLine::Parse(s);

		std::cout << s << std::endl;
	}
    return 0;
}

