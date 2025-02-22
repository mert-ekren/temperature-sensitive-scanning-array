    AREA |.text|, CODE, READONLY
    EXPORT DELAY100        ; Export the delay subroutine
	
DELAY100 
    PUSH    {R0, R1}                            ; Save registers R0 and R1 on the stack
    LDR     R0, =1600                         ; Outer loop count for ~150 ms delay

OUTER_LOOP
    LDR     R1, =75                        ; Inner loop count

INNER_LOOP
    NOP                                        ; No operation (1 cycle delay)
    SUBS    R1, R1, #1                          ; Decrement inner loop counter
    BNE     INNER_LOOP                          ; Repeat inner loop until R1 is 0

    SUBS    R0, R0, #1                          ; Decrement outer loop counter
    BNE     OUTER_LOOP                          ; Repeat outer loop until R0 is 0

    POP     {R0, R1}                            ; Restore registers R0 and R1 from the stack
    BX      LR                                  ; Return from subroutine
	
	ALIGN
	END