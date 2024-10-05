//#include "Asm.h"
//#include <iostream>
//
//asmjit::CodeHolder ConvortASTtoASM::code;
//asmjit::JitRuntime ConvortASTtoASM::runtime;
//asmjit::x86::Assembler* ConvortASTtoASM::assembler = nullptr;
//
//typedef int (*Func)(void);
//
//ConvortASTtoASM::ConvortASTtoASM(ASTNode* root) {
//    code.init(runtime.environment());
//    assembler = new asmjit::x86::Assembler(&code);
//
//    OperationCaller(root);
//
//    // Pop the final result from the stack into EAX for returning
//    assembler->pop(asmjit::x86::eax);
//
//    assembler->ret(); // Return from the function
//
//    Func fn;
//    asmjit::Error err = ConvortASTtoASM::runtime.add(&fn, &ConvortASTtoASM::code);
//
//    if (err) {
//        printf("AsmJit failed: %s\n", asmjit::DebugUtils::errorAsString(err));
//        return;
//    }
//
//    int result = fn();
//    printf("%d\n", result);
//
//    ConvortASTtoASM::runtime.release(fn);
//}
//
//void ConvortASTtoASM::OperationCaller(ASTNode* root) {
//    if (!root) return;
//
//    // Handle left and right children recursively
//    if (root->left) OperationCaller(root->left);
//    if (root->right) OperationCaller(root->right);
//
//    if (root->token == INT) {
//        // Push the integer value onto the stack
//        assembler->mov(asmjit::x86::eax, root->toInt());
//        assembler->push(asmjit::x86::eax);
//    }
//    else if (!root->left && root->right) {
//        // Handle unary operations (only one child)
//        switch (root->token) {
//        case SUBTRACTION: // Assuming SUBTRACTION token is used for unary negation as well
//            assembler->pop(asmjit::x86::eax); // Pop the value into EAX
//            assembler->neg(asmjit::x86::eax); // Negate the value in EAX
//            assembler->push(asmjit::x86::eax); // Push the result back onto the stack
//            break;
//        default:
//            break;
//        }
//    }
//    else {
//        // Perform the operation with the top two values on the stack
//        assembler->pop(asmjit::x86::ebx); // Pop the second operand into EBX
//        assembler->pop(asmjit::x86::eax); // Pop the first operand into EAX
//
//        switch (root->token) {
//        case ADDITION:
//            assembler->add(asmjit::x86::eax, asmjit::x86::ebx);
//            break;
//        case SUBTRACTION:
//            assembler->sub(asmjit::x86::eax, asmjit::x86::ebx);
//            break;
//        case MULTIPLICATION:
//            assembler->imul(asmjit::x86::eax, asmjit::x86::ebx);
//            break;
//        case DIVISION:
//            assembler->cdq();  // Sign-extend EAX into EDX:EAX for division
//            assembler->idiv(asmjit::x86::ebx);
//            break;
//        default:
//            break;
//        }
//
//        // Push the result back onto the stack
//        assembler->push(asmjit::x86::eax);
//    }
//}
