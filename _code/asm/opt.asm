include MACRO.inc
.DATA

public RuntimeProc_START
public RuntimeProc_END

RESULT REAL4 0.0

.CODE
;-------------------------------;
;СДЕСЬ СПЕЦЕФИЧНЫЕ ПРОЦЕДУРЫ ИЛИ УЛЬТРА ОПТИМИЗАЦИИ
;-------------------------------;
ClockCounter PROC
   RDTSC
   SHR RDX,32
   OR RAX,RDX

   RET
ClockCounter ENDP
;-------------------------------;

; --- ВЕЗДЕ СМЕЩЕНИЕ ПО 4 * REG (Так как везде float\uint 4байта  ) 
BLOCK_BASE equ 256
LIGHT_BASE equ 36352
; --- СМЕЩЕНИЯ ВНУТРИ BLOCK (Размер массива = 4000 байт)
B_POS_X         equ 0
B_POS_Y         equ 4000
B_SCALE_X       equ 8000
B_SCALE_Y       equ 12000
B_ROTATE        equ 16000
B_COLOR_R       equ 20000
B_COLOR_G       equ 24000
B_COLOR_B       equ 28000
B_TEXTURE       equ 32000
B_COUNT         equ 36000   ; (unsigned int)

; --- СМЕЩЕНИЯ ВНУТРИ LIGHT (Размер массива = 128 байт)
L_POS_X       equ 0
L_POS_Y       equ 128
L_COLOR_R     equ 256
L_COLOR_G     equ 384
L_COLOR_B     equ 512
L_RADIUS      equ 640
L_INTENSITY   equ 768
L_COUNT       equ 896     ; (unsigned int)

; --- OFFSET MY_MEMORY (Размер массива = 128 байт) можно сделать больше
MY_MEM        equ 37328

;.CODE
;-------------------------------;
;MAIN RUNTIMEPROC  ; rcx = this->                         ;код копируется в *code_ptr = virtualalloc()
;-------------------------------;
RuntimeProc PROC
  RuntimeProc_START::
  PUSH_ALL
  MOV R15,RCX

  ;COMMENT %
;mov block->[1] += 0.4
      MOV EAX,4089999Ah          ;  4.3f
      MOVD XMM0,EAX 

      MOVSS XMM1,DWORD PTR[R15 + BLOCK_BASE + B_POS_Y + 4]
      ADDSS XMM0,XMM1
      MOVSS DWORD PTR[R15 + BLOCK_BASE + B_POS_Y + 4],XMM0 

      
      ;IF light->[0] < 500 OR > 500 +-= 4.3f
      MOVD XMM1,EAX  ; в EAX уже сували и он не испорчен
     
      MOV EDX,043FA0000h ;500.f
      MOVD XMM2,EDX
      MOV ECX,0C3FA0000h ;-500.f
      MOVD XMM3,ECX
        
      MOVSS XMM0,DWORD PTR[R15 + LIGHT_BASE + B_POS_X + 0]

      COMISS XMM0,XMM2   ;сравнить с 500.f
      JAE @IF_GE

      COMISS XMM0,XMM3   ;сравнить с -500.f
      JBE @IF_LE
      JMP @SKIP1         ;skip если все лоожь

      @IF_LE:
        MOV BYTE PTR[R15 + MY_MEM], 0
        JMP @SKIP1
      @IF_GE:
        MOV BYTE PTR[R15 + MY_MEM], 1

      @SKIP1:            ; <- ->

      CMP BYTE PTR[R15 + MY_MEM], 0
      JE @ADD
        SUBSS XMM0,XMM1
        MOVSS DWORD PTR[R15 + LIGHT_BASE + B_POS_X + 0],XMM0 
      
        JMP @SKIP2
      @ADD:
        ADDSS XMM0,XMM1
        MOVSS DWORD PTR[R15 + LIGHT_BASE + B_POS_X + 0],XMM0 

      @SKIP2:
;%
      
  ; --- ВЫХОД ---;
  POP_ALL
  RET
  RuntimeProc_END::
RuntimeProc ENDP
;-------------------------------;
END

