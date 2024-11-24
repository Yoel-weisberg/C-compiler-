#pragma once
#include <iostream>

/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
	virtual ~ExprAST() = default;
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
	char Op;
	std::unique_ptr<ExprAST> LHS, RHS;

public:
	BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
		std::unique_ptr<ExprAST> RHS)
		: Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};



/// NumberExprAST - Expression class for numeric literals like "1.0".
class	FloatNumberExprAST : public ExprAST {
	double Val;

public:
	FloatNumberExprAST(double Val) : Val(Val) {}
};


/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
	std::string Name;

public:
	VariableExprAST(const std::string& Name) : Name(Name) {}
};


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
};
