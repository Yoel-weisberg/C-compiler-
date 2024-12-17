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
#include <variant>

#include "SymbolTable.h"
#include "CompilationErrors.h"
#include "Helper.h"

class Helper;


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


/// NumberExprAST - Expression class for numeric literals like "1.0".
class	FloatNumberExprAST : public ExprAST
{
private: 
	double _val;
	std::string _valAsStr;
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
	std::string _valAsStr;
public: 
	IntegerNumberExprAST(int val) :_val(val), _size(INTEGER_SIZE) {}
	virtual Value* codegen() override;
};

class CharExprAST : public ExprAST
{
private:
	char _val;
	int _size;
	std::string _valAsStr;
public:
	CharExprAST(char val) : _val(val), _size(CHAR_SIZE) {}
	virtual Value* codegen() override;
};


class ptrExprAST : public ExprAST
{
private:
	//int _addr; // Address of the "pointed to" variable
	int _size;
	std::string _valAsStr; // name of the pointed to variable
public:
	ptrExprAST(std::string val) : _valAsStr(val), _size(INTEGER_SIZE){}
	virtual Value* codegen() override;
};



// Some helpful info about array in LLVM --> https://stackoverflow.com/questions/38548680/confused-about-llvm-arrays
class arrExprAST : public ExprAST
{
private:
	uint64_t _size; // Number of elements in array	
	std::string _val; // Address to the first element in the array
	ArrayRef<uint64_t> _data; 
	std::string _name; // Variable name
	//std::string _type; // Type of variables in the array
	Type* _type;
public:
	arrExprAST(const std::string& type, std::string& size, const std::string& val, const std::string& name);
	virtual Value* codegen() override;
	void assignLLVMType(const std::string& type);
	void initArrayRef(const std::string& val, const std::string& type);
};

class BinaryExprAST : public ExprAST {
	Tokens_type Op;
	std::unique_ptr<ExprAST> LHS, RHS;

public:
	BinaryExprAST(Tokens_type Op, std::unique_ptr<ExprAST> LHS,
		std::unique_ptr<ExprAST> RHS)
		: Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

	Value* codegen() override;
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

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
	std::string Callee;
	std::vector<std::unique_ptr<ExprAST>> Args;

public:
	CallExprAST(const std::string& Callee,
		std::vector<std::unique_ptr<ExprAST>> Args)
		: Callee(Callee), Args(std::move(Args)) {}

	Value* codegen() override;
};

// code like a = 5;
//class RedefinitionExprAst : public ExprAST
//{
//
//};
