#include "gmock/gmock.h"
#include <asm/source_line.h>
#include <asm/instruction.h>

namespace Cpu { namespace Test {

using namespace ::testing;

void ExpectEncoding(const std::string& code, const std::vector<uint8_t>& expected)
{
	Instruction instr(SourceLine::Parse(code));

	EXPECT_EQ(instr.Encode([](const std::string&) {return 0;}), (expected));
}


TEST(Instruction, ADD)
{
	ExpectEncoding("ADD A", { 16 });
	ExpectEncoding("ADD [A]", { 17 });
	ExpectEncoding("ADD 122", { 22, 122 });
}

TEST(Instruction, AND)
{
	ExpectEncoding("AND A", { 72 });
	ExpectEncoding("AND [A]", { 73 });
	ExpectEncoding("AND 122", { 78, 122 });
}

TEST(Instruction, JMP_REG)
{
	ExpectEncoding("JMP A", { 232 });
	ExpectEncoding("JMP B", { 234 });
	ExpectEncoding("JMP ALO", { 236 });

	ExpectEncoding("JE A", { 208 });
	ExpectEncoding("JE B", { 210 });
	ExpectEncoding("JE ALO", { 212 });
}

TEST(Instruction, JMP_IMM)
{
	ExpectEncoding("JMP 12", { 238, 12 });
	ExpectEncoding("JE 12", { 214, 12 });
}

TEST(Instruction, MOV_REG_REG)
{
	ExpectEncoding("MOV B, A", {144});
	ExpectEncoding("MOV A, B", { 130});
	ExpectEncoding("MOV A, ALO", {132});
	ExpectEncoding("MOV OUT, A", {168});
}

TEST(Instruction, MOV_REG_REG_DEREF)
{
	ExpectEncoding("MOV B, [A]", {145});
	ExpectEncoding("MOV OUT, [A]", { 169 });
}

TEST(Instruction, MOV_ADDR_REG)
{
	ExpectEncoding("MOV [42], A", { 176, 42 });
}

TEST(Instruction, MOV_REG_ADDR)
{
	ExpectEncoding("MOV A, [42]", { 135, 42 });
}

TEST(Instruction, MOV_REG_DEREF_REG)
{
	ExpectEncoding("MOV [A], B", { 138});
	ExpectEncoding("MOV [A], ALO", { 140 });
}

}}