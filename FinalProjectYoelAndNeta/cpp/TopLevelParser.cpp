#include "TopLevelParser.h"

TopLevelParser::TopLevelParser(const std::vector<Token>& tokens) :
	parser(Parser(tokens))
{
}

void TopLevelParser::HandleTopLevelExpression()
{
    // First check what kind of expression we're dealing with
    Tokens_type currentType = parser.currentToken().getType();

    // Handle specific expression types
    if (currentType == RETURN_STATEMENT) {
        std::cerr << "Error: Return statement outside of function context\n";
        parser.consume(); // Skip for error recovery
        return;
    }

    // Try to parse as a general expression
    if (auto FnAST = parser.parseTopLevelExpr()) {
        if (FnAST->codegen()) {
            Helper::TheModule->print(llvm::errs(), nullptr); // Print IR here

            // JIT execution code...
            auto RT = Helper::TheJIT->getMainJITDylib().createResourceTracker();
            auto TSM = llvm::orc::ThreadSafeModule(std::move(Helper::TheModule), std::move(Helper::TheContext));
            Helper::ExitOnErr(Helper::TheJIT->addModule(std::move(TSM), RT));
            Helper::InitializeModuleAndManagers();

            // Delete the anonymous expression module from the JIT.
            Helper::ExitOnErr(RT->remove());
        }
    }
    else {
        // Skip token for error recovery
        std::cerr << "Failed to parse expression at token: "
            << parser.currentToken().getLiteral() << std::endl;
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

        // Check for common tokens that should be handled
        Tokens_type currentType = parser.currentToken().getType();

        switch (currentType)
        {
        case TYPE_DECLERATION:
            if (isFunctionDecleration())
            {
                HandleDefinition();
            }
            else
            {
                // Handle top-level variable declaration
                HandleTopLevelExpression();
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

        case RETURN_STATEMENT:
            // Return should be handled as part of function definition
            // If we find it at top level, it's part of function body
            parser.consume();
            break;

        default:
            // Uncomment this to handle expressions
            HandleTopLevelExpression();
            // Or use this if you want to just skip problematic tokens
            // parser.consume();
        }
    }
}