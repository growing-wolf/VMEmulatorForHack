#include "Parser.hpp"
#include <iostream>
#include <sstream>
#include <string>

Parser::Parser(const std::string &filename) : inFile(filename) { }
Parser::Parser(const std::filesystem::path &path): inFile(path){}
bool Parser::advance(){
    if(!hasMoreCommands()){
        return false;
    }
    //获取命令，解析token
    Parser::divide();
    return true;
}

bool Parser::hasMoreCommands(){
    std::string line;
    while(std::getline(inFile,line)){
        if(line.empty()||line.find("//")!=std::string::npos){continue;}
        command = line;
        return true;
    }
    return false;
}

CommandType Parser::commandType() {
    //查找命令表，对应相应命令
    auto it = commandMap.find(cmdtype);
    if (it != commandMap.end()) {
        return it->second;
    }
    return C_FAILED;
}

void Parser::divide() {
    //清理，划分语义块，正确对应arg
    tokens.clear();
    std::stringstream ss(command);
    std::string args;
    while (ss >> args) {tokens.push_back(args);}
    CommandType type = commandType();
    if(type==C_ARITHMETIC){
        arg1 = command;
    }
    if(type==C_POP||type==C_PUSH||type==C_FUNCTION||type==C_CALL){
      if (tokens.size() == 3) {
            cmdtype =tokens[0];
            arg1 = tokens[1];
            arg2 = tokens[2];
        }else{
            std::cout << tokens.size() << std::endl;
        }
    }
}

std::string Parser::ARG1(){
    return arg1;
}

int Parser::ARG2(){
    return std::stoi(arg2);
}