/*
 * File:   main.c
 * Author: Liam
 *
 * Created on August 29, 2018, 1:06 AM
 */

#include <stdint.h>
#include <stdio.h>
#include <xc.h>
#include <dsp.h>
#include <stdbool.h>

#include "main.h"
#include "define.h"
#include "adc.h"
#include "uart.h"
#include "controller.h"
#include "timer.h"
#include "pwm.h"


#include <libpic30.h> // FCY must be defined before including libpic30.h

volatile uint16_t dataAN0;
volatile uint16_t dataAN1;
uint16_t reference;
extern float myFloat2Fract(fractional in);

bool require_update = false; 

void ClockInit(void) {
    // Configure PLL prescaler, PLL postscaler, PLL divisor
    PLLFBD=63; // M=65
    CLKDIVbits.PLLPOST=0; // N2=2
    CLKDIVbits.PLLPRE=0; // N1=2
    
    // Initiate Clock Switch to FRC oscillator with PLL (NOSC=0b001)
    __builtin_write_OSCCONH(0x01);
    __builtin_write_OSCCONL(OSCCON | 0x01);

    // Wait for Clock switch to occur
    while (OSCCONbits.COSC!= 0b001);
    
    // Wait for PLL to lock
    while (OSCCONbits.LOCK!= 1);
}

int main(void) {
    
    ClockInit();
    InitADC();
    PWMInit( );
    timerINT( );
    
    UARTInit(9600, 0);
    __delay_ms(100); // Wait for UART module to stabilize
   
    controllerInit();
    
    // Enable LED
    TRISBbits.TRISB4 = OUTPUT;
    LATBbits.LATB4 = HIGH;

//    PDC1 = 4794;
    PDC1 = (int)4794;
 
    while(1) {        
        if(require_update){
            require_update = false;
            while ((ADSTATLbits.AN0RDY && ADSTATLbits.AN1RDY));       
            updateControlSignal();
              updateIntegral();                
        }
    }
    return 1;
}
 
// ADC AN0 ISR
void __attribute__((interrupt, no_auto_psv)) _ADCAN0Interrupt(void) {
    dataAN0 = ADCBUF0; // read conversion result
    IFS6bits.ADCAN0IF = 0; // clear interrupt flag
}

// ADC AN1 ISR
void __attribute__((interrupt, no_auto_psv)) _ADCAN1Interrupt(void) {
    dataAN1 = ADCBUF1; // read conversion result
    IFS6bits.ADCAN1IF = 0; // clear interrupt flag
}
     

