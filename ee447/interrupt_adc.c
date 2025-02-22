#include "TM4C123GH6PM.h"
extern void DELAY200(void); 
void Timer0_Init(void);
void Comparator_Init(void) {
    SYSCTL->RCGCACMP |= (1 << 0);       // Enable clock for Analog Comparator
    SYSCTL->RCGCGPIO |= (1 << 2);       // Enable clock for Port C
    while ((SYSCTL->PRGPIO & (1 << 2)) == 0);  // Wait until Port C is ready

    GPIOC->AFSEL |= (0x40 | 0x80);      // Enable alternate function on PC6, PC7
    GPIOC->DEN &= ~(0x40 | 0x80);       // Disable digital function on PC6, PC7
    GPIOC->AMSEL |= (0x40 | 0x80);      // Enable analog function on PC6, PC7

    COMP->ACCTL0 = 0x00;                // Clear previous configuration
    COMP->ACCTL0 = (0x02 << 1);         // Set C0+ (PC6) as positive, C0- (PC7) as negative input
    COMP->ACCTL0 |= (1 << 5);           // Enable trigger output (TOEN) for level trigger
    COMP->ACCTL0 |= (1 << 6);           // Interrupt on comparator output high level
    
    COMP->ACINTEN |= 0x01;              // Enable interrupt for Comparator 0
    COMP->ACRIS |= 0x01;                // Clear any pending interrupt
	
		NVIC->IPR[10] &= ~0xF0;         // Clear priority for interrupt 41 
		NVIC->IPR[10] |= (2 << 5);      // Set priority level 2 for interrupt 41 

		NVIC->ICPR[1] |= (1 << (41 - 32)); 
    NVIC->ISER[1] |= (1 << (41 - 32));  // Enable interrupt for Analog Comparator (Interrupt 41)
		COMP->ACRIS = 0x01;  // Clear interrupt status for Comparator 0
}


// ISR for Comparator 0


void enter_deepSleep(void) {
    while (1) {
    SCB->SCR |= 0x04;                // Set Sleep-Deep bit
    __asm(" WFI");                   // Enter deep sleep, wait for interrupt
    if (COMP->ACSTAT0 == 0x02) {
        break;  // Threshold crossed it is checked, exit deep sleep loop
            }
        }
    // Step 1: Clear the interrupt flag by writing to ACRIS
    COMP->ACRIS = 0x01;  // Clear interrupt status for Comparator 0
    GPIOF->DATA ^= 0x02; // Toggle LED (assuming PF1 is configured as output)
}
int main(void) {
		Timer0_Init();
    // Enable clock for Port F and configure PF1 as output for debugging
    SYSCTL->RCGCGPIO |= (1 << 5);       // Enable clock for Port F
    while ((SYSCTL->PRGPIO & (1 << 5)) == 0);
    GPIOF->DIR |= 0x02;                 // Set PF1 as output
    GPIOF->DEN |= 0x02;                 // Enable digital function on PF1
    Comparator_Init();                  // Initialize Comparator 0

    while (1) {
				enter_deepSleep();
				__asm("nop");                   // Wait for interrupt
				DELAY200();
				__asm("nop");                   // Wait for interrupt
    }
}

void Timer0_Init(void) {
    SYSCTL->RCGCTIMER |= (1 << 0);  // Enable clock for Timer 0
    while ((SYSCTL->PRTIMER & (1 << 0)) == 0);  // Wait for Timer 0 to be ready

    SYSCTL->DSLPCLKCFG |= (1 << 1);  // Configure deep sleep clock source to 32.768 kHz

    TIMER0->CTL = 0;                // Disable Timer 0 during configuration
    TIMER0->CFG = 0x4;              // Configure for 16-bit timer
    TIMER0->TAMR = 0x12;            // Periodic mode, count down
    TIMER0->TAILR = 32768;          // Set interval (1-second timeout for 32.768 kHz clock)
    TIMER0->IMR |= (1 << 0);        // Enable timeout interrupt
    NVIC->ISER[0] |= (1 << 19);     // Enable Timer 0A interrupt (IRQ19)
    TIMER0->CTL |= (1 << 0);        // Enable Timer 0
}

// Timer 0 Interrupt Handler
void TIMER0A_Handler(void) {
    TIMER0->ICR = (1 << 0);  // Clear the timer interrupt flag
}
