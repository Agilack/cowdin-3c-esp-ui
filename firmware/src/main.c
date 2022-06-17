/**
 * @file  main.c
 * @brief Entry point of the C code and main firmware loop
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
#include "hardware.h"
#include "uart.h"

/**
 * @brief Entry point of the C code
 *
 * This function is called by the reset handler when the low-level init of
 * the memory has been made (mainly stack init). This function should never
 * returns.
 */
int main(void)
{
	int i;

	/* Initialize low-level hardware access */
	hw_init();
	/* Initialize peripherals */
	uart_init();

	uart_puts("\r\n--=={ CowDIN UI }==--  ");

	// Dummy "blink led" loop
	while(1)
	{
		reg_wr(0x60000000 + 0x14, (1 << 28));
		for (i = 0; i < 0x40000; i++)
			asm volatile("nop");
		reg_wr(0x60000000 + 0x18, (1 << 28));
		for (i = 0; i < 0x40000; i++)
			asm volatile("nop");
	}
}
/* EOF */
