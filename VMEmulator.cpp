#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
//枚举vm命令类型
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
//语义分析器
class Parser{
    private:
    
        int Arg2;
        std::string command;
        std::ifstream inFile;
        std::string arg1;
        std::string arg2;
        std::vector<std::string> tokens;

    public:
        Parser(std::string &filename) : inFile(filename) { };
        bool advance(){
                  
            if(!hasMoreCommands()){
                return false;
                
            }
            
            return true;
        }
        bool hasMoreCommands(){
            std::string line;
            while(std::getline(inFile,line)){
                //Skip empty lines and comments
               
                if(line.empty()||line.find("//")!=std::string::npos){continue;}
        
                command = line;

                return true;
            }
            return false;
        }
        CommandType commandType(){
            if(command.find("push")!=std::string::npos){
                return C_PUSH;
            }
            if(command.find("pop")!=std::string::npos){
                return C_POP;
            }
            //遍历识别算术和逻辑命令
            const char* commands[] = {"add", "sub", "neg", "and", "or", "not", "eq", "gt", "lt"};
            for (const char* cmd : commands) {
                if (command.find(cmd) != std::string::npos) {
                    return C_ARITHMETIC;
                }
            }
            return C_FAILED;
        }
        //解析命令，划分语法快
        void divide(){
            tokens.clear();
            std::stringstream ss(command);
            std::string args;
            while (ss >> args) {tokens.push_back(args);}
            CommandType type = commandType();
            if(type==C_ARITHMETIC){
                arg1 = command;
            }
            if(type==C_POP||type==C_PUSH||type==C_FUNCTION||type==C_CALL){
                
                if(tokens.size()==3){
                    arg1 = tokens[1];
                    arg2 = tokens[2];

                }else{
                    std::cout << tokens.size() << std::endl;
                }
            }
        }
        std::string ARG1(){
            return arg1;
        }
        int ARG2(){
           
            // if (arg2.empty()) {
            //     return 0;  // 或抛出更友好的错误
            // }
            return std::stoi(arg2);
        }   

};
                    
class CodeWriter{
    private:
        int labelCounter = 0;
        std::string filename;
        std::ofstream output;
    public:
      
