// #include <iostream>
// #include <exception>
// #include <string>

// class SyntaxError : public std::exception {
// private:
//     std::string message;
//     int chrIndex;
//     std::string fullMessage;  // Store the full error message

// public:
//     // Constructor to initialize the exception message and character index
//     SyntaxError(const std::string& msg, int chrIndex)
//         : message(msg), chrIndex(chrIndex)
//     {
//         // Construct the full message here, so it persists
//         fullMessage = "Syntax error!\n" + message + " in chr: " + std::to_string(chrIndex);
//     }

//     // Override the what() method from std::exception
//     const char* what() const noexcept override {
//         return fullMessage.c_str();
//     }
// };
