#include <iostream>
#include "Parser.hpp"
#include "CodeWriter.hpp"

namespace fs =std::filesystem;

int main(int argc, char *argv[]) {
     // 检查命令行参数：需要提供 .vm 文件或目录路径
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> " << std::endl;
        return 1;
    }
    // 存储所有 .vm 文件路径
    std::vector<fs::path> vmFiles;
    // 获取输入的路径
    std::string path=argv[1];
    if (fs::is_directory(path)) {
      // 情况1：输入的是目录 → 收集目录下所有 .vm 文件
      for (const auto &entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".vm") {
          vmFiles.push_back(entry.path());
          }
      }
    } else if (fs::is_regular_file(path) &&
               fs::path(path).extension() == ".vm") {
       // 情况2：输入的是单个 .vm 文件 → 只处理这个文件
       vmFiles.push_back(fs::path(path));
      }else {
            std::cerr << "Error: Invalid path or not a .vm file" << std::endl;
      }


    // 确定输出文件名
    std::string outputName;
    if (vmFiles.size() == 1) {

        // 单个文件：输出到文件所在目录的同名.asm
        outputName = (vmFiles[0].parent_path() / (vmFiles[0].stem().string() + ".asm")).string();
    } else {
        // 多个文件：输出到目录下的目录名.asm
        fs::path dirPath = vmFiles[0].parent_path();
        outputName = (dirPath / (dirPath.filename().string() + ".asm")).string();
    }
    CodeWriter cw(outputName);
    cw.WriteInit();  // 生成初始化代码（SP=256，调用Sys.init）
    //逐个翻译每个 .vm 文件 
    for (const auto &vmFile : vmFiles) {
      Parser parser(vmFile);
      
        while (parser.advance()) {
          auto type = parser.commandType();
         
            switch (type) {
                case C_ARITHMETIC:
                    cw.WriteArithmetic(parser.cmdType());
                    break;
                    
                case C_PUSH:
                case C_POP:
                    cw.WritePushPOP(parser.commandType(), parser.ARG1(), parser.ARG2());
                    break;
                    
                case C_LABEL:
                    cw.WriteLabel(parser.ARG1());
                    break;
                    
                case C_GOTO:
                    cw.WriteGoto(parser.ARG1());
                    break;
                    
                case C_IF:
                    cw.WriteIf(parser.ARG1());
                    break;
                    
                case C_CALL:
                    cw.WriteCall(parser.ARG1(), parser.ARG2());
                    break;
                    
                case C_RETURN:
                    cw.WriteReturn();
                    break;
                    
                case C_FUNCTION:
                    cw.WriteFunction(parser.ARG1(), parser.ARG2());
                    break;
                    
                default:
                    std::cerr << "Failed to recognize command" << std::endl;
                    break;
            }
        }      
    }

    return 0;
}