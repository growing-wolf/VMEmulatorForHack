#include "CodeWriter.hpp"
#include "Parser.hpp"
#include <iostream>


namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  // 检查命令行参数：需要提供 .vm 文件或目录路径
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input_file> " << std::endl;
    return 1;
  }
  // 存储所有 .vm 文件路径
  std::vector<fs::path> vmFiles;
  // 获取输入的路径并确定输出文件名
  std::string outputName;
  std::string path = argv[1];
  if (fs::is_directory(path)) {
    //目录名.asm
    // 情况1：输入的是目录 → 收集目录下所有 .vm 文件
    for (const auto &entry : fs::directory_iterator(path)) {
      if (entry.is_regular_file() && entry.path().extension() == ".vm") {
        vmFiles.push_back(entry.path());
      }
    }
    // 先计算目录名
    std::string dirName = (vmFiles[0].has_extension() 
                        ? vmFiles[0].parent_path().filename() 
                        : vmFiles[0].filename()).string();
    // 再拼接输出路径
    outputName = (vmFiles[0].parent_path() / (dirName + ".asm")).string();
    
    std::cout<<outputName<<std::endl;
  } else if (fs::is_regular_file(path) && fs::path(path).extension() == ".vm") {
    //同名.asm
    outputName =
        (vmFiles[0].parent_path() / (vmFiles[0].stem().string() + ".asm"))
            .string();
    // 情况2：输入的是单个 .vm 文件 → 只处理这个文件
    vmFiles.push_back(fs::path(path));
  } else {
    std::cerr << "Error: Invalid path or not a .vm file" << std::endl;
  }
  //初始化语法解析器和编码器
  CodeWriter cw(outputName);
  cw.WriteInit(); // 生成初始化代码（SP=256，调用Sys.init）

  //逐个翻译每个 .vm 文件
  for (const auto &vmFile : vmFiles) {

    Parser parser(vmFile.string());
    cw.setFileName(vmFile.string());
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