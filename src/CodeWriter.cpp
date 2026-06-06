#include "CodeWriter.hpp"
#include <filesystem>

CodeWriter::CodeWriter(const std::string& pathname) : output(pathname) {
    filename = std::filesystem::path(pathname).stem().string();
}

void CodeWriter::setFileName(std::string& pathname){
    if (output.is_open()){
        output.close();
    }
    output.open(pathname);
    filename = std::filesystem::path(pathname).stem().string();
}

void CodeWriter::WriteArithmetic(std::string command) {
    output<<"// "<<command<<"\n";
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
        output <<"@SP\nAM=M-1\nD=M\n@SP\nAM=M-1\nD=M-D\n@EQ_TRUE." + label  + "\nD;JEQ\nD=0\n@EQ_END" +label  + "\n0;JMP\n(EQ_TRUE" + label  + ")\nD=-1\n(EQ_END" + label  + ")\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
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

void CodeWriter::WritePushPOP(CommandType type, std::string segment, int index){
    std::string seg;
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
            output <<"//push constant "<<index<<"\n"
                <<"@" << index << "\n"
                << "D=A\n"
                << "@SP\n"
                << "A=M\n"
                << "M=D\n"
                << "@SP\n"
                << "M=M+1\n";
        }
        else if (segment == "static") {
            output <<"//push static "<<index<<"\n"
                << "@"<<filename<<"." << index << "\n"  
                << "D=M\n"
                << "@SP\n"
                << "A=M\n"
                << "M=D\n"
                << "@SP\n"
                << "M=M+1\n";
        }
        else if (segment == "temp" || segment == "pointer") {
            output <<"//push "<<segment<<" "<<index<<"\n"
                << "@R" << seg << "\n"
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
            output <<"//push "<<segment<<" "<<index<<"\n"
                << "@" << seg << "\n"
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
            output <<"//pop static "<<index<<"\n"
                << "@SP\n"
                << "AM=M-1\n"
                << "D=M\n"
                << "@"<<filename<<"."<< index << "\n"
                << "M=D\n";
        }
        else if (segment == "temp" || segment == "pointer") {
            output <<"//pop "<<segment<<" "<<index<<"\n"
                << "@R" << seg << "\n"
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
            output <<"//pop "<<segment<<" "<<index<<"\n"
                << "@" << seg << "\n"
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

void CodeWriter::Close() { output.close(); }

void CodeWriter::WriteInit() {
  output << "@256\n"
         << "D=A\n"
         << "@SP\n"
         << "M=D\n";
  WriteCall("Sys.init",0);
  
};
void CodeWriter::WriteLabel(std::string label){output << "(" << filename << "$" << label << ")\n";};
void CodeWriter::WriteGoto(std::string label){output << "@" << filename << "$" << label << "\n" << "0;JMP\n";};

void CodeWriter::WriteIf(std::string label) {
  output << "@SP\n"
         << "AM=M-1\n"
         << "D=M\n"
         << "@" << filename << "$" << label << "\n"
         << "D;JNE\n";
};


void CodeWriter::WriteCall(std::string functionName, int numArgs) {
    std::string returnLabel = functionName + "$ret." + std::to_string(retCounter++);
    
    // 保存返回地址
    output << "@" << returnLabel << "\n"
           << "D=A\n"
           << "@SP\n"
           << "A=M\n"
           << "M=D\n"
           << "@SP\n"
           << "M=M+1\n";
    
    // 保存 LCL, ARG, THIS, THAT
    std::vector<std::string> segments = {"LCL", "ARG", "THIS", "THAT"};
    for (const auto& seg : segments) {
        output << "@" << seg << "\n"
               << "D=M\n"
               << "@SP\n"
               << "A=M\n"
               << "M=D\n"
               << "@SP\n"
               << "M=M+1\n";
    }
    
    // 重新定位 ARG
    output << "@SP\n"
           << "D=M\n"
           << "@5\n"
           << "D=D-A\n"
           << "@" << numArgs << "\n"
           << "D=D-A\n"
           << "@ARG\n"
           << "M=D\n";
    
    // 重新定位 LCL
    output << "@SP\n"
           << "D=M\n"
           << "@LCL\n"
           << "M=D\n";
    
    // 跳转到函数
    output << "@" << functionName << "\n"
           << "0;JMP\n";
    
    // 返回地址标签
    output << "(" << returnLabel << ")\n";
}
void CodeWriter::WriteReturn() {
    // 保存返回地址到 R13（帧指针）
    output << "@LCL\n"
           << "D=M\n"
           << "@R13\n"
           << "M=D\n";
    
    // 保存返回值到 R14
    output << "@SP\n"
           << "AM=M-1\n"
           << "D=M\n"
           << "@R14\n"
           << "M=D\n";
    
    // 恢复 THAT
    output << "@R13\n"
           << "D=M\n"
           << "@1\n"
           << "A=D-A\n"
           << "D=M\n"
           << "@THAT\n"
           << "M=D\n";
    
    // 恢复 THIS
    output << "@R13\n"
           << "D=M\n"
           << "@2\n"
           << "A=D-A\n"
           << "D=M\n"
           << "@THIS\n"
           << "M=D\n";
    
    // 恢复 ARG
    output << "@R13\n"
           << "D=M\n"
           << "@3\n"
           << "A=D-A\n"
           << "D=M\n"
           << "@ARG\n"
           << "M=D\n";
    
    // 恢复 LCL
    output << "@R13\n"
           << "D=M\n"
           << "@4\n"
           << "A=D-A\n"
           << "D=M\n"
           << "@LCL\n"
           << "M=D\n";
    
    // 设置返回地址
    output << "@R13\n"
           << "D=M\n"
           << "@5\n"
           << "A=D-A\n"
           << "D=M\n"
           << "@R15\n"
           << "M=D\n";
    
    // 将返回值放到 ARG 位置
    output << "@R14\n"
           << "D=M\n"
           << "@ARG\n"
           << "A=M\n"
           << "M=D\n";
    
    // 重置 SP
    output << "@ARG\n"
           << "D=M+1\n"
           << "@SP\n"
           << "M=D\n";
    
    // 跳转到返回地址
    output << "@R15\n"
           << "A=M\n"
           << "0;JMP\n";
}




void CodeWriter::WriteFunction(std::string functionName, int numLocals) {
    // 函数入口标签
    output << "(" << functionName << ")\n";
    // 为局部变量初始化空间（push constant 0，重复 numLocals 次）
    for (int i = 0; i < numLocals; i++) {
        output << "@0\n"
               << "D=A\n"
               << "@SP\n"
               << "A=M\n"
               << "M=D\n"
               << "@SP\n"
               << "M=M+1\n";
    }
}