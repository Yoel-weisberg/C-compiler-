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

#include "SymbolTable.h"
#include "Helper.h"

using namespace llvm;

#define INTEGER_SIZE 32
#define CHAR_SIZE 8

/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:

	virtual ~ExprAST() = default;
	virtual Value* codegen() = 0;
	ExprAST* kid = nullptr;
};

///// BinaryExprAST - Expression class for a binary operator.
//class BinaryExprAST : public ExprAST {
//	char Op;
//	std::unique_ptr<ExprAST> LHS, RHS;
//public:
//	BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
//		std::unique_ptr<ExprAST> RHS)
//		: Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
//	virtual Value* codegen() override;
//};
//


/// NumberExprAST - Expression class for numeric literals like "1.0".
class	FloatNumberExprAST : public ExprAST
{
private: 
	double _val;

public:
	FloatNumberExprAST(double val) : _val(val) {}
	virtual Value* codegen() override;
};


// IntegerNumberExprAST - class for numerical integers
class IntegerNumberExprAST : public ExprAST
{
private:
	int _val;
	int _size;
public: 
	IntegerNumberExprAST(int val) :_val(val), _size(INTEGER_SIZE) {}
	virtual Value* codegen() override;
};

class CharExprAST : public ExprAST
{
private:
	char _val;
	int _size;
public:
	CharExprAST(char val) : _val(val), _size(CHAR_SIZE) {}
	virtual Value* codegen() override;
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST
{
	std::string _name;

public:
	VariableExprAST(const std::string& Name) : _name(Name) {}
	virtual Value* codegen() override;
};

// code like int a = 5;
class AssignExprAST : public ExprAST
{
    std::string _varName;
    std::unique_ptr<ExprAST> _value;
	std::string _varType;
public:
    AssignExprAST(const std::string &VarName, std::unique_ptr<ExprAST> Value, std::string type)
        : _varName(VarName), _value(std::move(Value)), _varType(type) {}

    const std::string &getVarName() const { return _varName; }
    ExprAST *getValue() const { return _value.get(); }
	const std::string getType() { return _varType; }
	virtual Value* codegen() override;
};

// code like a = 5;
//class RedefinitionExprAst : public ExprAST
//{
//
//};
