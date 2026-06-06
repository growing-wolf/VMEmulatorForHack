// CodeWriter.hpp
#ifndef CODEWRITER_HPP
#define CODEWRITER_HPP

#include <string>
#include <fstream>
#include "Parser.hpp"

class CodeWriter{
private:
    int labelCounter;
    std::string filename;
    std::ofstream output;

public:
    CodeWriter(const std::string& pathname);
    void setFileName(std::string &pathname);
    void WriteInit();
    void WriteArithmetic(std::string command);
    void WritePushPOP(CommandType type, std::string segment, int index);
    void WriteLabel(std::string label);
    void WriteGoto(std::string label);
    void WriteIf(std::string label);
    void WriteCall(std::string functionName, int numArgs);
    void WriteReturn();
    void WriteFunction(std::string functionName, int numLocals );
    void Close();
};

#endif