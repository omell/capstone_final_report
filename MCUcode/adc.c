/*
 * File:   init.c
 * Author: Liam
 *
 * Created on August 29, 2018, 1:07 AM
 */

#include "adc.h"

void InitADC(void) {
    // Configure the I/O pins to be used as analog inputs.
    //ANSELAbits.ANSA0 = 1; TRISAbits.TRISA0 = 1; // AN0/RA0
    //ANSELAbits.ANSA1 = 1; TRISAbits.TRISA1 = 1; // AN1/RA1
    ANSELCbits.ANSC5 = 1; TRISCbits.TRISC5 = 1; // AN0ALT/RC5
    ANSELCbits.ANSC4 = 1; TRISCbits.TRISC4 = 1; // AN1ALT/RC4
    
    // Setup ADC Clock
    ADCON3Hbits.CLKSEL = 2;  // Fast RC Oscillator (8MHz)
    ADCON3Hbits.CLKDIV = 0;  // Global clock divider (1:1)
    ADCORE0Hbits.ADCS  = 0;  // Core 0 clock divider (1:2)
    ADCORE1Hbits.ADCS  = 0;  // Core 0 clock divider (1:2)
    
    // ADC Cores in 12-bit resolution mode
    ADCORE0Hbits.RES   = 2;  
    ADCORE1Hbits.RES   = 2;
    
    ADCON1Hbits.FORM   = 1;  // FRACTIONAL format (1:frac 0: int)  
    ADCON3Lbits.REFSEL = 0;  // AVdd as voltage reference
    
    // Select single-ended input configuration and unsigned output format.
    ADMOD0Lbits.SIGN0  = 0;  // AN0/RA0
    ADMOD0Lbits.DIFF0  = 0;  // AN0/RA0
    ADMOD0Lbits.SIGN1  = 0;  // AN1/RA1
    ADMOD0Lbits.DIFF1  = 0;  // AN1/RA1
     
    // This might be redundant.
    ADCON4Hbits.C0CHS  = 3;  // Core 0 source AN0ALT
    ADCON4Hbits.C1CHS  = 3;  // Core 1 source AN1ALT
    
    // Enable and calibrate the module.
    InitCalibrateADC();
    
    // Configure and enable ADC interrupts
    ADIELbits.IE0      = 1;  // Enable AN0 interrupt
    ADIELbits.IE1      = 1;  // Enable AN1 interrupt
    
    // Set Timer 2 as interrupt source
    ADTRIG0Lbits.TRGSRC0 = 13;
    ADTRIG0Lbits.TRGSRC1 = 13;
    
    // Clear and Enable the interrupt flags
    IFS6bits.ADCAN0IF = 0;
    IEC6bits.ADCAN0IE = 1;
    IFS6bits.ADCAN1IF = 0;
    IEC6bits.ADCAN1IE = 1;
}

void InitCalibrateADC(void) {
    // Set initialization time to maximum
    ADCON5Hbits.WARMTIME = 15;
    // Turn on ADC module
    ADCON1Lbits.ADON = 1;
    
    // Turn on analog power for dedicated core 0
    ADCON5Lbits.C0PWR = 1;
    while(ADCON5Lbits.C0RDY == 0);
    ADCON3Hbits.C0EN  = 1;   // Enable ADC core 0
    
    // Turn on analog power for dedicated core 1
    ADCON5Lbits.C1PWR = 1;
    while(ADCON5Lbits.C1RDY == 0);
    ADCON3Hbits.C1EN  = 1;   // Enable ADC core 1
    
    // Enable calibration for the dedicated core 0
    ADCAL0Lbits.CAL0EN   = 1;
    ADCAL0Lbits.CAL0DIFF = 0;         // Single-ended input calibration
    ADCAL0Lbits.CAL0RUN  = 1;         // Start Cal
    while(ADCAL0Lbits.CAL0RDY == 0);
    ADCAL0Lbits.CAL0EN   = 0;         // Cal complete

    // Enable calibration for the dedicated core 1
    ADCAL0Lbits.CAL1EN   = 1;
    ADCAL0Lbits.CAL1DIFF = 0;         // Single-ended input calibration
    ADCAL0Lbits.CAL1RUN  = 1;         // Start Cal
    while(ADCAL0Lbits.CAL1RDY == 0);
    ADCAL0Lbits.CAL1EN   = 0;         // Cal complete
}