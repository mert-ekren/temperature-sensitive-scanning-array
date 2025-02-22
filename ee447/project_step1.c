#include "TM4C123GH6PM.h"
#include <stdio.h>
#include <string.h>
#include "PLL.h"
#include "Nokia5110.h"
#include "I2C0.h"
#include "valvtm4c123gh6pm.h"
// external functions mostly buttons and keypad and delays
extern void DELAY200(void); 
extern void DELAY2(void);
extern void DELAY100(void);
extern void OutStr(char* s);
extern int  CheckPB6(void);
extern void enter_deepsleep(void);
extern int READ(void);

// Global variables to store button states
volatile uint8_t button1 = 0;
volatile uint8_t button2 = 0;
volatile uint8_t button3 = 0;
volatile uint8_t button4 = 0;
volatile int threshhold = 0;


// Function prototypes

void GPIO_Init(void);
void Timer_Init(void);
void Trigger_TrigPin(void);
unsigned int Measure_PulseWidth(void);
void Step_Motor(uint8_t step_no);
void Delay(void); // Function prototype for delay
void Comparator_Init(void);
void Timer0_Init(void);
void enter_deepSleep(void);
void configure_pd6_output(void);
void configure_pd7_output(void);
void COMP0_Handler(void);

int main(void) {        
    Timer0_Init();     
    GPIO_Init();
		DELAY200();
    Timer_Init();
		Comparator_Init();
		configure_pd6_output();
	
    while (1) {
				int deneme =0;
				uint8_t step_no = 1; // Starting step
				unsigned int pulse_w = 0;
				char result[50] = {0};
				int stop=1;
				int total_angles=0;
				unsigned int distance_count = 0; // Counter for valid distances
				unsigned int distance = 0; // Counter for valid distances
				unsigned int total_distance = 0; // Cumulative sum of distances
				int rotation=0;
				rotation = 0;
				float temp_thresh =0;
				float gercek_thresh=0;
				float temp_i2c =0;
				float gercek_temp=0;
				__asm("nop");
				__asm("nop");
				__asm("nop");
				DELAY200();
				button1 = READ();
				__asm("nop");
				__asm("nop");
				__asm("nop");
				DELAY200();
				button2 = READ();
				__asm("nop");
				__asm("nop");
				__asm("nop");
				DELAY200();
				button3 = READ();
				__asm("nop");
				__asm("nop");
				__asm("nop");
				DELAY200();
				button4 = READ();
				__asm("nop");
				__asm("nop");
				__asm("nop");
				DELAY200();
				threshhold =button1*1000+button2*100+button3*10+button4;
				__asm("nop");
				__asm("nop");
				__asm("nop");
				temp_thresh=threshhold;
				gercek_thresh=temp_thresh/100;
				let_there_be_tt(gercek_thresh);
				Timer0_Init(); 
        enter_deepSleep();  // Enter deep sleep and, wait interrupt  
				// bundan sonrasi
				GPIO_Init(); 
				configure_pd6_output();
				DELAY200();
				Timer_Init();
				Comparator_Init();
				configure_pd7_output();
				GPIOD->DATA |=0x80;
				// bundan öncesi her lcd den sonra gelecek yoksa sistem siçiyor
				rotation = 0;
				gercek_temp=temp_i2c/100;
				TIMER0->IMR &= ~(1 << 0); // this is to disable the timer after its initialization for the distance  measurement i messed up
				while(1){
					temp_i2c = let_there_be_temp();
					gercek_temp=temp_i2c/100;
					DELAY100();
					if (gercek_temp>gercek_thresh){
						break;
					}
				}
				DELAY100();
				stop=CheckPB6();
				DELAY100();
				stop=CheckPB6();
				DELAY100();
				for (int i=0;i<20;i++){
					for (int i=0;i<30;i++){
						GPIOD->DATA^=0x40;
						DELAY2();
					}
					GPIOD->DATA &=~0x40;
					DELAY100();
				}
				GPIOD->DATA &=~0x40;
        // Move counterclockwise 6 steps 180 times (half-step mode)
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 34; j++) {
                // Counterclockwise half-step decrement using switch
                switch (step_no) {
                    case 1:
                        step_no = 8;
                        Delay();
                        break;
                    case 2:
                        step_no = 1;
                        Delay();
                        break;
                    case 3:
                        step_no = 2;
                        Delay();
                        break;
                    case 4:
                        step_no = 3;
                        Delay();
                        break;
                    case 5:
                        step_no = 4;
                        Delay();
                        break;
                    case 6:
                        step_no = 5;
                        Delay();
                        break;
                    case 7:
                        step_no = 6;
                        Delay();
                        break;
                    case 8:
                        step_no = 7;
                        Delay();
                        break;
                    default:
                        step_no = 1; // Default to step 1 if invalid state
                        Delay();
                        break;
                }
                Step_Motor(step_no);
                Delay();  // Add a delay between steps
                
            }
						DELAY100();
            // Trigger the ultrasonic sensor
            Trigger_TrigPin();
                    
            // Measure the pulse width from the Echo pin
            pulse_w = Measure_PulseWidth();
                    
            // Calculate and display the distance
            distance= 34 * pulse_w / 2000;
						
            if (distance < 80 && distance_count < 60) {
                distance_count++;
                total_distance += distance; // Add the distance to the total
								total_angles+=i;
								rotation++;
            }
        }
				temp_i2c = let_there_be_temp();
				gercek_temp=temp_i2c/100;
				if (distance_count !=0){
					total_distance=total_distance/distance_count;
					__asm(" NOP");
					__asm(" NOP");
					__asm(" NOP");
					total_angles=total_angles/distance_count;
					total_angles=total_angles*3;
					__asm(" NOP");
					__asm(" NOP");
					__asm(" NOP");
					sprintf(result, "Distance: %d cm, Angle: %d\r\4", total_distance, total_angles-90);
 
					OutStr(result); 
					
					sprintf(result, "threshhold: %d C/100 \r\4", threshhold);
 
					OutStr(result); 
					if (total_distance >= 75 && total_distance <= 100) {
							GPIOF->DATA |= (1 << 3);  // Turn on green LED (PF3), turn off others
					} else if (total_distance >= 50 && total_distance < 75) {
							GPIOF->DATA |= (1 << 2);  // Turn on blue LED (PF2), turn off others
					} else if (total_distance < 50) {
							GPIOF->DATA |= (1 << 1);  // Turn on red LED (PF1), turn off others
					} else {
							GPIOF->DATA |= 0x00;      // Turn off all LEDs if out of range
					}
					let_there_be_screen(1,gercek_thresh,gercek_temp,total_angles,total_distance);
					GPIO_Init();
					configure_pd7_output();
					GPIOD->DATA |=0x80;					
					DELAY200();
					Timer_Init();
					Comparator_Init();
					deneme=1;
				}
				else{
					sprintf(result, "Distance and angle not found \r\4"); 
					OutStr(result); 
					
					sprintf(result, "threshhold: %d C/100 \r\4", threshhold);
 
					OutStr(result); 
					let_there_be_screen(0,gercek_thresh,gercek_temp,total_angles,total_distance);
					GPIO_Init(); 
					configure_pd7_output();
					GPIOD->DATA |=0x80;
					DELAY200();
					Timer_Init();
					Comparator_Init();
				}
				configure_pd7_output();
				GPIOD->DATA |=0x80;
				configure_pd6_output();
				GPIOD->DEN &= ~0x40;               // Enable PD6 as a digital pin
        // Move clockwise 1080 steps (half-step mode)
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 34; j++) {
							switch (step_no) {
									case 1:
											step_no = 2;
											Delay();
											break;
									case 2:
											step_no = 3;
											Delay();
											break;
									case 3:
											step_no = 4;
											Delay();
											break;
									case 4:
											step_no = 5;
											Delay();
											break;
									case 5:
											step_no = 6;
											Delay();
											break;
									case 6:
											step_no = 7;
											Delay();
											break;
									case 7:
											step_no = 8;
											Delay();
											break;
									case 8:
											step_no = 1;
											Delay();
											break;
									default:
											step_no = 1; // Default to step 1 if invalid state
											Delay();
											break;
							}
            Step_Motor(step_no);
            Delay();  // Add a delay between steps
        }
				DELAY100();
			}
				temp_i2c = let_there_be_temp();
				gercek_temp=temp_i2c/100;
				while(1){
						temp_i2c = let_there_be_temp();
						gercek_temp=temp_i2c/100;
						stop=CheckPB6();
						if(stop == 0){
								break;
						}
						DELAY200();
						let_there_be_screen(deneme,gercek_thresh,gercek_temp,total_angles,total_distance);
						GPIO_Init(); 
						DELAY200();
						Timer0_Init();
						Timer_Init();
						Comparator_Init();
					//sistemi sifirlama komutlari buraya yazicak
						configure_pd7_output();
						GPIOD->DATA |= 0x80;
					}
					let_there_be_tt(gercek_thresh);
					GPIO_Init(); 
					DELAY200();
					Timer0_Init();
					Timer_Init();
					Comparator_Init();
				//sistemi sifirlama komutlari buraya yazicak
					configure_pd7_output();
					GPIOD->DATA &= ~0x80;
					GPIOF->DATA &= ~0x0E;  
					
		}

    return 0;
}

