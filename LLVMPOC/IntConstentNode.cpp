#include "IntConstentNode.h"

llvm::Value* IntConstentNode::codegen()
{
    return ConstantFP::get(*TheContext, APFloat(m_value));
}
