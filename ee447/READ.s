    AREA |.text|, CODE, READONLY
    ; Define register addresses and constants
    ;SYSCTL_RCGCGPIO    EQU 0x400FE608
	
    ;GPIOC_LOCK         EQU 0x40006520
    ;GPIOC_CR           EQU 0x40006524
    ;GPIOC_DIR          EQU 0x40006400
    ;GPIOC_PUR          EQU 0x40006510
    ;GPIOC_DEN          EQU 0x4000651C
    ;GPIOC_DATA         EQU 0x400063FC
	
    ;GPIOD_CR           EQU 0x40007524
    ;GPIOD_DIR          EQU 0x40007400
    ;GPIOD_PUR          EQU 0x40007510
    ;GPIOD_DEN          EQU 0x4000751C
    ;GPIOD_DATA         EQU 0x400073FC
	
    ;GPIOE_CR           EQU 0x40008524
    ;GPIOE_DIR          EQU 0x40008400
    ;GPIOE_PUR          EQU 0x40008510
    ;GPIOE_DEN          EQU 0x4000851C
    ;GPIOE_DATA         EQU 0x400083FC
	
    ;UNLOCK_VALUE       EQU 0x4C4F434B

    EXPORT READ 

READ
; Function: port_Init1
port_Init1
    ; Enable clock to PORTE and PORTD
    LDR R0, =0x400FE608
    LDR R1, [R0]
    ORR R1, R1, #0x18
    STR R1, [R0]

    ; Wait for PORTC and PORTD clocks to stabilize
wait_PortE
    LDR R1, [R0]
    TST R1, #0x10
    BEQ wait_PortE

wait_PortD
    LDR R1, [R0]
    TST R1, #0x08
    BEQ wait_PortD

    ; Configure PORTD (PD0-PD3) as outputs
    LDR R0, =0x40007524
    LDR R1, [R0]
    ORR R1, R1, #0x0F
    STR R1, [R0]

    LDR R0, =0x40007400
    LDR R1, [R0]
    ORR R1, R1, #0x0F
    STR R1, [R0]

    LDR R0, =0x4000751C
    LDR R1, [R0]
    ORR R1, R1, #0x0F
    STR R1, [R0]

    LDR R0, =0x400073FC
    LDR R1, [R0]
    BIC R1, R1, #0x0F
    STR R1, [R0]

    ; Configure PORTC (PE2-PE5) as inputs with pull-up resistors
    LDR R0, =0x40024400
    LDR R1, [R0]
    BIC R1, R1, #0x3C
    STR R1, [R0]

    LDR R0, =0x40024510
    LDR R1, [R0]
    ORR R1, R1, #0x3C
    STR R1, [R0]

    LDR R0, =0x4002451C
    LDR R1, [R0]
    ORR R1, R1, #0x3C
    STR R1, [R0]

; Function: read_PortC
read_PortC
    LDR R0, =0x400243FC
    LDR R1, [R0]
    AND R1, R1, #0x3C
	LSL R1,#2
    CMP R1, #0xF0
    BEQ port_Init1
	
	MOV R7,R1

; Function: port_Init2
port_Init2
    ; Enable clock to PORTC and PORTD
    LDR R0, =0x400FE608
    LDR R1, [R0]
    ORR R1, R1, #0x18
    STR R1, [R0]

    ; Wait for PORTC and PORTD clocks to stabilize
wait_PortE2
    LDR R1, [R0]
    TST R1, #0x10
    BEQ wait_PortE2

wait_PortD2
    LDR R1, [R0]
    TST R1, #0x08
    BEQ wait_PortD2

    ; Configure PORTC (PC4-PC7) as outputs
    LDR R0, =0x40024400
    LDR R1, [R0]
    ORR R1, R1, #0x3C
    STR R1, [R0]

    LDR R0, =0x4002451C
    LDR R1, [R0]
    ORR R1, R1, #0x3C
    STR R1, [R0]

    LDR R0, =0x400243FC
    LDR R1, [R0]
    BIC R1, R1, #0x3C
    STR R1, [R0]

    ; Configure PORTD (PD0-PD3) as inputs with pull-up resistors
    LDR R0, =0x40007400
    LDR R1, [R0]
    BIC R1, R1, #0x0F
    STR R1, [R0]

    LDR R0, =0x40007510
    LDR R1, [R0]
    ORR R1, R1, #0x0F
    STR R1, [R0]

    LDR R0, =0x4000751C
    LDR R1, [R0]
    ORR R1, R1, #0x0F
    STR R1, [R0]

; Function: read_PortD
read_PortD
	
    LDR R0, =0x400073FC
    LDR R1, [R0]
    AND R1, R1, #0x0F

    CMP R1, #0x0F
    BEQ port_Init2
	
read_for
	
    LDR R0, =0x400073FC
    LDR R3, [R0]
    AND R3, R3, #0x0F

    CMP R3, #0x0F
    BNE read_for

; Function: calculateThresholdTemp
calculateThresholdTemp
    CMP R7, #0x70
    BEQ row1
    CMP R7, #0xB0
    BEQ row2
    CMP R7, #0xD0
    BEQ row3
    CMP R7, #0xE0
    BEQ row4

row1
    CMP R1, #0x07
    BEQ temp0
    CMP R1, #0x0B
    BEQ temp1
    CMP R1, #0x0D
    BEQ temp2
    CMP R1, #0x0E
    B   temp3

row2
    CMP R1, #0x07
    BEQ temp4
    CMP R1, #0x0B
    BEQ temp5
    CMP R1, #0x0D
    BEQ temp6
    CMP R1, #0x0E
    B temp7

row3
    CMP R1, #0x07
    BEQ temp8
    CMP R1, #0x0B
    BEQ temp9
    CMP R1, #0x0D
    BEQ temp10
    CMP R1, #0x0E
    B temp11

row4
    CMP R1, #0x07
    BEQ temp12
    CMP R1, #0x0B
    BEQ temp13
    CMP R1, #0x0D
    BEQ temp14
    CMP R1, #0x0E
    B temp15

temp0
    LDR R0,= 0
    BX LR
temp1
    LDR R0,= 1
    BX LR
temp2
    LDR R0,= 2
    BX LR
temp3
    LDR R0,= 3
    BX LR
temp4
    LDR R0,= 4
    BX LR
temp5
    LDR R0,= 5
    BX LR
temp6
    LDR R0,= 6
    BX LR
temp7
    LDR R0,= 7
    BX LR
temp8
    LDR R0,= 8
    BX LR
temp9
    LDR R0,= 9
    BX LR  
temp10
    LDR R0,= 10
    BX LR  
temp11
    LDR R0,= 11
    BX LR  
temp12
    LDR R0,= 12
    BX LR  
temp13
    LDR R0,= 13
    BX LR  
temp14
    LDR R0,= 14
    BX LR  
temp15
    LDR R0,= 15
    BX LR  
	align
	end

