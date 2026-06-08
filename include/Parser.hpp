// Parser.hpp
#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <filesystem>

enum CommandType
{
    C_ARITHMETIC = 0,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
    C_FAILED
};

class Parser{
private:
    std::string command;
    std::string cmdtype;
    std::ifstream inFile;
    std::string arg1;
    std::string arg2;
    std::vector<std::string> tokens;
    const std::unordered_map<std::string, CommandType> commandMap = {
        {"push", C_PUSH},
        {"pop", C_POP},
        {"add", C_ARITHMETIC},
        {"sub", C_ARITHMETIC},
        {"neg", C_ARITHMETIC},
        {"and", C_ARITHMETIC},
        {"or", C_ARITHMETIC},
        {"not", C_ARITHMETIC},
        {"eq", C_ARITHMETIC},
        {"gt", C_ARITHMETIC},
        {"lt", C_ARITHMETIC},
        {"goto", C_GOTO},
        {"label", C_LABEL},
        {"if-goto", C_IF},
        {"function", C_FUNCTION},
        {"call", C_CALL},
        {"return", C_RETURN}
    };
    

public:
  Parser(const std::string filename);
    bool advance();
    bool hasMoreCommands();
    CommandType commandType();
    void divide();
    std::string cmdType();
    std::string ARG1();
    int ARG2();
};

#endif