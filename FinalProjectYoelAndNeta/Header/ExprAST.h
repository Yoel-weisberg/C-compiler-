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
 
// Used for both function arguments and struct members
struct Field
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

class UnaryOpExprAST : public ExprAST
{
private:
	Tokens_type _op; // Operators used in the code
	std::unique_ptr<ExprAST> _LHS; // Only handling postfix, so only the LHS is needed
public:
	UnaryOpExprAST(Tokens_type operators, std::unique_ptr<ExprAST> LHS) : _op(operators), _LHS(std::move(LHS)) {}
	llvm::Value* codegen() override;
};

// VariableExprAST - Expression class for referencing a variable, like "a".
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


class IfExprAST : public ExprAST
{
private:
	std::unique_ptr<ExprAST> Cond, Then, Else;
public:
	IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then,
		std::unique_ptr<ExprAST> Else)
		: Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

	Value* codegen() override;
};

class WhileLoopAST : public ExprAST
{
private:
	std::unique_ptr<ExprAST> _condition;
	std::vector<std::unique_ptr<ExprAST>> _body;
public:
	WhileLoopAST(std::unique_ptr<ExprAST> condition, std::vector<std::unique_ptr<ExprAST>> body) : _condition(std::move(condition)), _body(std::move(body)) {}
	llvm::Value* codegen() override;
	//llvm::PHINode* processBody();
};


class DoWhileLoopAST : public ExprAST
{
private:
	std::unique_ptr<ExprAST> _condition;
	std::vector<std::unique_ptr<ExprAST>> _body;
public:
	DoWhileLoopAST(std::unique_ptr<ExprAST> condition, std::vector<std::unique_ptr<ExprAST>> body) : _condition(std::move(condition)), _body(std::move(body)) {}
	llvm::Value* codegen() override;
};


class ForLoopAST : public ExprAST
{
private:
	std::vector<std::unique_ptr<ExprAST>> _body;
	// ---- Condition Parts ----
	std::unique_ptr<ExprAST> _condInit; // Initilization
	std::unique_ptr<ExprAST> _condC; // Condition 
	std::unique_ptr<ExprAST> _condStep;	// Increasement / Decreasement or any other change... 
public:
	ForLoopAST(std::vector<std::unique_ptr<ExprAST>> body, std::unique_ptr<ExprAST> condInit, std::unique_ptr<ExprAST> condC, std::unique_ptr<ExprAST> condInc) : _body(std::move(body)), _condInit(std::move(condInit)), _condC(std::move(condC)), _condStep(std::move(condInc)) {}
	llvm::Value* codegen() override;
};

class PrototypeAST
{
	std::string Name;
	std::vector<Field> Args;
	std::string returnType;
public:
	PrototypeAST(const std::string& Name, std::vector<Field> Args, std::string returnType)
		: Name(Name), Args(std::move(Args)), returnType(returnType) {}

	llvm::Function* codegen();
	const std::string& getName() const { return Name; }
	const std::string& getReturnType() const { return returnType; }
	const std::string  getParamName(unsigned idx) const;
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
	std::unique_ptr<PrototypeAST> Proto;
	std::vector<std::unique_ptr<ExprAST>> Body;
	std::unique_ptr<ExprAST> ReturnValue;
	std::string returnType;

public:
	FunctionAST(std::unique_ptr<PrototypeAST> Proto,
		std::vector<std::unique_ptr<ExprAST>> Body,
		std::unique_ptr<ExprAST> ReturnValue,
		std::string ReturnType)
		: Proto(std::move(Proto)), Body(std::move(Body)),  ReturnValue(std::move(ReturnValue)) ,returnType(ReturnType){}

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


class StructDefinitionExprAST : public ExprAST
{
	std::string _name; // Struct name
	std::vector<Field> _members; // Struct members
	
public:
	StructDefinitionExprAST(const std::string& name, std::vector<Field> members) : _name(name), _members(members){}
	llvm::Value* codegen() override;
};


class StructDeclerationExprAST : public ExprAST
{
	std::string _name;
	std::string _type;
public:
	StructDeclerationExprAST(const std::string& type, const std::string& name) : _type(type), _name(name){}
	llvm::Value* codegen() override;
};

class StringExprAST : public ExprAST
{
private:
	std::string _str;

public:
	StringExprAST(const std::string& str) : _str(str) {}
	Value* codegen() override;
};

class  EmptyExprAST : public ExprAST
{
public:
	EmptyExprAST() {}

	llvm::Value* codegen() override {
		// An empty statement generates no code
		return nullptr;
	}
};

class BlockExprAST : public ExprAST {
private:
	std::vector<std::unique_ptr<ExprAST>> Statements;

public:
	BlockExprAST(std::vector<std::unique_ptr<ExprAST>> statements)
		: Statements(std::move(statements)) {}

	llvm::Value* codegen() override;
	const std::vector<std::unique_ptr<ExprAST>>& getStatements() const { return Statements;  }
};
// code like a = 5;
//class RedefinitionExprAst : public ExprAST
//{
//
//};
