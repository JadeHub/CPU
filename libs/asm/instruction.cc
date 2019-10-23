#include "instruction.h"
#include "source_line.h"
#include <ctrl/constants.h>

namespace Cpu
{

namespace {

uint8_t GetAncillaryOpCode(const std::string& op)
{
	if (op == "PUSH")
		return INSTR_PUSH;
	if (op == "POP")
		return INSTR_POP;
	if (op == "CALL")
		return INSTR_CALL;
	if (op == "RET")
		return INSTR_RET;
	if (op == "HTL")
		return INSTR_HALT;
	if (op == "NOOP")
		return INSTR_NOOP;
	return 0xFF;
}

uint8_t GetJumpOpCode(const std::string& op)
{
	if (op == "JMP")
		return INSTR_JMP;
	if (op == "JZ")
		return INSTR_JZ;
	if (op == "JE")
		return INSTR_JE;
	if (op == "JC")
		return INSTR_JC;
	if (op == "JN")
		return INSTR_JN;
	return 0xFF;
}

uint8_t GetAluOpCode(const std::string& op)
{
	if (op == "INC")
		return ALU_INC;
	if (op == "DEC")
		return ALU_DEC;
	if (op == "ADD")
		return ALU_ADD;
	if (op == "ADC")
		return ALU_ADC;
	if (op == "SUB")
		return ALU_SUB;
	if (op == "SBC")
		return ALU_SBC;
	if (op == "CMP")
		return ALU_CMP;
	if (op == "SFT")
		return ALU_SFT;
	if (op == "NOT")
		return ALU_NOT;
	if (op == "AND")
		return ALU_AND;
	if (op == "OR")
		return ALU_OR;
	if (op == "XOR")
		return ALU_XOR;
	return 0xFF;
}
}

Parameter::Parameter(const std::string& p)
{
	std::string param = p;

	if (p[0] == '[')
	{
		if (p[p.size()-1] != ']')
			throw std::runtime_error("where's the ']'?");
		param = p.substr(1, p.size()-2);
		mDeref = true;
	}

	//Params: A, [A], 42, [42], #label, [#label]
	//Registers: A, B, ALO, OUT
	if (param == "A")
	{
		mReg = R_A;
	}
	else if (param == "B")
	{
		mReg = R_B;
	}
	else if (param == "ALO")
	{
		mReg = R_ALO;
	}
	else if (param == "OUT")
	{
		mReg = R_OUT;
	}
	else if (param[0] == '#')
	{
		mLabel = param.substr(1);
	}
	else
	{
		mLiteral = std::stoi(param);
	}
}

Instruction::Instruction(const SourceLine& line)
:	mLine(line)
{
	if (line.Param1())
		mParam1 = Parameter(*line.Param1());
	if (line.Param2())
		mParam2 = Parameter(*line.Param2());
}

bool Instruction::IsMovOp() const
{
	return mLine.OpCode() == "MOV";
}

bool Instruction::IsAluOp() const
{
	return GetAluOpCode(mLine.OpCode()) != 0xFF;
}

bool Instruction::IsJumpOp() const
{
	return GetJumpOpCode(mLine.OpCode()) != 0xFF;
}

bool Instruction::IsAncillaryOpCode() const
{
	return GetAncillaryOpCode(mLine.OpCode()) != 0xFF;
}

uint8_t Instruction::EncodedLength() const
{
	if(mParam2)
		return mParam2->IsLiteral() ? 2 : 1;
	if (mParam1)
		return mParam1->IsLiteral() ? 2 : 1;
	return 1;
}

std::vector<uint8_t> Instruction::Encode(std::function<uint8_t(const std::string&)> resolveLabel) const
{
	if (IsMovOp())
	{
		return EncodeMov();
	}
	if (IsJumpOp())
	{
		return EncodeJump(resolveLabel);
	}
	if (IsAluOp())
	{
		return EncodeAlu();
	}
	if (IsAncillaryOpCode())
	{
		return EncodeAncillary();
	}
	return std::vector<uint8_t>();
}


std::vector<uint8_t> Instruction::EncodeAncillary() const
{
	return std::vector<uint8_t>();
}

std::vector<uint8_t> Instruction::EncodeAlu() const
{
	assert(mParam1 && !mParam2);
	const auto op = GetAluOpCode(mLine.OpCode());

	if (mParam1->IsRegister())
	{
		//Add A
		//Add [A]
		return {
			make_alu_instruction_code(
				GetAluOpCode(mLine.OpCode()),
				encode_source_reg(mParam1->Register(), mParam1->IsDereferenced())) };
		
	}
	else if (mParam1->IsLiteral())
	{
		//Add 12
		return {
			make_alu_instruction_code(
				GetAluOpCode(mLine.OpCode()),
				encode_source_reg(R_PC, false)),
			mParam1->Literal() };
	}
}

std::vector<uint8_t> Instruction::EncodeJump(std::function<uint8_t(const std::string&)> resolveLabel) const
{
	assert(mParam1 && !mParam2);

	if (mParam1->IsRegister())
	{
		return {
			make_ancillory_instruction_code(
				GetJumpOpCode(mLine.OpCode()),
				encode_source_reg(mParam1->Register(), false))};
	}
	else if (mParam1->IsLiteral())
	{
		return {
			make_ancillory_instruction_code(
				GetJumpOpCode(mLine.OpCode()),
				encode_source_reg(R_PC, false)),
			mParam1->Literal()};
	}
	else if (mParam1->IsLabel())
	{
		return {
			make_ancillory_instruction_code(
				GetJumpOpCode(mLine.OpCode()),
				encode_source_reg(R_PC, false)),
			resolveLabel(mParam1->Label())};
	}
	assert(false);
	return std::vector<uint8_t>();
}

std::vector<uint8_t> Instruction::EncodeMov() const
{
	const auto& source = *mParam2;
	const auto& dest = *mParam1;

	if (dest.IsDereferenced())
	{
		//storing
		//MOV [imm], reg
		//MOV [reg], reg
		if (dest.IsLiteral())
		{
			//MOV [imm], reg
			assert(source.IsRegister());
			return {
				make_mov_instruction_code(
					encode_dest_reg(R_PC, true),
					encode_source_reg(source.Register(), false)),
				dest.Literal()};
		}
		else
		{
			//MOV [reg], reg
			assert(source.IsRegister());
			assert(!source.IsDereferenced());
			return {
				make_mov_instruction_code(
					encode_dest_reg(dest.Register(), true),
					encode_source_reg(source.Register(), false))};
		}
	}
	else
	{
		//loading
		//MOV reg, reg
		//MOV reg, [reg]
		//MOV reg, imm
		//MOV reg, [imm]
		if (source.IsRegister())
		{
			return {
				make_mov_instruction_code(
					encode_dest_reg(dest.Register(), false),
					encode_source_reg(source.Register(), source.IsDereferenced()))};
		}
		else
		{
			assert(source.IsLiteral());
			return {
				make_mov_instruction_code(
					encode_dest_reg(dest.Register(), false),
					encode_source_reg(R_PC, source.IsDereferenced())),
				source.Literal()};
		}
	}
	assert(false);
	return std::vector<uint8_t>();
}

}