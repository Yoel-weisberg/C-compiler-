#include "INode.h"

class LLVMHelper {
public:
	static void generateIR(INode* astRoot);
	static 	llvm::Function* createFunction();
};