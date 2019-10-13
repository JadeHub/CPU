#pragma once

#include <ostream>
#include <string>
#include <optional>

namespace Cpu {

/*
label:	MOV A, [42]
		MOV A, [#label]
		MOV A, B
		MOV A, 42
		MOV A, #label
		MOV [42], A
		MOV [A], B
		
		ADD A
		ADD [A]
		ADD 42
		ADD [42]

		JMP A
		JMP [42]
		JMP 42

		PUSH A
		POP A

		CALL A
		CALL 42

		RET
		HALT
		NOOP

*/

class Parameter
{

};

using OptionalString = std::optional<std::string>;

class Instruction
{
public:

	static Instruction Parse(const std::string& line);

	void Print(std::ostream&);

	uint8_t Size() const { return 1; }
private:

	//label: MOV A, [12]


	OptionalString mLabel;
	std::string mInstr;
	OptionalString mParam1;
	OptionalString mParam2;
	OptionalString mComment;
};

}