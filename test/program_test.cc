#include "gmock/gmock.h"
#include <asm/program.h>

namespace Cpu { namespace Test {

using namespace ::testing;

TEST(Program, foo)
{
	Program p;

	p.AddLine(SourceLine::Parse("ADD A"));
	p.AddLine(SourceLine::Parse("label: ADD A"));
	p.AddLine(SourceLine::Parse("JC #label"));
	p.AddLine(SourceLine::Parse("HLT"));

	std::vector<uint8_t> expected = {16, 16, 222, 1, 249};
	EXPECT_EQ(p.MachineCode(), expected);
}

}}