#include "Parser.hpp"
#include <iostream>
#include <sstream>
#include <string>

Parser::Parser(const std::string filename) : inFile(filename) { }

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
    while (std::getline(inFile, line)) {
      if (!line.empty() && line.back() == '\r') {
            line.pop_back(); 
        }
        if (line.empty() || line.find("//") != std::string::npos) {
          continue;
        }
       
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
    std::string token;
    while (ss >> token) {
      tokens.push_back(token);
    }
    int size=tokens.size();
    cmdtype=tokens[0];
    arg1 = size>=2 ? tokens[1]:"";
    arg2 = size >= 3 ? tokens[2] : "";
    // std::cout << tokens.size()<<" " << cmdtype << " " << arg1 << " "
    //           <<arg2 << std::endl;
    // // std::cout<<cmdtype<<" "<<arg1<<" "<<arg2<<std::endl;
         
}

std::string Parser::ARG1() {
    return arg1;
}

int Parser::ARG2() { return std::stoi(arg2); }

std::string Parser::cmdType() {
     return cmdtype;
};