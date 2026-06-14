
# 完整的构建 + 测试脚本

cmake --build build

echo ""
echo "========== Project 7: ProgramFlow tests (单文件, 无Sys.init引导) =========="
./build/VMtranslator ./test/ProgramFlow/BasicLoop/BasicLoop.vm
./build/VMtranslator ./test/ProgramFlow/FibonacciSeries/FibonacciSeries.vm

echo ""
echo "========== Project 8: FunctionCalls tests (目录, 含Sys.init引导) =========="
./build/VMtranslator ./test/FunctionCalls/FibonacciElement
./build/VMtranslator ./test/FunctionCalls/NestedCall
./build/VMtranslator ./test/FunctionCalls/SimpleFunction
./build/VMtranslator ./test/FunctionCalls/StaticsTest

