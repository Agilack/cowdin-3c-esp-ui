/**
 * @file  uart.h
 * @brief Definitions and prototypes for UART functions
 *
 * @author Saint-Genest Gwenael <gwen@cowlab.fr>
 * @copyright Agilack (c) 2022
 *
 * @page License
 * Cowdin-3C-ui firmware is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 3 as published by the Free Software Foundation. You should
 * have received a copy of the GNU Lesser General Public License along
 * with this program, see LICENSE.md file for more details.
 * This program is distributed WITHOUT ANY WARRANTY.
 */
#ifndef UART_H
#define UART_H
#include "hardware.h"
#include "types.h"

#define UART_DBG SERCOM0_ADDR
#define UART_SYS SERCOM3_ADDR

void uart_crlf(void);
void uart_dump(u8 *d, int l);
void uart_init(void);
void uart_putc(unsigned char c);
void uart_puts(char *s);
void uart_puthex  (const u32 c);
void uart_puthex8 (const u8  c);
void uart_puthex16(const u16 c);

#endif
/* EOF */
