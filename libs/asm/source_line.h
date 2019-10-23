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
		ADC
		SUB
		SBC
		SFT
		CMP
		NOT
		AND
		OR
		XOR

		JMP A
		JMP [42]
		JMP 42
		JZ
		JE
		JN
		JC

		PUSH A
		POP A

		CALL A
		CALL 42

		RET
		HALT
		NOOP

*/
using OptionalString = std::optional<std::string>;

class SourceLine
{
public:
	static SourceLine Parse(const std::string& line);

	const std::string& OpCode() const {return mOpCode;}
	const std::optional<std::string>& Param1() const {return mParam1;}
	const std::optional<std::string>& Param2() const {return mParam2;}
	const std::optional<std::string>& Label() const { return mLabel; }

	void Print(std::ostream&) const;
private:

	SourceLine(const OptionalString& label,
			const std::string& op,
			const OptionalString& p1,
			const OptionalString& p2,
			const OptionalString& comment);

	OptionalString mLabel;
	std::string mOpCode;
	OptionalString mParam1;
	OptionalString mParam2;
	OptionalString mComment;
};

}