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

#include "CompilationErrors.h"
#include "Helper.h"

class Helper;

struct FuncArg
{
	std::string Type;
	std::string Name;
};

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
	std::string _name;
	double _val;
	std::string _valAsStr;
public:
	FloatNumberExprAST(double val, const std::string& name = "") : _val(val), _name(name) {}
	virtual Value* codegen() override;
};


// IntegerNumberExprAST - class for numerical integers
class IntegerNumberExprAST : public ExprAST
{
private:
	int _val;
	int _size;
	std::string _valAsStr;
	std::string _name;
public: 
	IntegerNumberExprAST(int val, const std::string& name) :_val(val), _size(INTEGER_SIZE), _name(name) {}
	virtual Value* codegen() override;
};

class CharExprAST : public ExprAST
{
private:
	char _val;
	int _size;
	std::string _valAsStr;
	std::string _name;
public:
	CharExprAST(char val, const std::string& name) : _val(val), _size(CHAR_SIZE), _name(name) {}
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
	std::string _typeStr; // Type of variables in the array
	Type* _type;
public:
	arrExprAST(const std::string& type, std::string& size, const std::string& val, const std::string& name) : _size(std::stoull(size)), _name(name), _typeStr(type)
	{
		assignLLVMType(type);
		initArrayRef(val, type);
	}
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
	std::unique_ptr<ExprAST> _cond; // "if condition" content 
	std::unique_ptr<ExprAST> _then; // "if" content
	std::unique_ptr<ExprAST> _else; // "else" content 
public:
	IfExprAST(std::unique_ptr<ExprAST> condExpr, std::unique_ptr<ExprAST> thenExpr,
		std::unique_ptr<ExprAST> elseExpr)
		: _cond(std::move(condExpr)), _then(std::move(thenExpr)), _else(std::move(elseExpr)) {}

	Value* codegen() override;
};



class whileLoopExprAST : public ExprAST
{
private:
	std::unique_ptr<ExprAST> _cond; // "while" loop codition content
	std::unique_ptr<ExprAST> _content; // loop content
public:
	whileLoopExprAST(std::unique_ptr<ExprAST> cond, std::unique_ptr<ExprAST> content) : _cond(std::move(cond)), _content(std::move(content)){}
	Value* codegen() override;

};


class binaryEprAST : public ExprAST
{

};

class PrototypeAST {
	std::string Name;
	std::vector<FuncArg> Args;
	std::string returnType;
public:
	PrototypeAST(const std::string& Name, std::vector<FuncArg> Args, std::string returnType)
		: Name(Name), Args(std::move(Args)), returnType(returnType) {}

	Function* codegen();
	const std::string& getName() const { return Name; }
	const std::string& getReturnType() const { return returnType; }
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
	std::unique_ptr<PrototypeAST> Proto;
	std::unique_ptr<ExprAST> Body;
	std::unique_ptr<ExprAST> ReturnValue;
	std::string returnType;

public:
	FunctionAST(std::unique_ptr<PrototypeAST> Proto,
		std::unique_ptr<ExprAST> Body,
		std::unique_ptr<ExprAST> ReturnValue,
		std::string ReturnType)
		: Proto(std::move(Proto)), Body(std::move(Body)), returnType(ReturnType), ReturnValue(std::move(ReturnValue)) {}

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

class VoidAst : public ExprAST {
public:
	Value* codegen();
};

// code like a = 5;
//class RedefinitionExprAst : public ExprAST
//{
//
//};
