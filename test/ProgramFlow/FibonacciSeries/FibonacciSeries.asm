@256
D=A
@SP
M=D
@ARG    // push argument 1
D=M
@1
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@R3    // pop pointer 1
D=A
@1
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D
@0    // push constant 0
D=A
@SP
A=M
M=D
@SP
M=M+1
@THAT    // pop that 0
D=M
@0
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D
@1    // push constant 1
D=A
@SP
A=M
M=D
@SP
M=M+1
@THAT    // pop that 1
D=M
@1
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D
@ARG    // push argument 0
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@2    // push constant 2
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP    // sub
AM=M-1
D=M
@SP
AM=M-1
M=M-D
@SP
M=M+1
@ARG    // pop argument 0
D=M
@0
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D
($LOOP)
@ARG    // push argument 0
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
@$COMPUTE_ELEMENT
D;JNE
@$END
0;JMP
($COMPUTE_ELEMENT)
@THAT    // push that 0
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@THAT    // push that 1
D=M
@1
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP    // add
AM=M-1
D=M
@SP
AM=M-1
M=M+D
@SP
M=M+1
@THAT    // pop that 2
D=M
@2
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D
@R3    // push pointer 1
D=A
@1
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@1    // push constant 1
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP    // add
AM=M-1
D=M
@SP
AM=M-1
M=M+D
@SP
M=M+1
@R3    // pop pointer 1
D=A
@1
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D
@ARG    // push argument 0
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@1    // push constant 1
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP    // sub
AM=M-1
D=M
@SP
AM=M-1
M=M-D
@SP
M=M+1
@ARG    // pop argument 0
D=M
@0
D=D+A
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D
@$LOOP
0;JMP
($END)
