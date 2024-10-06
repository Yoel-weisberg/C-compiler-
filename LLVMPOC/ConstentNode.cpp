#include "ConstentNode.h"

llvm::Value* ConstentNode::codegen()
{
    return ConstantFP::get(*TheContext, APFloat(m_value));
}
