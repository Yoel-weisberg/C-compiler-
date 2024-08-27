#pragma once
#include <asmjit/asmjit.h>
#include "ParseToAst.h"

class ConvortASTtoASM
{
public:
	ConvortASTtoASM(ASTNode* root);
	static asmjit::x86::Assembler* assambler;
	static 	asmjit::JitRuntime runtime;    // Runtime for executing the generated code
	static asmjit::CodeHolder code;       // Code holder for storing the generated code
private:
	ASTNode* OperationCaller(ASTNode* root);
	void doOperation(ASTNode* root, bool side);
	bool m_axInitilised;
};