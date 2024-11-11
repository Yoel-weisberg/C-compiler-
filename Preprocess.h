#include <iostream>
#include <fstream>
#include <map>

class Preprocess{
public:
    Preprocess(const std::string& fileName) : _fileName(fileName) {std::cout << "Created a 'Preprocess' instance\n" << std::endl;} // print only for testing

private:
    std::string _fileName;
    void removeComments ();
    void manageIncludes();
    void handleMacroVariebles();
    // void handleFunctinMacro(); - sprint2
    std::map <std::string, std::string> _macroTable;
};