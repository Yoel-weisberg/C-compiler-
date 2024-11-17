class Token {
private:
	std::string _literal;
	Tokens_type _type;
public:

	Token(const std::string& _literal, const Tokens_type& _type)
		: _literal(_literal), _type(_type)
	{
	}
	std::string getLiteral() { return _literal; }
	Tokens_type getType() { return _type; }
};