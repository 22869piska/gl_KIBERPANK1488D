#include "jit.hpp"

#pragma comment(linker, "/SECTION:.jit,ERW")

namespace JIT_ASM {
    __declspec(allocate(".jit")) alignas(4096) unsigned char code_mem[ASM_CODE_SIZE];
    unsigned char* code_ptr = code_mem;
}