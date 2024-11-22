#include <iostream>
#include <vector>


class Symbol {
public:
	Symbol(std::string name, std::string type, std::string value) : _name(name), _type(type), _value(value) {}

private:
	std::string _name;
	std::string _type;
	std::string _value;
};

class SymbolTable {
public:
	SymbolTable() = default;
	void addSymbol(Symbol* symbol);

private:
	std::vector<Symbol*> _table;
};
