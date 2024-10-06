#include "INode.h"

class LLVMHelper {
public:
	static void generateIR(INode* astRoot);
private:
	llvm::Function* createFunction();
};