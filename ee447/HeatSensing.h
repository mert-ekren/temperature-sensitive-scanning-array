#ifndef HEAT_SENSING_H
#define HEAT_SENSING_H

#include "tm4c123gh6pm.h"

// Constants
#define LM35_POSITIVE_INPUT  0x08  // AIN0 (PE3) as the positive input
#define TRIMPOT_NEGATIVE_INPUT 0x04 // AIN1 (PE2) as the negative input

// Function Prototypes
static void Comparator_Init(void);       // Initialize the Analog Comparator
static void Enter_DeepSleep(void);       // Enter Deep Sleep Mode
static void Comparator_Handler(void);    // Analog Comparator Interrupt Handler

// Analog Comparator Initialization
static void Comparator_Init(void) {
    SYSCTL_RCGCACMP_R |= 0x01;       // Enable clock for Analog Comparator
    SYSCTL_RCGCGPIO_R |= 0x10;       // Enable clock for Port E
    while ((SYSCTL_PRGPIO_R & 0x10) == 0);

    GPIO_PORTE_AFSEL_R |= (LM35_POSITIVE_INPUT | TRIMPOT_NEGATIVE_INPUT);  // Enable alternate function
    GPIO_PORTE_DEN_R &= ~(LM35_POSITIVE_INPUT | TRIMPOT_NEGATIVE_INPUT);  // Disable digital function
    GPIO_PORTE_AMSEL_R |= (LM35_POSITIVE_INPUT | TRIMPOT_NEGATIVE_INPUT); // Enable analog function

    COMP_ACCTL0_R = 0x02;            // AIN0 (PE3) as positive, AIN1 (PE2) as negative input
    COMP_ACREFCTL_R = 0x00;          // Disable internal voltage reference
    COMP_ACCTL0_R |= 0x40;           // Interrupt on rising edge
    COMP_ACMIS_R |= 0x01;            // Clear any pending interrupt
    NVIC_EN0_R |= (1 << 41);         // Enable interrupt for Analog Comparator
}

// Enter Deep Sleep Mode
static void Enter_DeepSleep(void) {
    SYSCTL_RCC_R |= SYSCTL_RCC_MOSCDIS; // Disable Main Oscillator to save power
    SCB_SCR |= 0x04;                    // Set Sleep-Deep bit in the System Control Block
    __asm(" WFI");                      // Wait for Interrupt instruction (enter deep sleep)
}

// Analog Comparator Interrupt Handler
static void Comparator_Handler(void) {
    COMP_ACMIS_R = 0x01;  // Clear the interrupt flag

    // Disable comparator interrupt to prevent further wakeups
    NVIC_DIS0_R |= (1 << 41);

    // Additional wake-up logic here
    GPIO_PORTF_DATA_R ^= 0x02; // Toggle LED (debugging)
}

#endif // HEAT_SENSING_H
