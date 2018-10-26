#include <xc.h>
#include <p33EP64GS504.h>

void PWMInit( void ){
    
    // set up the clock 
    ACLKCONbits.FRCSEL = 1;  /* FRC is input to Auxiliary PLL */
    ACLKCONbits.SELACLK = 1; /* Auxiliary Oscillator provides the clock source*/
    ACLKCONbits.APSTSCLR = 7; /* Divide Auxiliary clock by 1 */
    ACLKCONbits.ENAPLL = 1;  /* DISABLE Auxiliary PLL */
    /* Wait for Auxiliary PLL to Lock while(ACLKCONbits.APLLCK! = 1) */
    while(ACLKCONbits.APLLCK == 0); 
        
    PWMCON1bits.CAM = 0; /* For Edge-Aligned Mode */

    /* Select PWM time base input clock prescaler */
    /* Choose divide ratio of 1:2, which affects all PWM timing operations */
//    PTCON2bits.PCLKDIV = 1;
    
    PWMCON1bits.ITB = 0;    /* PTPER provides the PWM time period value */
    
    /* Choose PWM time period based on FRC input clock */
    /* PWM frequency is 100 kHz */
    /* Choose one of the following options */
//     PTPER = ACLK*8*T_switching - 8;
    PTPER = 9579; /* When PWMCONx<9> = 0 */
    
    /* Select either Master Duty cycle or Independent Duty cycle */
    PWMCON1bits.MDCS = 0;   /* PDCX/SDCX provides duty cycle value */
    
    PDC1 = 0;               /* Set Independent Primary Duty Cycle */
    
    PWMCON1bits.DTC = 2;    /* Dead-time function is disabled */
            
    PHASE1 = 0;     /* Primary shift register */
    
    PWMCON1bits.TRGIEN = 0; /* disable trigger interrupt */
    
    IOCON1bits.PMOD = 0; /* For Complementary Output mode */
    
    /* To enable writing of FCLCON1 register */
    // replace the asm lines with below line once PWMLOCK changed in fuses
    FCLCON1bits.FLTMOD = 3; // fault mode set as fault input disabled
//    asm volatile ("push.s"); /* Context save w0-w3 */
//    asm volatile ("mov #0xABCD, w0");
//    asm volatile ("mov #0x4321, w1");
//    asm volatile ("mov #0x0003, w2");
//    asm volatile ("mov w0, PWMKEY"); /* Perform Unlock Sequence */
//    asm volatile ("mov w1, PWMKEY");
//    asm volatile ("mov w2, FCLCON1"); /* Write FCLCONx register */
//    asm volatile ("pop.s"); /* Restore context for w0-w3*/
    
    /* PWM Output pin control assigned to PWM generator */
    IOCON1bits.PENH = 1;
    IOCON1bits.PENL = 1;
    
    /* High and Low switches set to active-high state */
    IOCON1bits.POLH = 0;
    IOCON1bits.POLL = 0;
    
    PTCONbits.PTEN = 1; /* enable PWM module */
}