        CodeWriter(const std::string& pathname) : output(pathname) {
            filename = std::filesystem::path(pathname).stem().string();
         
        }
        void setFileName(std::string& pathname){
            if (output.is_open()){
                output.close();
            }
            output.open(pathname);
            filename = std::filesystem::path(pathname).stem().string();
            
        }
        void WriteArithmetic(std::string command){
            //算术逻辑命令生成汇编代码
            if(command=="add"){
               output <<"@SP\nAM=M-1\nD=M\n@SP\nAM=M-1\nM=M+D\n@SP\nM=M+1\n";
            }
            if(command == "sub") {
                output <<"@SP\nAM=M-1\nD=M\n@SP\nAM=M-1\nM=M-D\n@SP\nM=M+1\n";
            }
            if(command == "neg") {
                output << "@SP\nAM=M-1\nM=-M\n@SP\nM=M+1\n";
            }
            if(command == "and") {
                output <<"@SP\nAM=M-1\nD=M\n@SP\nAM=M-1\nM=M&D\n@SP\nM=M+1\n";
            }
            if(command == "or") {
                output <<"@SP\nAM=M-1\nD=M\n@SP\nAM=M-1\nM=M|D\n@SP\nM=M+1\n";
            }
            if(command == "not") {
                output <<"@SP\nAM=M-1\nM=!M\n@SP\nM=M+1\n";
            }
            if(command == "eq") {
                std::string label = std::to_string(labelCounter++);
                output <<"@SP\nAM=M-1\nD=M\n@SP\nAM=M-1\nD=M-D\n@EQ_TRUE" + label  + "\nD;JEQ\nD=0\n@EQ_END" +label  + "\n0;JMP\n(EQ_TRUE" + label  + ")\nD=-1\n(EQ_END" + label  + ")\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
                
            }
            if(command == "gt") {
                std::string label = std::to_string(labelCounter++);
                output <<"@SP\nAM=M-1\nD=M\n@SP\nAM=M-1\nD=M-D\n@GT_TRUE" + label  + "\nD;JGT\nD=0\n@GT_END" + label  + "\n0;JMP\n(GT_TRUE" + label  + ")\nD=-1\n(GT_END" + label  + ")\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
                
            }
            if(command == "lt") {
            std::string label = std::to_string(labelCounter++);
            output <<"@SP\nAM=M-1\nD=M\n@SP\nAM=M-1\nD=M-D\n@LT_TRUE" + label + "\nD;JLT\nD=0\n@LT_END" + label + "\n0;JMP\n(LT_TRUE" + label + ")\nD=-1\n(LT_END" + label + ")\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
            }
        }
        void WritePushPOP(CommandType type,std::string segment,int index){
            
            std::string seg;
            //为内存段赋予相应基地址或预定义符号
            if (segment == "local") seg = "LCL";
            else if (segment == "argument") seg = "ARG";
            else if (segment == "this") seg = "THIS";
            else if (segment == "that") seg = "THAT";
            else if (segment == "pointer") seg = "3";
            else if (segment == "temp") seg = "5";
            else if (segment == "constant") seg = "";
            else if (segment == "static") seg = "16";
            if (type == C_PUSH) {
                if (segment == "constant") {
                    output << "@" << index << "\n"
                        << "D=A\n"
                        << "@SP\n"
                        << "A=M\n"
                        << "M=D\n"
                        << "@SP\n"
                        << "M=M+1\n";
                }
                else if (segment == "static") {
                    output << "@"<<filename<<"." << index << "\n"  
                        << "D=M\n"
                        << "@SP\n"
                        << "A=M\n"
                        << "M=D\n"
                        << "@SP\n"
                        << "M=M+1\n";
                }
                else if (segment == "temp" || segment == "pointer") {
                    output << "@R" << seg << "\n"
                        << "D=A\n"
                        << "@" << index << "\n"
                        << "A=D+A\n"
                        << "D=M\n"
                        << "@SP\n"
                        << "A=M\n"
                        << "M=D\n"
                        << "@SP\n"
                        << "M=M+1\n";
                }
                else {
                    output << "@" << seg << "\n"
                        << "D=M\n"
                        << "@" << index << "\n"
                        << "A=D+A\n"
                        << "D=M\n"
                        << "@SP\n"
                        << "A=M\n"
                        << "M=D\n"
                        << "@SP\n"
                        << "M=M+1\n";
                }
            }
            else if (type==C_POP) {
                if (segment == "static") {
                    output << "@SP\n"
                        << "AM=M-1\n"
                        << "D=M\n"
                        << "@"<<filename<<"."<< index << "\n"
                        << "M=D\n";
                }
                else if (segment == "temp" || segment == "pointer") {
                    output << "@R" << seg << "\n"
                        << "D=A\n"
                        << "@" << index << "\n"
                        << "D=D+A\n"
                        << "@R13\n"
                        << "M=D\n"
                        << "@SP\n"
                        << "AM=M-1\n"
                        << "D=M\n"
                        << "@R13\n"
                        << "A=M\n"
                        << "M=D\n";
                }
                else {
                    output << "@" << seg << "\n"
                        << "D=M\n"
                        << "@" << index << "\n"
                        << "D=D+A\n"
                        << "@R13\n"
                        << "M=D\n"
                        << "@SP\n"
                        << "AM=M-1\n"
                        << "D=M\n"
                        << "@R13\n"
                        << "A=M\n"
                        << "M=D\n";
                }
            }
        }
        void Close(){
            output.close();
        }

};


int main(int argc,char* argv[]){
    if (argc < 1) {
        std::cerr << "Usage: " << argv[0] << " <input_file> " << std::endl;
        return 1;
    }
    std::string inFileName = argv[1];
    size_t pos = inFileName.find(".vm");
    std::string outFileName = inFileName.substr(0, pos) + ".asm";

    Parser parser(inFileName);
    CodeWriter cw(outFileName);
    //开始翻译
 
    while(parser.advance()){
        parser.divide();
        if(parser.commandType()==C_ARITHMETIC){
            
            cw.WriteArithmetic(parser.ARG1());
        }
        if(parser.commandType()==C_PUSH || parser.commandType()==C_POP){
            
            cw.WritePushPOP(parser.commandType(), parser.ARG1(), parser.ARG2());
        }

    }

    return 0;
}