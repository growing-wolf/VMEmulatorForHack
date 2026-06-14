#include "CodeWriter.hpp"
#include <filesystem>

CodeWriter::CodeWriter(const std::string& pathname) : output(pathname) {
    filename = std::filesystem::path(pathname).stem().string();
}

void CodeWriter::setFileName(std::string pathname){
    filename = std::filesystem::path(pathname).stem().string();
}

void CodeWriter::WriteArithmetic(std::string command) {
    if(command=="add"){
        output << "@SP    // add\nAM=M-1\nD=M\n@SP\nAM=M-1\nM=M+D\n@SP\nM=M+1\n";
    }
    if(command == "sub") {
        output << "@SP    // sub\nAM=M-1\nD=M\n@SP\nAM=M-1\nM=M-D\n@SP\nM=M+1\n";
    }
    if(command == "neg") {
        output << "@SP    // neg\nAM=M-1\nM=-M\n@SP\nM=M+1\n";
    }
    if(command == "and") {
        output << "@SP    // and\nAM=M-1\nD=M\n@SP\nAM=M-1\nM=M&D\n@SP\nM=M+1\n";
    }
    if(command == "or") {
        output << "@SP    // or\nAM=M-1\nD=M\n@SP\nAM=M-1\nM=M|D\n@SP\nM=M+1\n";
    }
    if(command == "not") {
        output << "@SP    // not\nAM=M-1\nM=!M\n@SP\nM=M+1\n";
    }
    if(command == "eq") {
        std::string label = std::to_string(labelCounter++);
        output << "@SP    // eq\nAM=M-1\nD=M\n@SP\nAM=M-1\nD=M-D\n@EQ_TRUE." + label  + "\nD;JEQ\nD=0\n@EQ_END." +label  + "\n0;JMP\n(EQ_TRUE." + label  + ")\nD=-1\n(EQ_END." + label  + ")\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    }
    if(command == "gt") {
        std::string label = std::to_string(labelCounter++);
        output << "@SP    // gt\nAM=M-1\nD=M\n@SP\nAM=M-1\nD=M-D\n@GT_TRUE." + label  + "\nD;JGT\nD=0\n@GT_END." + label  + "\n0;JMP\n(GT_TRUE." + label  + ")\nD=-1\n(GT_END." + label  + ")\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    }
    if(command == "lt") {
        std::string label = std::to_string(labelCounter++);
        output << "@SP    // lt\nAM=M-1\nD=M\n@SP\nAM=M-1\nD=M-D\n@LT_TRUE." + label + "\nD;JLT\nD=0\n@LT_END." + label + "\n0;JMP\n(LT_TRUE." + label + ")\nD=-1\n(LT_END." + label + ")\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
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
            output << "@" << index << "    // push constant " << index << "\n"
                << "D=A\n"
                << "@SP\n"
                << "A=M\n"
                << "M=D\n"
                << "@SP\n"
                << "M=M+1\n";
        }
        else if (segment == "static") {
            output << "@" << filename << "." << index << "    // push static " << index << "\n"
                << "D=M\n"
                << "@SP\n"
                << "A=M\n"
                << "M=D\n"
                << "@SP\n"
                << "M=M+1\n";
        }
        else if (segment == "temp" || segment == "pointer") {
            output << "@R" << seg << "    // push " << segment << " " << index << "\n"
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
            output << "@" << seg << "    // push " << segment << " " << index << "\n"
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
            output << "@SP    // pop static " << index << "\n"
                << "AM=M-1\n"
                << "D=M\n"
                << "@" << filename << "." << index << "\n"
                << "M=D\n";
        }
        else if (segment == "temp" || segment == "pointer") {
            output << "@R" << seg << "    // pop " << segment << " " << index << "\n"
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
            output << "@" << seg << "    // pop " << segment << " " << index << "\n"
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

void CodeWriter::WriteInit(bool bootstrap) {
  output << "@256\n"
         << "D=A\n"
         << "@SP\n"
         << "M=D\n";
  if (bootstrap) {
    WriteCall("Sys.init", 0);
  }
};
void CodeWriter::WriteLabel(std::string label){output << "(" << currentFunction << "$" << label << ")\n";};
void CodeWriter::WriteGoto(std::string label){output << "@" << currentFunction << "$" << label << "\n" << "0;JMP\n";};

void CodeWriter::WriteIf(std::string label) {
  output << "@SP\n"
         << "AM=M-1\n"
         << "D=M\n"
         << "@" << currentFunction << "$" << label << "\n"
         << "D;JNE\n";
};


void CodeWriter::WriteCall(std::string functionName, int numArgs) {
    //简化下层翻译，默认vm文件函数名已包含文件名前缀
    std::string returnLabel = functionName + "$ret." + std::to_string(retCounter++);
    std::string functionLabel= functionName;
    // 保存返回地址
    output << "@" << returnLabel <<"     "<<"//call"<<" "<<functionName<<" "<<numArgs<<"\n"
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
    output << "@" << functionLabel << "\n"
           << "0;JMP\n";

    // 返回地址标签
    output << "(" << returnLabel << ")\n";
}
void CodeWriter::WriteReturn() {
    // FRAME = LCL
    output << "@LCL    // return\n"
           << "D=M\n"
           << "@R13\n"
           << "M=D\n";

    // RET = *(FRAME-5)
    output << "@5\n"
           << "A=D-A\n"
           << "D=M\n"
           << "@R15\n"
           << "M=D\n";

    // *ARG = pop()  —— 用当前的ARG（调用者设置的），在恢复之前
    output << "@SP\n"
           << "AM=M-1\n"
           << "D=M\n"
           << "@ARG\n"
           << "A=M\n"
           << "M=D\n";

    // SP = ARG + 1
    output << "@ARG\n"
           << "D=M+1\n"
           << "@SP\n"
           << "M=D\n";

    // 恢复 THAT = *(FRAME-1)
    output << "@R13\n"
           << "D=M\n"
           << "@1\n"
           << "A=D-A\n"
           << "D=M\n"
           << "@THAT\n"
           << "M=D\n";

    // 恢复 THIS = *(FRAME-2)
    output << "@R13\n"
           << "D=M\n"
           << "@2\n"
           << "A=D-A\n"
           << "D=M\n"
           << "@THIS\n"
           << "M=D\n";

    // 恢复 ARG = *(FRAME-3)
    output << "@R13\n"
           << "D=M\n"
           << "@3\n"
           << "A=D-A\n"
           << "D=M\n"
           << "@ARG\n"
           << "M=D\n";

    // 恢复 LCL = *(FRAME-4)
    output << "@R13\n"
           << "D=M\n"
           << "@4\n"
           << "A=D-A\n"
           << "D=M\n"
           << "@LCL\n"
           << "M=D\n";

    // goto RET
    output << "@R15\n"
           << "A=M\n"
           << "0;JMP\n";
}




void CodeWriter::WriteFunction(std::string functionName, int numLocals) {

  // 函数入口标签
    currentFunction=functionName;
    std::string functionLabel="(" +functionName + ")";
    output <<  functionLabel << "    // function " << functionName << " " << numLocals << "\n";
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