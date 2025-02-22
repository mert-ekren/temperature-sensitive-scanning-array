	AREA |.text|, CODE, READONLY
	EXPORT CheckPB6        ; Export the delay subroutine
		
CheckPB6
    ; Read the data register
    LDR R1, =0x400053FC          ; Load the address of Port B data register
    LDR R2, [R1]                 ; Read Port B data
    LSRS R2, R2, #6              ; Shift to get PB6 (bit 6)
    AND R2, R2, #0x01            ; Mask out other bits, keeping only PB6 state
    MOV R0, R2                   ; Place the result in R0
    BX LR                        ; Return from subroutine

	ALIGN
	END
