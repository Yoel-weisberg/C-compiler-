#include "INode.h"

class IntConstantNode : public INode
{
public:
	IntConstantNode(double value) : INode(), m_value(value) {}
	llvm::Value* codegen() override;

private:
	double m_value;
};