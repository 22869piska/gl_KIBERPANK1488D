; ===== размеры элементов =====
SIZEOF_F32   EQU 4
SIZEOF_U32   EQU 4
SIZEOF_U8    EQU 1

; ===== this и буфер кода (RVA от базы exe) ===== ;
THIS_RVA             EQU 05A8C0h
CODE_MEM_RVA         EQU 096000h
CODE_MEM_FROM_THIS   EQU 03B740h
ASM_CODE_SIZE        EQU 65536

; ===== BLOCK (смещения от this) ===== ;
BLOCK_POS_X_F32      EQU 0100h
BLOCK_POS_Y_F32      EQU 10A0h
BLOCK_SCALE_X_F32    EQU 2040h
BLOCK_SCALE_Y_F32    EQU 2FE0h
BLOCK_ROTATE_A_F32   EQU 3F80h
BLOCK_TEXTURE_U32    EQU 4F20h
BLOCK_FLAG_U32       EQU 5EC0h

BLOCK_COUNT_U32  EQU 5EC4h

; ===== LIGHT ===== ;
LIGHT_POS_X_F32      EQU 6EC0h
LIGHT_POS_Y_F32      EQU 6F40h
LIGHT_COLOR_R_F32    EQU 6FC0h
LIGHT_COLOR_G_F32    EQU 7040h
LIGHT_COLOR_B_F32    EQU 70C0h
LIGHT_RADIUS_F32     EQU 7140h
LIGHT_INTENSITY_F32  EQU 71C0h

LIGHT_COUNT_U32  EQU 71C4h

; ===== MEM ===== ;
MY_MEM_START_U8          EQU 7290h
MY_MEM_END_U8            EQU 7309h

; ==== no copy data ====
CAM_POS_X_F32            EQU 7310h
CAM_POS_Y_F32            EQU 7314h
CAM_ZOOM_F32             EQU 7318h

key_start_addr      EQU 731Ch
; ===== Буквы A-Z (26 байт) ===== ;
KEY_A                   EQU 731Ch
KEY_B                   EQU 731Dh
KEY_C                   EQU 731Eh
KEY_D                   EQU 731Fh
KEY_E                   EQU 7320h
KEY_F                   EQU 7321h
KEY_G                   EQU 7322h
KEY_H                   EQU 7323h
KEY_I                   EQU 7324h
KEY_J                   EQU 7325h
KEY_K                   EQU 7326h
KEY_L                   EQU 7327h
KEY_M                   EQU 7328h
KEY_N                   EQU 7329h
KEY_O                   EQU 732Ah
KEY_P                   EQU 732Bh
KEY_Q                   EQU 732Ch
KEY_R                   EQU 732Dh
KEY_S                   EQU 732Eh  ; <--- ИСПРАВЛЕНО! (а не 7334h)
KEY_T                   EQU 732Fh
KEY_U                   EQU 7330h
KEY_V                   EQU 7331h
KEY_W                   EQU 7332h
KEY_X                   EQU 7333h
KEY_Y                   EQU 7334h
KEY_Z                   EQU 7335h

; ===== Цифры 0-9 (10 байт) ===== ;
KEY_0                   EQU 7336h
KEY_1                   EQU 7337h
KEY_2                   EQU 7338h
KEY_3                   EQU 7339h
KEY_4                   EQU 733Ah
KEY_5                   EQU 733Bh
KEY_6                   EQU 733Ch
KEY_7                   EQU 733Dh
KEY_8                   EQU 733Eh
KEY_9                   EQU 733Fh

; ===== Стрелки и спецклавиши ===== ;
KEY_UP                  EQU 7340h  ; 731Ch + 24h
KEY_DOWN                EQU 7341h
KEY_LEFT                EQU 7342h
KEY_RIGHT               EQU 7343h

KEY_ESCAPE              EQU 7344h
KEY_ENTER               EQU 7345h
KEY_BACKSPACE           EQU 7346h
KEY_SPACE               EQU 7347h
KEY_TAB                 EQU 7348h
KEY_CAPS_LOCK           EQU 7349h
KEY_EQUAL               EQU 734Ah
KEY_MINUS               EQU 734Bh

; ===== Модификаторы =====
KEY_LSHIFT              EQU 734Ch
KEY_RSHIFT              EQU 734Dh
KEY_LCTRL               EQU 734Eh
KEY_RCTRL               EQU 734Fh
KEY_LALT                EQU 7350h
KEY_RALT                EQU 7351h
KEY_TAB_WAS_RELEASED    EQU 7352h

; ==========================================
; MouseState mouse; 
; ВНИМАНИЕ: 7353h  это паддинг (1 байт), чтобы выровнять float до 4 байт
; ==========================================
MOUSE_WORLD_X_F32           EQU 7354h  ; float
MOUSE_WORLD_Y_F32           EQU 7358h  ; float
MOUSE_X_F32                 EQU 735Ch  ; float
MOUSE_Y_F32                 EQU 7360h  ; float
MOUSE_SCROLL_X_F32          EQU 7364h  ; float
MOUSE_SCROLL_Y_F32          EQU 7368h  ; float

MOUSE_LEFT_DOWN         EQU 736Ch  ; bool
MOUSE_L_CLICKED         EQU 736Dh  ; bool
MOUSE_RIGHT_DOWN        EQU 736Eh  ; bool
MOUSE_R_CLICKED         EQU 736Fh  ; bool


key_end_addr        EQU 7352h
;-------------------------------------------------------------------;

; ==== PUSHALL POPALL ====;
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
	
	;sub rsp, 20h                    ; shadow space, всегда последним
ENDM

; --- МАКРОС ВОССТАНОВЛЕНИЯ ВСЕГО ---;
POP_ALL MACRO
    ;add rsp, 20h                    ; shadow space, всегда первым
	
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

GET_ADR MACRO adr, reg
         call adr
    adr:: pop reg
ENDM

     ;movss  xmm0, REAL4 PTR [r15 + CAM_POS_Y]
    ; mov    eax, 40A00000h                        ; 5.0f 
    ; movd   xmm1, eax
   ;  addss  xmm0, xmm1                            ; Y += 5.0
   ;  movss  REAL4 PTR [r15 + CAM_POS_Y], xmm0