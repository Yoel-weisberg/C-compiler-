#include "TopLevelParser.h"

TopLevelParser::TopLevelParser(const std::vector<Token>& tokens) :
	parser(Parser(tokens))
{
}


void TopLevelParser::HandleTopLevelExpression()
{
    // Evaluate a top-level expression into an anonymous function.
    if (auto FnAST = parser.parseTopLevelExpr()) {
        if (FnAST->codegen()) {
            Helper::TheModule->print(llvm::errs(), nullptr); // Print IR here
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
            //double (*FP)() = ExprSymbol.getAddress().toPtr<double (*)()>();
            //fprintf(stderr, "Evaluated to %f\n", FP());


            // Delete the anonymous expression module from the JIT.
            Helper::ExitOnErr(RT->remove());
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

void TopLevelParser::HandleDefinition()
{
    if (auto FnAST = parser.parseFunctionDefinition()) {
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

bool TopLevelParser::isStructDefinition()
{
    parser.consume(2); // Move past 'struct' and its' name
    if (parser.currentToken().getType() == L_CURLY_BRACK)
    {
        parser.consume(-2);
        return true;
    }
    parser.consume(-2);
    return false;
}

void TopLevelParser::HandleStructDefinition()
{
    if (auto structAST = parser.parseStructDefinition())
    {
        if (auto* structIR = structAST->codegen())
        {
            fprintf(stderr, "Read function definition:");
            fprintf(stderr, "\n");
            Helper::ExitOnErr(Helper::TheJIT->addModule(
                llvm::orc::ThreadSafeModule(std::move(Helper::TheModule), std::move(Helper::TheContext))));
        }
    }
    else
    {
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
                HandleDefinition();
            }
            break;
        case R_CURLY_BRACK:
            parser.consume();
            break;
        case STRUCT:
            if (isStructDefinition())
            {
                HandleStructDefinition();
            }
            break;
        default:
            std::cerr << "WTF is wrong now ?!" << std::endl;
            //HandleTopLevelExpression();
        }
    }
}
