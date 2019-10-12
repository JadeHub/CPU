#include "instruction.h"

#include <cwctype>
#include <vector>

namespace Cpu
{

const std::string white_space = "\r\n\t ";

size_t SkipWhiteSpace(const std::string& str, size_t pos)
{
	if (pos >= str.length())
		throw std::runtime_error("parser error");
	return str.find_first_not_of(white_space, pos);
}

enum State { LABEL, OP, P1, P2};

bool ChangeState(State state, char c)
{
	if (c == ';')
		return true;
	if (state == LABEL && c == ':')
		return true;
	if (state == OP && std::iswspace(c))
		return true;
	if (state == P1 && (std::iswspace(c) || c == ','))
		return true;
	if (state == P2 && std::iswspace(c))
		return true;
	return false;
}

State NextState(State state, char c)
{
	if (state == LABEL)
		return OP;
	if (state == OP)
		return P1;
	return P2;
}

bool SkipChar(char c)
{
	return std::iswspace(c) || c == ':' || c == ',';
}

void ParseLine(const std::string& line)
{
	State state = line.find_first_of(':') != std::string::npos ? LABEL : OP;
	std::string op;
	OptionalString label, p1, p2, comment;

	std::string buff;
	const char* c = line.c_str();
	do{
		if (*c == ';')
		{
			comment = c+1;
			break;
		}
		if (ChangeState(state, *c) || *(c) == 0)
		{
			if (state == LABEL)
				label = buff;
			else if (state == OP)
				op = buff;
			else if (state == P1)
				p1 = buff;
			else if (state == P2)
				p2 = buff;
			buff.resize(0);
			
			//skip white space
			while (*c && SkipChar(*c))
				c++;
			if (*c == 0)
				break;

			state = NextState(state, *c);
		}
		else
		{
			buff.append(1, *c);
			c++;
		}
	}while(1);
}

Instruction Instruction::Parse(const std::string& line)
{
	ParseLine(line);
	return Instruction();
}

void Instruction::Print(std::ostream& str)
{
	if (mLabel)
		str << *mLabel << ":";
	str << "\t\t" << mInstr;
	if (mParam1)
		str << *mParam1;
	if (mParam2)
		str << ", " << *mParam2;
	if (mComment)
		str << "\t\t; " << *mComment;
}

}