#pragma once
#include "ParseToAst.h"
#include <asmjit/asmjit.h>
class ConvortASTtoASM
{
public:
	ConvortASTtoASM(ASTNode* root);
	static asmjit::JitRuntime runtime;    // Runtime for executing the generated code
	static asmjit::CodeHolder code;       // Code holder for storing the generated code
	static asmjit::x86::Assembler* assembler;

private:
	void OperationCaller(ASTNode* root);
	bool m_axInitilised;
};