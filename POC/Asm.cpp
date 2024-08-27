#include "Asm.h"

asmjit::CodeHolder ConvortASTtoASM::code;
asmjit::JitRuntime ConvortASTtoASM::runtime;
asmjit::x86::Assembler* ConvortASTtoASM::assambler = nullptr;


ConvortASTtoASM::ConvortASTtoASM(ASTNode* root) :
	m_axInitilised(false)
{
	code.init(runtime.environment());
	assambler = new asmjit::x86::Assembler(&code);
	OperationCaller(root);
}

ASTNode* ConvortASTtoASM::OperationCaller(ASTNode* root)
{
	asmjit::x86::Assembler a(&code);
	a.add(asmjit::x86::eax, 3);
	if (root->haveKids());
	{
		if (root->left->haveKids())
		{
			OperationCaller(root->left);
		}
		if (root->right->haveKids())
		{
			OperationCaller(root->left);
		}
		if (!root->left->haveKids())
		{
			// case where the two kids, are leafs. and ax was not initlised
			if (!m_axInitilised && !root->left->haveKids() && !root->right->haveKids())
			{
				m_axInitilised = true;
				a.mov(asmjit::x86::eax, root->left->toInt());
			}
			doOperation(root, )
		}
	}
}

void ConvortASTtoASM::doOperation(ASTNode* root, bool side)
{
	int value = side ? root->left->toInt() : root->right->toInt();
	switch (root->token)
	{
	case ADDITION:
		a.add(asmjit::x86::eax, value);
		break;
	case SUBTRACTION:
		a.sub(asmjit::x86::eax, value);
		break;
	case MULTIPLICATION:
		a.mov(asmjit::x86::ebx, value);
		a.mul(asmjit::x86::ebx);
		break;
	case DIVISION:
		a.mov(asmjit::x86::ebx, value);
		a.div(asmjit::x86::ebx);
		break;
	default:
		break;
	}
}
