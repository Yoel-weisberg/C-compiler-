#pragma once

#include <iostream>
#include <exception>
#include <string>

class SyntaxError : public std::exception {
private:
    std::string _message;
    int _chrIndex;
    std::string _fullMessage;  // Store the full error message

public:
    // Constructor to initialize the exception message and character index
    SyntaxError(const std::string& msg, int chrIndex = 0)
        : _message(msg), _chrIndex(chrIndex)
    {
        // Construct the full message here, so it persists
        _fullMessage = "[SYNTAX ERROR]\t" + _message + " in chr: " + std::to_string(chrIndex);
    }

    // Override the what() method from std::exception
    const char* what() const noexcept override 
    {
        return _fullMessage.c_str();
    }
};



class ParserError : public std::exception
{
private:
    std::string _message;
    int _chrIndex;
    std::string _fullMessage;
public:
    // Constructor to initialize the exception message and character index
    ParserError(const std::string& msg, int chrIndex = 0)
        : _message(msg), _chrIndex(chrIndex)
    {
        // Construct the full message here, so it persists
        _fullMessage = "[PARSER ERROR]\t" + _message + " in chr: " + std::to_string(chrIndex);
    }

    // Override the what() method from std::exception
    const char* what() const noexcept override
    {
        return _fullMessage.c_str();
    }
};