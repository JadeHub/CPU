#pragma once

#include "source_line.h"

#include <string>
#include <optional>
#include <functional>

namespace Cpu {

class SourceLine;

class Parameter
{
public:
	Parameter(const std::string& param);

	bool IsLiteral() const {return mLiteral.has_value();}
	uint8_t Literal() const {return *mLiteral;}

	bool IsRegister() const {return mReg.has_value();}
	uint8_t Register() const {return *mReg;}

	bool IsDereferenced() const {return mDeref;}
	bool IsLabel() const {return mLabel.has_value();}
	const std::string& Label() const {return *mLabel;}

private:
	std::optional<uint8_t> mLiteral;
	std::optional<uint8_t> mReg;
	std::optional<std::string> mLabel;
	bool mDeref = false;
};

class Instruction
{
public:
	Instruction(const SourceLine& line);
	uint8_t EncodedLength() const;
	std::vector<uint8_t> Encode(std::function<uint8_t (const std::string&)> resolveLabel) const;

private:
	//static void CheckCond(bool cond, const char* error);

	std::vector<uint8_t> EncodeMov() const;
	std::vector<uint8_t> EncodeJump(std::function<uint8_t(const std::string&)> resolveLabel) const;
	std::vector<uint8_t> EncodeAlu() const;
	std::vector<uint8_t> EncodeAncillary() const;

	bool IsMovOp() const;
	bool IsAluOp() const;
	bool IsJumpOp() const;
	bool IsAncillaryOpCode() const;

	SourceLine mLine;
	std::optional<Parameter> mParam1;
	std::optional<Parameter> mParam2;
};

}