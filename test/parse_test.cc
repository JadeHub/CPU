#include "gmock/gmock.h"
#include <asm/source_line.h>
#include <asm/instruction.h>

namespace Cpu { namespace Test {

TEST(Parse, add)
{
	SourceLine::Parse("MOV B, A");
	SourceLine::Parse("label: MOV	 A, B		;blah");
	SourceLine::Parse("MOV A, B");
	SourceLine::Parse("RET");
	SourceLine::Parse("RET ;ret");
	SourceLine::Parse("_R: RET ;ret");
	SourceLine::Parse("PUSH [B]");
	SourceLine::Parse("POP [B]	; comment");
}

}}