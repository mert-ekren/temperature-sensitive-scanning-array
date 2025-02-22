#include "TM4C123GH6PM.h"
#include <stdio.h>
#include <string.h>

extern void DELAY200(void); // Modified version of the delay subroutine I used in exp 2
extern void OutStr(char* s);

void GPIO_Init(void);
void Timer_Init(void);
void Trigger_TrigPin(void);
unsigned int Measure_PulseWidth(void);

int main() {
	
	unsigned int pulse_w = 0;
	char result[50] = {0};
	
	// Initialize GPIO and Timer
	GPIO_Init();
	Timer_Init();
	
	// Main loop
	while(1) {
		// Trigger the ultrasonic sensor
		Trigger_TrigPin();
		
		// Measure the pulse width from the Echo pin
		pulse_w = Measure_PulseWidth();
		
		// Calculate and display the distance
		sprintf(result, "Distance: %d cm\r\4", 34 * pulse_w / 2000);  
		OutStr(result); 
	}
	
	return 0;
}

// Function to initialize GPIO pins
void GPIO_Init(void) {
    // Enable clock for GPIOA
    SYSCTL->RCGCGPIO |= (1 << 0); // Enable GPIOA clock

    // Wait for clock to stabilize
    while ((SYSCTL->PRGPIO & (1 << 0)) == 0) {}

    // Configure PA4-PA7 as outputs for ULN2003A (Stepper Motor Control)
    GPIOA->DIR |= 0xF0;       // Set PA4-PA7 as outputs
    GPIOA->DEN |= 0xF0;       // Enable digital function on PA4-PA7
		// Enable the clock for GPIOB
		SYSCTL->RCGCGPIO |= 0x02; 
		__ASM("NOP");
		__ASM("NOP");
		__ASM("NOP");
		
		// Configure PB4 as input (Echo Pin)
		GPIOB->DIR			&= ~(0x10); // PB4 input
		GPIOB->AFSEL		|= 0x10;    // Enable alternative function (Timer 1)
		GPIOB->PCTL			&= ~(0x000F0000); // Clear PCTL bits for PB4
		GPIOB->PCTL			|= (0x00070000); // PB4 -> Timer 1  
		GPIOB->AMSEL		= 0;       // Disable analog mode
		GPIOB->DEN			|= 0x10;   // Enable digital function on PB4
		
		// Configure PB5 as output (TRIG Pin)
		GPIOB->DIR			|= 0x20;   // PB5 output
		GPIOB->DEN			|= 0x20;   // Enable digital function on PB5
		GPIOB->AMSEL		|= 0;      // Disable analog mode on PB5
}

// Function to initialize Timer 1
void Timer_Init(void) {
	// Enable the clock for Timer 1
	SYSCTL->RCGCTIMER	|= 0x02; // Start Timer 1
	__ASM("NOP");
	__ASM("NOP");
	__ASM("NOP");
	
	// Configure Timer 1
	TIMER1->CTL			&= 0xFFFFFFFE; // Disable timer during setup
	TIMER1->CFG			= 0x04;  // Set 16-bit mode
	TIMER1->TAMR		= 0x07;  // Set capture, edge time, countdown
	TIMER1->CTL			|= 0x0C; // Both edges
	TIMER1->TAPR		= 15;    // Extend
	TIMER1->CTL			|= 0x03; // Bit 0 to enable and bit 1 to stall on debug
}

// Function to trigger the ultrasonic sensor (TRIG Pin)
void Trigger_TrigPin(void) {
	// Send the TRIG signal (PB5)
	GPIOB->DATA &= ~(0x20); // Set PB5 low
	for(volatile int i = 0; i < 1600; i++) {
		__ASM("NOP");
	}
	GPIOB->DATA |= 0x20;  // Set PB5 high
	for(volatile int i = 0; i < 1600; i++) {
		__ASM("NOP");
	}
	GPIOB->DATA &= ~(0x20); // Set PB5 low again
}

// Function to measure the pulse width from the Echo Pin (PB4)
unsigned int Measure_PulseWidth(void) {
	unsigned int pos_edge_time = 0;
	unsigned int neg_edge_time = 0;

	// Wait for positive edge on PB4 (Echo Pin)
	while((TIMER1->RIS & 0x04) != 0x04) {
	}
	pos_edge_time = TIMER1->TAR; 
	TIMER1->ICR |= 0x04;  // Clear the interrupt flag
	
	// Wait for negative edge on PB4 (Echo Pin)
	while((TIMER1->RIS & 0x04) != 0x04) {
	}
	neg_edge_time = TIMER1->TAR;  
	TIMER1->ICR |= 0x04;  // Clear the interrupt flag
	
	// Calculate the pulse width (in clock cycles)
	return (pos_edge_time - neg_edge_time) / 16; 
}
