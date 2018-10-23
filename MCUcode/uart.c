/*
 * File:   uart.c
 * Author: Liam
 *
 * Created on September 8, 2018, 7:05 PM
 */
#include "uart.h"

void UARTInit(const uint32_t baud_rate, const uint8_t BRGH) {
    /* Configure RX and TX pins */
    RPOR1bits.RP34R = 1; // RP34 (RB2) tied to UART1 Transmit
    ANSELBbits.ANSB0 = 0; // Configure RB0 analog pin as digital
    RPINR18bits.U1RXR = 32; // RP32 (RB0) tied to UART1 Receive
    TRISBbits.TRISB0 = 1; // Set RX pin as input
    
    /* Default UART mode */
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8-Data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud disabled

    if (BRGH == 0) {
        U1BRG = (uint16_t) (FCY/(16*baud_rate) - 0.5);
        U1MODEbits.BRGH = 0;
        
    } else {
        U1BRG = (uint16_t) (FCY/(4*baud_rate) - 0.5);
        U1MODEbits.BRGH = 1;
    }

    U1STAbits.UTXISEL0 = 0; // Interrupt after one TX character is transmitted
    U1STAbits.UTXISEL1 = 0;

    U1STAbits.URXISEL0 = 0; // Interrupt after one RX character is received
    
    IEC0bits.U1TXIE = 1; // Enable UART TX interrupt
    IEC0bits.U1RXIE = 1; // Enable UART RX interrupt

    U1MODEbits.UARTEN = 1; // Enable UART
    U1STAbits.UTXEN = 1; // Enable UART TX
}

void UARTSendChar(const char c) {
    while (!U1STAbits.TRMT); // wait until buffer is empty
    U1TXREG = c; // send the new byte
}

void UARTSendString(const char *str) {
    int i;
    for (i=0; i<strlen(str); i++) {
        UARTSendChar(str[i]);
    }
}

// printf() helper function
void putch(unsigned char c) {
    // wait until the end of tranmission
    while (!U1STAbits.TRMT);
    U1TXREG = c; // send the new byte
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0; // Clear TX Interrupt flag
    //U1TXREG = 'a';       // Transmit one character
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    U1TXREG = U1RXREG;
    if (U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;
    }
    IFS0bits.U1RXIF = 0;
}
