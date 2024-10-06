#include "INode.h"

std::unique_ptr<LLVMContext> INode::TheContext = nullptr;
std::unique_ptr<IRBuilder<>> INode::Builder = nullptr;
std::unique_ptr<Module> INode::TheModule = nullptr;
std::map<std::string, Value*> INode::NamedValues;

void INode::initializeLLVM() {
    if (!TheContext) {
        TheContext = std::make_unique<LLVMContext>();
        Builder = std::make_unique<IRBuilder<>>(*TheContext);
        TheModule = std::make_unique<Module>("my cool jit", *TheContext);
    }
}

Value* INode::LogErrorV(const char* Str) {
    LogError(Str);
    return nullptr;
}

void INode::LogError(const char* Str) {
    fprintf(stderr, "Error: %s\n", Str);
    abort(); // Stop execution
}
