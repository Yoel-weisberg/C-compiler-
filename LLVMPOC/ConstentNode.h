#include "INode.h"

class ConstentNode : public INode
{
public:
	ConstentNode(double value) : INode(), m_value(value) {}
	llvm::Value* codegen() override;

private:
	double m_value;
};