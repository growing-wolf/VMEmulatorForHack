cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(VMemulator_C++)
set(CMAKE_CXX_STANDARD 17) #设置c++标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB SOURCES "src/*.cpp")
add_executable(VMtranslator ${SOURCES}) #自动添加源文件

target_include_directories(VMtranslator PUBLIC include)
