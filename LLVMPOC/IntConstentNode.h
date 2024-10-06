#include "INode.h"

class IntConstentNode : public INode
{
public:
	IntConstentNode(double value) : INode(), m_value(value) {}
	llvm::Value* codegen() override;

private:
	double m_value;
};