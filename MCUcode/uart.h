/*
 * File:   uart.h
 * Author: Liam
 *
 * Created on September 8, 2018, 7:05 PM
 */

#ifndef UART_H
#define	UART_H

#include <stdint.h>
#include <string.h>
#include <xc.h> // include processor files - each processor file is guarded.

#include "define.h"

void UARTInit(const uint32_t baud_rate, const uint8_t BRGH);
void UARTSendChar(const char c);
void UARTSendString(const char *str);
uint8_t UARTReadChar(void);

void putch(unsigned char);

#endif	/* UART_H */
