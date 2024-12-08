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
            // Create a ResourceTracker to track JIT'd memory allocated to our
            // anonymous expression -- that way we can free it after executing.
            auto RT = Helper::TheJIT->getMainJITDylib().createResourceTracker();

            auto TSM = llvm::orc::ThreadSafeModule(std::move(Helper::TheModule), std::move(Helper::TheContext));
            Helper::ExitOnErr(Helper::TheJIT->addModule(std::move(TSM), RT));
            Helper::InitializeModuleAndManagers();

            // Search the JIT for the __anon_expr symbol.
            auto ExprSymbol = Helper::ExitOnErr(Helper::TheJIT->lookup("__anon_expr"));

            // Get the symbol's address and cast it to the right type (takes no
            // arguments, returns a double) so we can call it as a native function.
            double (*FP)() = ExprSymbol.getAddress().toPtr<double (*)()>();
            fprintf(stderr, "Evaluated to %f\n", FP());

            // Delete the anonymous expression module from the JIT.
            Helper::ExitOnErr(RT->remove());
        }
    }
    else {
        // Skip token for error recovery.
        parser.consume();
    }
}

void TopLevelParser::mainLoop()
{
    while (true)
    {
        fprintf(stderr, " ready > ");
        switch (parser.currentToken().getType())
        {
         // TODO - add here function decleration
        default:
            HandleTopLevelExpression();
        }
    }
}
