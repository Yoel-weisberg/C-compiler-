#include "INode.h"

class BinaryNode : public INode {
    char Op;
    std::unique_ptr<INode> LHS, RHS;

public:
    BinaryNode(char Op, std::unique_ptr<INode> LHS,
        std::unique_ptr<INode> RHS)
        : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

    llvm::Value* codegen() override;
};