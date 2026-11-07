include inc\data.inc
.CODE
;--------------------------------------------------------;
_entry PROC
PUSH_ALL
    ;reg init ->
	mov r15,rcx
	
	call _init
	;runtimeloops ->
	;call _collision
	call _player
	
    ;end ->
POP_ALL
RET
_entry ENDP
;--------------------------------------------------------;
_init PROC
    mov al,byte ptr[player_has_init]

    cmp al,1
    jne @init
RET
@init: ;if init != 1 ->
	
    inc byte ptr[player_has_init]
	
	mov eax, 00000000h
    movd xmm0, eax
    movss dword ptr [r15 + BLOCK_POS_X_F32], xmm0
    movss dword ptr [r15 + BLOCK_POS_Y_F32], xmm0
 
RET
_init ENDP
;--------------------------------------------------------;
_collision PROC
	mov edx,dword ptr[r15 + BLOCK_COUNT_U32]
	
@begin:	 
            ;  

    dec edx
	jnz @begin
RET
_collision ENDP
;--------------------------------------------------------;
_player PROC
	mov edx, 40A00000h ;5.f
    movd xmm0, edx     ;speed.f
	movss xmm1, dword ptr[r15 + BLOCK_POS_X_F32];заранее взять
	
            test byte ptr[r15 + KEY_A], 1     ;KEY_A
	        jz @key_a_skip

	subss xmm1,xmm0
	movss dword ptr[r15 + BLOCK_POS_X_F32],xmm1
@key_a_skip:
	        test 1, byte ptr[r15 + KEY_D], 1      ;KEY_D
	        jz @key_d_skip
			
	addss xmm1, xmm0
	movss dword ptr[r15 + BLOCK_POS_X_F32],xmm1
@key_d_skip:

RET	
_player ENDP
;--------------------------------------------------------;


; === data === ;
player_has_init    DB 0
player_on_ground   DB 0
player_in_movement DB 0

; input ;

; === mymem === ;
 
END