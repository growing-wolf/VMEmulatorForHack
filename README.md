# VMtranslator —— 基于 C++17 的 Nand2Tetris 虚拟机翻译器

《计算机系统要素》（Nand2Tetris）课程中双层虚拟机翻译器的完整实现，涵盖 **项目 7**（栈算术与内存访问）和**项目 8**（程序流与函数调用协议）。将 `.vm` 源文件翻译为可执行的 Hack 汇编代码，整个代码库采用 C++17 编写，零外部依赖，架构清晰。

---

## 功能覆盖

### 内存段（项目 7）

| 段         | Hack 映射      | 访问方式              |
|-----------|---------------|---------------------|
| `local`    | `@LCL`        | 基址指针 + 偏移        |
| `argument` | `@ARG`        | 基址指针 + 偏移        |
| `this`     | `@THIS`       | 基址指针 + 偏移        |
| `that`     | `@THAT`       | 基址指针 + 偏移        |
| `pointer`  | `R3`–`R4`     | 直接寄存器             |
| `temp`     | `R5`–`R12`    | 直接寄存器             |
| `static`   | `@File.n`     | 按文件符号化           |
| `constant` | 立即数          | 字面常量值             |

### 算术与逻辑运算

`add` · `sub` · `neg` · `and` · `or` · `not` · `eq` · `gt` · `lt`

比较指令生成带有动态标签的分支代码，支持任意多次出现而不会产生标签冲突。

### 程序流控制（项目 8）

`label` · `goto` · `if-goto`

标签以文件作用域形式生成（`(FileName$label)`），确保多文件项目之间不会冲突。

### 函数调用协议（项目 8）

完整实现了 Nand2Tetris 调用约定：

- **`function f k`** — 生成函数入口标签，并为 `k` 个局部变量初始化为零。
- **`call f n`** — 压入返回地址，保存调用者的帧（`LCL`、`ARG`、`THIS`、`THAT`），为被调用者重新定位 `ARG` 和 `LCL`，然后转移控制权。
- **`return`** — 从保存的上下文中恢复调用者帧，将返回值置于 `ARG[0]`，重置 `SP`，然后跳转回调用点。

### 引导序列

`WriteInit()` 生成标准的 `SP=256` 初始化代码，后跟 `call Sys.init 0`，使每个翻译后的程序均可直接在 CPU 模拟器中运行。

---

## 架构设计

```
  .vm 源文件  ──►  Parser（解析器）  ──►  CodeWriter（代码生成器）  ──►  .asm 目标文件
```

**`Parser`**（`include/Parser.hpp`、`src/Parser.cpp`）
> 词法分析器。读取 `.vm` 文件的每一行，去除空白和注释，将每条命令拆分为词法单元，并通过 `unordered_map` 支持的枚举分发（`CommandType`）进行归类。对外暴露 `ARG1()` 和 `ARG2()` 方法用于类型化操作数获取。

**`CodeWriter`**（`include/CodeWriter.hpp`、`src/CodeWriter.cpp`）
> 汇编代码生成器。维护标签计数器和返回地址计数器，确保符号唯一性。将每种 `CommandType` 翻译为对应的 Hack 汇编序列，处理段基址解析、静态变量消歧（`FileName.n`）、帧保存/恢复以及控制流标签生成。

**`main`**（`src/main.cpp`）
> 编排层。接受单个 `.vm` 文件或目录作为输入；收集所有 `.vm` 源文件；打开单个 `.asm` 输出文件；生成引导序列；然后基于 `switch` 分发表驱动解析-生成循环，遍历所有输入文件。

设计原则：单一职责的类，解析与代码生成严格分离，除标准容器外无堆分配。

---

## 构建与运行

### 环境要求

- **编译器：** 支持 C++17（GCC ≥ 8、Clang ≥ 7、MSVC ≥ 2017）
- **构建系统：** CMake ≥ 3.10
- **依赖：** 无外部依赖，仅使用 C++ 标准库

### 构建

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

可执行文件生成在 `build/VMtranslator`。

### 使用方法

```bash
# 翻译单个 .vm 文件
./build/VMtranslator path/to/Prog.vm          # → Prog.asm

# 翻译目录下所有 .vm 文件
./build/VMtranslator path/to/FunctionCalls/   # → FunctionCalls.asm
```

单文件输入时，输出的 `.asm` 文件与源文件位于同一目录。目录输入时，会在该目录下生成以目录名命名的合并 `.asm` 文件——这是 Nand2Tetris 对多文件项目的标准约定。

---

## 测试套件

`test/` 目录包含 Nand2Tetris 官方 CPU 模拟器测试脚本，覆盖项目 7 和项目 8：

| 分类         | 测试                  | 覆盖范围                      |
|-------------|----------------------|------------------------------|
| 程序流控制    | `BasicLoop`          | `label`、`goto`、`if-goto`   |
|             | `FibonacciSeries`    | 指针算术、循环控制              |
| 函数调用      | `SimpleFunction`     | `function`、`return`         |
|             | `NestedCall`         | 多层调用嵌套、帧完整性           |
|             | `FibonacciElement`   | 递归函数调用                   |
|             | `StaticsTest`        | 多文件静态变量隔离              |

每个测试均包含 `.vm` 源文件、`.tst` 测试脚本和 `.cmp` 对比文件。可使用 Nand2Tetris 官方 CPU 模拟器或 VM 模拟器，通过提供的 `*VME.tst` 变体脚本运行。

---

## 项目结构

```
.
├── CMakeLists.txt          # CMake 构建定义
├── LICENSE                 # MIT 许可证
├── README.md
├── include/
│   ├── Parser.hpp          # 词法分析器接口 + CommandType 枚举
│   └── CodeWriter.hpp      # 代码生成器接口
├── src/
│   ├── main.cpp            # 入口点与编排
│   ├── Parser.cpp          # 词法分析器实现
│   └── CodeWriter.cpp      # 代码生成器实现
└── test/
    ├── ProgramFlow/        # 项目 7 测试用例
    └── FunctionCalls/      # 项目 8 测试用例
```

---

## 许可证

MIT — 详见 [LICENSE](./LICENSE)。
