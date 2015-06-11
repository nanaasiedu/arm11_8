# Arm11_8
C Project for Imperial Computing Summer Term - Year 1

### Part I - Emulator &mdash; &#10004;
### Part II - Assembler &mdash; &#10004;
### Part III - GPIO &mdash; &#10004;
### Part IV - Extension

### TODO

 - Look at Instruction struct and map for function pointers
 - make mov r15 branch in emulate
 - Make comments in assemble


#### Useful information

 - Using functions

    - set up stack pointer at beginning of program:
    ldr r13,=0x10000

    - call:
    add r14,r15,#8
    sub r13,r13,#4
    str r14,[r13]
    b fn_name

    - ret:
    ldr r14,[r13]
    add r13,r13,#4
    mov r15, r14
