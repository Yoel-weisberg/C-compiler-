#include "../Header/sourceFileHandler.h"
#include "../Header/SyntexError.h"
#include <iostream>
#include <limits.h>


SourceFileHandler::SourceFileHandler(char* argv[], int argc)
{

	if (argc == 2) {
		_fileName = argv[1];
	}
	else
	{
		_fileName = "../testFiles/testFile.c";
	}
	_sourceFile.open(_fileName);
	// Check if file opens
	if (!_sourceFile.is_open())
	{
		throw SyntaxError("File cant open " + _fileName, 0);
	}

	std::ostringstream buffer;
	buffer << _sourceFile.rdbuf();
	_srcFileContent = buffer.str();  // Store the entire file content in _srcFileContent

	// Close the file after reading
	_sourceFile.close();
}

std::string SourceFileHandler::getSrcFileContent()
{
	return _srcFileContent;
}


