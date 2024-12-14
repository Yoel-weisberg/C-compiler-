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
#include "Helper.h"
#include "CompilationErrors.h"


using namespace llvm;

#define INTEGER_SIZE 32
#define CHAR_SIZE 8
//
//template<typename T> T(T);
//std::vector<T> arrVec

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
	std::string _valAsStr;
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
	arrExprAST(const std::string& type, std::string& size, const std::string& val, const std::string& name) : _val(val), _name(name)
	{	
		assignLLVMType(type);
		_size = Helper::hexToDec(size);

		// Parse 'init' into `_data`
		std::vector<uint64_t> parsedData;
		std::stringstream ss(val);
		std::string token;
		while (std::getline(ss, token, ',')) {
			parsedData.push_back(std::stoull(token));
		}
		_data = llvm::ArrayRef<uint64_t>(parsedData);
	}
	virtual Value* codegen() override;
	void assignLLVMType(const std::string& type);
	void initArrayRef(const std::string& val, const std::string& type);
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
