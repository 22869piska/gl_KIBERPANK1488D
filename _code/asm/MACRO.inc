
; --- МАКРОС СОХРАНЕНИЯ ВСЕГО ---;
PUSH_ALL MACRO
    ; Сохраняем целочисленные регистры (15 штук * 8 байт = 120 байт)
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Выделяем место под 16 регистров XMM (16 * 16 байт = 256 байт)
    sub rsp, 256
    ; Сохраняем XMM (используем MOVUPS, чтобы не зависеть от выравнивания)
    movups xmmword ptr [rsp + 0],   xmm0
    movups xmmword ptr [rsp + 16],  xmm1
    movups xmmword ptr [rsp + 32],  xmm2
    movups xmmword ptr [rsp + 48],  xmm3
    movups xmmword ptr [rsp + 64],  xmm4
    movups xmmword ptr [rsp + 80],  xmm5
    movups xmmword ptr [rsp + 96],  xmm6
    movups xmmword ptr [rsp + 112], xmm7
    movups xmmword ptr [rsp + 128], xmm8
    movups xmmword ptr [rsp + 144], xmm9
    movups xmmword ptr [rsp + 160], xmm10
    movups xmmword ptr [rsp + 176], xmm11
    movups xmmword ptr [rsp + 192], xmm12
    movups xmmword ptr [rsp + 208], xmm13
    movups xmmword ptr [rsp + 224], xmm14
    movups xmmword ptr [rsp + 240], xmm15
ENDM

; --- МАКРОС ВОССТАНОВЛЕНИЯ ВСЕГО ---;
POP_ALL MACRO
    
    movups xmm0,  xmmword ptr [rsp + 0]
    movups xmm1,  xmmword ptr [rsp + 16]
    movups xmm2,  xmmword ptr [rsp + 32]
    movups xmm3,  xmmword ptr [rsp + 48]
    movups xmm4,  xmmword ptr [rsp + 64]
    movups xmm5,  xmmword ptr [rsp + 80]
    movups xmm6,  xmmword ptr [rsp + 96]
    movups xmm7,  xmmword ptr [rsp + 112]
    movups xmm8,  xmmword ptr [rsp + 128]
    movups xmm9,  xmmword ptr [rsp + 144]
    movups xmm10, xmmword ptr [rsp + 160]
    movups xmm11, xmmword ptr [rsp + 176]
    movups xmm12, xmmword ptr [rsp + 192]
    movups xmm13, xmmword ptr [rsp + 208]
    movups xmm14, xmmword ptr [rsp + 224]
    movups xmm15, xmmword ptr [rsp + 240]
    add rsp, 256 ; Чистим стек от XMM

    ; В ОБРАТНОМ порядке
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax
ENDM

