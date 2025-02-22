    AREA |.text|, CODE, READONLY
    EXPORT enter_deepsleep  

enter_deepsleep
    ldr r0, =0xE000ED00      ; Load the base address of the SCB into r0
    ldr r1, [r0, #0x10]      ; Load the current value of the SCR register (offset 0x10)
    orr r1, r1, #0x04        ; Set the Sleep-Deep bit (bit 2)
    str r1, [r0, #0x10]      ; Store the updated value back into the SCR register
    wfi                      ; Wait for interrupt (enter deep sleep mode)
    bx lr                    ; Return from the subroutine

    align                    ; Ensure alignment
    END