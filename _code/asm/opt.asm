.DATA

.CODE
;-------------------------------;

;-------------------------------;
ClockCounter PROC
   RDTSC
   SHR RDX,32
   OR RAX,RDX

   RET
ClockCounter ENDP
;-------------------------------;
END