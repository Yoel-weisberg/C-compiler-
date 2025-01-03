#include "TopLevelParser.h"

TopLevelParser::TopLevelParser(const std::vector<Token>& tokens) :
	parser(Parser(tokens))
{
}


void TopLevelParser::HandleTopLevelExpression()
{
    // Evaluate a top-level expression into an anonymous function.
    if (auto FnAST = parser.ParseTopLevelExpr()) {
        if (FnAST->codegen()) {
            Helper::TheModule->print(llvm::errs(), nullptr); // Print IR here
            // Create a ResourceTracker to track JIT'd memory allocated to our
            // anonymous expression -- that way we can free it after executing.
            auto RT = Helper::TheJIT->getMainJITDylib().createResourceTracker();

            // Add the module to the JIT
            auto TSM = llvm::orc::ThreadSafeModule(std::move(Helper::TheModule), std::move(Helper::TheContext));
            Helper::ExitOnErr(Helper::TheJIT->addModule(std::move(TSM), RT));


            // Search the JIT for the __anon_expr symbol.
            auto ExprSymbol = Helper::ExitOnErr(Helper::TheJIT->lookup("__anon_expr"));


            auto SymbolAddr = ExprSymbol.getAddress();
            llvm::outs() << "__anon_expr Address: " << SymbolAddr << "\n";
            // Get the symbol's address and cast it to the right type (takes no
            // arguments, returns a double) so we can call it as a native function.
            //double (*FP)() = ExprSymbol.getAddress().toPtr<double (*)()>();
            //fprintf(stderr, "Evaluated to %f\n", FP());


            // Delete the anonymous expression module from the JIT.
            Helper::ExitOnErr(RT->remove());

            Helper::InitializeModuleAndManagers();

        }
    }
    else {
        // Skip token for error recovery.
        parser.consume();
    }
}

bool TopLevelParser::isFunctionDecleration()
{
    parser.consume(2);
    if (parser.currentToken().getType() == LPAREN)
    {
        parser.consume(-2);
        return true;
    }
    parser.consume(-2);
    return false;
}

void TopLevelParser::HandeleDefinition()
{
    if (auto FnAST = parser.ParseDefinition()) {
        if (auto* FnIR = FnAST->codegen()) {
            fprintf(stderr, "Read function definition:");
            FnIR->print(errs());
            fprintf(stderr, "\n");
            Helper::ExitOnErr(Helper::TheJIT->addModule(
                llvm::orc::ThreadSafeModule(std::move(Helper::TheModule), std::move(Helper::TheContext))));
            Helper::InitializeModuleAndManagers();
        }
    }
    else {
        // Skip token for error recovery.
        parser.consume();
    }
}

void TopLevelParser::mainLoop()
{
    while (!parser.isFinished())
    {
        fprintf(stderr, " ready > ");
        switch (parser.currentToken().getType())
        {
         // TODO - add here function decleration
        case TYPE_DECLERATION:
            if (isFunctionDecleration())
            {
                HandeleDefinition();
            }
            break;
        case R_CURLY_BRACK:
            parser.consume();
            break;
        default:
            HandleTopLevelExpression();
        }
    }
}
