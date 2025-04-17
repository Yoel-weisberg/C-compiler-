#ifndef STABLE_H
#define STABLE_H

#include <iostream>
#include <algorithm>
#include <tuple>
#include <vector>

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "Helper.h"

class Helper;

// Class manages all types of symbol tables (varibales, structs and functions)
class STable
{
public:
	STable();

	// Print All Tables
	void printAllTables();

	// Add Entry
	void addVarEntry(std::string var_name, std::string var_type, const std::string& pTT = EMPTY_STR, const int size = 1);
	void addStructDefinitionEntry(std::string struct_name, llvm::StructType* type);
	void addStructVarEntry(std::string var_name, std::string type, llvm::AllocaInst* address);
	void addFuncEntry(std::string func_name, std::string ret_type, llvm::Function* address); 

	// Remove Entry

	// Add Scope
	void addScope(const std::string& new_scope);
	std::string getScope();

	// Searching methods
	llvm::StructType* getStructType(std::string type);
	llvm::AllocaInst* getVarAddress(std::string name);
	std::string getRetType();

private:

	// Variables Table 
	//                        name          scope         type           address
	std::vector<std::tuple<std::string, std::string, std::string, llvm::AllocaInst*>> _varTable;

	// Struct Table  (for declerations only! struct type variables are stored in 'varTable')
	//                      name          scope          type
	std::vector<std::tuple<std::string, std::string, llvm::StructType*>> _structTable;

	// Funciton Table
	//                        name          scope    return type     address
	std::vector<std::tuple<std::string, std::string, std::string, llvm::Function*>> _funcTable;


	// Scope managing
	std::vector<std::string> _scopes;

	// --- Get Addresses ---
	// Variables 
	llvm::AllocaInst* getVarAddress(std::string var_name, std::string var_type, const int size, const std::string& pTT);

	// Functions 
	//llvm::FunctionType * get

};

#endif