#include "IntConstantNode.h"

llvm::Value* IntConstantNode::codegen()
{
    return ConstantFP::get(*TheContext, APFloat(m_value));
}
