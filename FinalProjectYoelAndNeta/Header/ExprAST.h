#pragma once
#include <iostream>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"


#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "SymbalTable.h"
#include "Helper.h"

using namespace llvm;


/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:

	virtual ~ExprAST() = default;
	virtual Value* codegen() = 0;
	ExprAST* kid = nullptr;
};


/// NumberExprAST - Expression class for numeric literals like "1.0".
class	FloatNumberExprAST : public ExprAST {
	double Val;

public:
	FloatNumberExprAST(double Val) : Val(Val) {}
	virtual Value* codegen() override;
};


/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
	std::string Name;

public:
	VariableExprAST(const std::string& Name) : Name(Name) {}
	virtual Value* codegen() override;
};

// code like int a = 5;
class AssignExprAST : public ExprAST {
    std::string _VarName;
    std::unique_ptr<ExprAST> _Value;
	std::string _varType;
public:
    AssignExprAST(const std::string &VarName, std::unique_ptr<ExprAST> Value, std::string type)
        : _VarName(VarName), _Value(std::move(Value)), _varType(type) {}

    const std::string &getVarName() const { return _VarName; }
    ExprAST *getValue() const { return _Value.get(); }
	const std::string getType() { return _varType; }
	virtual Value* codegen() override;
};


class IfExprAST : public ExprAST {
private:
	std::unique_ptr<ExprAST> Cond, Then, Else;
public:
	IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
		std::unique_ptr<ExprAST> Else)
		: Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

	Value* codegen() override;
};

class PrototypeAST {
	std::string Name;
	std::vector<std::string> Args;

public:
	PrototypeAST(const std::string& Name, std::vector<std::string> Args)
		: Name(Name), Args(std::move(Args)) {}

	Function* codegen();
	const std::string& getName() const { return Name; }
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
	std::unique_ptr<PrototypeAST> Proto;
	std::unique_ptr<ExprAST> Body;

public:
	FunctionAST(std::unique_ptr<PrototypeAST> Proto,
		std::unique_ptr<ExprAST> Body)
		: Proto(std::move(Proto)), Body(std::move(Body)) {}

	Function* codegen();
};

// code like a = 5;
//class RedefinitionExprAst : public ExprAST
//{
//
//};
