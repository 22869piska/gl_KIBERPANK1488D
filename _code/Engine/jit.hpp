#pragma once
#pragma once
#define ASM_CODE_SIZE 65536   // кратно 4096

#pragma section(".jit", read, write, execute)

namespace JIT_ASM {
    extern unsigned char code_mem[ASM_CODE_SIZE];
    extern unsigned char* code_ptr;
    typedef void (*LogicFunc)(void*);
}