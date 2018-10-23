#include "timer.h"
#include "define.h"
#include "controller.h"

#include <stdbool.h>

extern float d;
extern bool  require_update;

extern void LEDtoggle();

void timerINT( void ){
    // Timer2 allows for ADC trigger
    T2CONbits.TON = 0;      // Disable timer
    T2CONbits.TCS = 0;      // Select internal introduction cycle clock
    T2CONbits.TGATE = 0;    // Disable Gated Timer mode
    T2CONbits.TCKPS = 0b00; // Select 1:1 prescaler
    TMR2 = 0x00;            // Clear timer register
//    PR2 = 1200;             // Load the period value 50kHz
    PR2 = 1200*2.5;         // 10 kHz
//    PR2 = 1200*2.5*4;         // 5kHZ
//    PR2 = 1200*2.5*10;      // 1kHz
    
    IPC1bits.T2IP = 0x01;   // Set Timer 2 interrupt priority level
    IFS0bits.T2IF = 0;      // Clear Timer 2 interrupt flag
    IEC0bits.T2IE = 0;      // Enable Timer 2 interrupt ****SET BACK TO 1***
    
    T2CONbits.TON = 1;      // Start Timer
}


void __attribute__((__interrupt__, no_auto_psv)) _T2Interrupt(void){
    /**************************************************************************/
    LATBbits.LATB4 = !LATBbits.LATB4;
    PDC1 = (int) ACLK*8*d*TPWM;     // ** REDEFINE CONSTANT HERE
    PDC1 = DUTY_CONST * d;       
    require_update = true;
	/**************************************************************************/
    IFS0bits.T2IF = 0;      // clear flag
}