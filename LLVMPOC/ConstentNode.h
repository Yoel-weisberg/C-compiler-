#include "INode.h"

class ConstentNode : public INode
{
public:
	ConstentNode(double value);
	llvm::Value* codegen() override;

private:
	double value;
};