void GPIO_Init(void) {
    // Enable clock for GPIOB and F
    SYSCTL->RCGCGPIO |= (1 << 1); // Enable GPIOB clock
    while ((SYSCTL->PRGPIO & (1 << 1)) == 0) {}
			
		SYSCTL->RCGCGPIO |= (1 << 5); // Enable GPIOF clock
		while ((SYSCTL->PRGPIO & (1 << 5)) == 0) {}
			
    GPIOF->DIR |= 0x0E;  // Set PF1 (red), PF2 (blue), PF3 (green) as output
    GPIOF->DEN |= 0x0E;  // Enable digital function on PF1, PF2, PF3

    // Configure PB0-PB1 as outputs for ULN2003A (Stepper Motor Control)
    GPIOB->DIR |= 0x03;       // Set PB0-PB3  as outputs
    GPIOB->DEN |= 0x03;       // Enable digital function on PB0-PB3 
			
		GPIOB->DIR &= ~(0x40);       // PB6 input
		GPIOB->DEN |= 0x40;       // Enable digital function on PB6
			
		SYSCTL->RCGCGPIO |= 0x02; 
		__ASM("NOP");
		__ASM("NOP");
		__ASM("NOP");

	  SYSCTL->RCGCGPIO |= 0x04;        // Enable clock to PORTC and PORTD
    while ((SYSCTL->RCGCGPIO & 0x04) == 0);  // Wait for PORTC clock to stabilize

    // Unlock and configure PC4-PC5
    GPIOC->LOCK = 0x4C4F434B;        // Unlock GPIOCR register
    GPIOC->CR = 0x30;                // Allow changes to PC4-PC7
    // Configure PORTC (PC4-PC5) as outputs
    GPIOC->DIR |= 0x30;              // Set PC4-PC5 as outputs
    GPIOC->DEN |= 0x30;              // Enable digital functionality on PC4-PC5
    GPIOC->DATA &= ~0x30;            // Set all PC4-PC5 outputs to 0
		
		
		
		// for hc sr 04 below 
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

void Step_Motor(uint8_t step_no) {
    // Mask the current state of PA4-PA7 to avoid affecting other pins
    GPIOB->DATA &= ~0x03;
		GPIOC->DATA &= ~0x30;

    switch (step_no) {
        case 1:
            GPIOB->DATA |= 0x01; // PB0=1, PB1=0
						GPIOC->DATA |= 0x00; // PC0=0, PC1=0
            break;
        case 2:
            GPIOB->DATA |= 0x03; // PB0=1, PB1=1
						GPIOC->DATA |= 0x00; // PC0=0, PC1=0
            break;
        case 3:
            GPIOB->DATA |= 0x02; // PB0=0, PB1=1
						GPIOC->DATA |= 0x00; // PC0=0, PC1=0
            break;
        case 4:
            GPIOB->DATA |= 0x02; // PB0=0, PB1=1
						GPIOC->DATA |= 0x10; // PC0=1, PC1=0
            break;
        case 5:
            GPIOB->DATA |= 0x00; // PB0=0, PB1=0
						GPIOC->DATA |= 0x10; // PC0=1, PC1=0
            break;
        case 6:
            GPIOB->DATA |= 0x00; // PB0=0, PB1=0
						GPIOC->DATA |= 0x30; // PC0=1, PC1=1
            break;
        case 7:
            GPIOB->DATA |= 0x00; // PB0=0, PB1=0
						GPIOC->DATA |= 0x20; // PC0=0, PC1=1
            break;
        case 8:
            GPIOB->DATA |= 0x01; // PB0=1, PB1=0
						GPIOC->DATA |= 0x20; // PC0=0, PC1=1
            break;
        default:
            GPIOB->DATA &= ~0x03; // Turn off all outputs (PA4 to PA7)
						GPIOC->DATA &= ~ 0x30; // PA4=1, PA5=0, PA6=0, PA7=0
            break;
    }
}

void Timer0_Init(void) {
    SYSCTL->RCGCTIMER |= (1 << 0);  // Enable clock for Timer 0
    while ((SYSCTL->PRTIMER & (1 << 0)) == 0);  // Wait for Timer 0 to be ready

    SYSCTL->DSLPCLKCFG |= (1 << 1);  // Configure deep sleep clock source to 32.768 kHz

    TIMER0->CTL = 0;                // Disable Timer 0 during configuration
    TIMER0->CFG = 0x4;              // Configure for 16-bit timer
    TIMER0->TAMR = 0x12;            // Periodic mode, count down
    TIMER0->TAILR = 10000;          // Set interval (1-second timeout for 32.768 kHz clock)
    TIMER0->IMR |= (1 << 0);        // Enable timeout interrupt
    NVIC->ISER[0] |= (1 << 19);     // Enable Timer 0A interrupt (IRQ19)
    TIMER0->CTL |= (1 << 0);        // Enable Timer 0
}



// Timer 0 Interrupt Handler
void TIMER0A_Handler(void) {
    TIMER0->ICR = (1 << 0);  // Clear the timer interrupt flag
}


void Delay(void) {
    // Simple delay function, adjust the value to control speed
    volatile int i;
    for (i = 0; i < 1000; i++) {
        // Just waste time
    }
}

void configure_pd6_output(void) {
    SYSCTL->RCGCGPIO |= 0x08;        // Enable clock for GPIOD
    while ((SYSCTL->RCGCGPIO & 0x08) == 0);  // Wait for PORTC clock to stabilize

    
    GPIOD->LOCK = 0x4C4F434B;         // Unlock GPIOD
    GPIOD->CR |= 0x40;                // Allow changes to PD6 (bit 6)
    GPIOD->LOCK = 0;                  // Lock GPIOD again (optional)
    
    GPIOD->DIR |= 0x40;               // Set PD6 as output
    GPIOD->DEN |= 0x40;               // Enable PD6 as a digital pin
}
void configure_pd7_output(void) {
    SYSCTL->RCGCGPIO |= 0x08;        // Enable clock for GPIOD
    while ((SYSCTL->RCGCGPIO & 0x08) == 0);  // Wait for PORTC clock to stabilize

    
    GPIOD->LOCK = 0x4C4F434B;         // Unlock GPIOD
    GPIOD->CR |= 0x80;                // Allow changes to PD6 (bit 6)
    GPIOD->LOCK = 0;                  // Lock GPIOD again (optional)
    
    GPIOD->DIR |= 0x80;               // Set PD6 as output
    GPIOD->DEN |= 0x80;               // Enable PD6 as a digital pin
}
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

void enter_deepSleep(void) {
		Comparator_Init();
    while (1) {
			
    SCB->SCR |= 0x04;                // Set Sleep-Deep bit
    __asm(" WFI");                   // Enter deep sleep, wait for interrupt
		
    if (COMP->ACSTAT0 == 0x02) { // this is used since the adjusting of the trimpot creates fluctuations in the threshhold voltage which creates problems
				// the below is for the fact that sometimes there is weird interrupts eventhough we didnot crossed the treshhold it is used for re entering sleep
				COMP0_Handler();  // Clear interrupt status also we want no more interrupts from comparators anymore for now
        break;  // threshold crossed it is checked, exit deep sleep loop 
            }
		else{}
		
        }
		NVIC->ICER[1] |= (1 << (41 - 32)); // clear interrupt enable for comparason
}

void COMP0_Handler(void) {
    COMP->ACRIS = 0x01;  // Clear interrupt status for Comparator 0
}



