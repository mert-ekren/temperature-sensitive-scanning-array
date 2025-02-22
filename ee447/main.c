#include "TM4C123GH6PM.h"
extern void DELAY200(void);
extern void DELAY2(void);
extern void DELAY100(void);
void ADC0_Init(void);
void ADC1_Init(void);
int Read_ADC0_Value(int channel);
int Read_ADC1_Value(int channel);
void Timer0_Init(void);
void enter_deepSleep(void);

int main(void) {
    // Enable clock for GPIO Port F
    SYSCTL->RCGCGPIO |= (1 << 5);
    while ((SYSCTL->PRGPIO & (1 << 5)) == 0);

    // Configure PF1 as output (debug LED)
    GPIOF->DIR |= 0x02;      
    GPIOF->DEN |= 0x02;      

    ADC0_Init();        
    ADC1_Init();        
    Timer0_Init();      

    while (1) {

        enter_deepSleep();  // Enter deep sleep and, wait interrupt

        // Continue main program after threshold is crossed
        DELAY200();
				DELAY200();
				DELAY200();
				DELAY200();
				DELAY200();
				DELAY200();
				DELAY200();
				DELAY200();
        GPIOF->DATA &= ~0x02;  // Turn off LED
        DELAY200();

        // Re-enable Timer 0 interrupt at the end of the loop
        TIMER0->IMR |= (1 << 0);  // Re-enable Timer 0 interrupt
    }

    return 0;
}

void ADC0_Init(void) {
    SYSCTL->RCGCADC |= (1 << 0);     
    SYSCTL->RCGCGPIO |= (1 << 4);     
    while ((SYSCTL->PRGPIO & (1 << 4)) == 0);

    GPIOE->AFSEL |= 0x04;           
    GPIOE->DEN &= ~0x04;             
    GPIOE->AMSEL |= 0x04;          

    ADC0->ACTSS &= ~(1 << 3);       
    ADC0->EMUX &= ~0xF000;        
    ADC0->SSMUX3 = 1;             
    ADC0->SSCTL3 = 0x06;            
    ADC0->ACTSS |= (1 << 3);       
}

void ADC1_Init(void) {
    SYSCTL->RCGCADC |= (1 << 1);     
    SYSCTL->RCGCGPIO |= (1 << 4);    
    while ((SYSCTL->PRGPIO & (1 << 4)) == 0);

    GPIOE->AFSEL |= 0x08;        
    GPIOE->DEN &= ~0x08;          
    GPIOE->AMSEL |= 0x08;          

    ADC1->ACTSS &= ~(1 << 3);      
    ADC1->EMUX &= ~0xF000;       
    ADC1->SSMUX3 = 0;              
    ADC1->SSCTL3 = 0x06;          
    ADC1->ACTSS |= (1 << 3);         
}

int Read_ADC0_Value(int channel) {
    ADC0->SSMUX3 = channel;          
    ADC0->PSSI |= (1 << 3);         
    while ((ADC0->RIS & (1 << 3)) == 0);
    int result = ADC0->SSFIFO3 & 0xFFF; 
    ADC0->ISC = (1 << 3);           
    return result;
}

int Read_ADC1_Value(int channel) {
    ADC1->SSMUX3 = channel;         
    ADC1->PSSI |= (1 << 3);           
    while ((ADC1->RIS & (1 << 3)) == 0); 
    int result = ADC1->SSFIFO3 & 0xFFF; 
    ADC1->ISC = (1 << 3);      
    return result;
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

void enter_deepSleep(void) {
    while (1) {
    SCB->SCR |= 0x04;                // Set Sleep-Deep bit
    __asm(" WFI");                   // Enter deep sleep, wait for interrupt

    int lm35_value = Read_ADC0_Value(1);  // Read LM35 value (AIN0)
    DELAY200();
    int trimpot_value = Read_ADC1_Value(0);  // Read Trimpot value (AIN1)
    DELAY200();
    if (lm35_value > trimpot_value) {
				GPIOF->DATA |= 0x02;   // Turn on LED
        TIMER0->IMR &= ~(1 << 0);  // Disable Timer 0 interrupt
        break;  // Threshold crossed, exit deep sleep loop
            }
        }
}

// Timer 0 Interrupt Handler
void TIMER0A_Handler(void) {
    TIMER0->ICR = (1 << 0);  // Clear the timer interrupt flag
}
