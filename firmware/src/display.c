/**
 * @file  display.c
 * @brief Driver for the display (using sercom/spi)
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
#include "display.h"
#include "display_font.h"
#include "types.h"
#include "uart.h"

#define SPI_DISP SERCOM0_ADDR

static void disp_cmd(u8 *cmd, int len);
static void disp_dc(uint mode);
static void spi_init(void);

static void spi_cs(uint state);
static void spi_wait(void);
static void spi_wr(unsigned char v);

/**
 * @brief Initialize display module
 *
 */
void disp_init(void)
{
	int i;

	spi_init();

	reg_wr(0x60000000 + 0x18, (1 << 03));
	for (i = 0; i < 10000; i++)
		asm volatile("nop");

	disp_cmd((u8 *)"\xAE", 1);     // Set Display Off
	disp_cmd((u8 *)"\xD5\x80", 2); // Set Clock
	disp_cmd((u8 *)"\xA8\x3F", 2); // Set Multiplex Ratio
	disp_cmd((u8 *)"\xD3\x00", 2); // Set Display Offset
	disp_cmd((u8 *)"\x40", 1);     // Set Display Start Line
	disp_cmd((u8 *)"\x8D\x14", 2); // Configure Charge Pump
	disp_cmd((u8 *)"\xA0", 1);     // Set Segment Remap
	disp_cmd((u8 *)"\xC0", 1);     // Set COM output scan direction
	disp_cmd((u8 *)"\xDA\22", 2);  // COM pins hardware configuration
	disp_cmd((u8 *)"\x81\xCF", 2); // Set Contrast Control
	disp_cmd((u8 *)"\xD9\xF1", 2); // Set precharge period
	disp_cmd((u8 *)"\xDB\x40", 2); // Set VCOMH deselect level
	disp_cmd((u8 *)"\xA4", 1);     // Set Entire Display On/Off

	disp_clear(0xFF);
	disp_cmd((u8 *)"\x20\x02", 2); // Set Adressing Mode : Page
	disp_cmd((u8 *)"\xAF", 1);     // Set Display On
}

/**
 * @brief Clear one or multiple lines of display
 *
 * @param lines Bitmask of the lines to clear (0xFF to clear all)
 */
void disp_clear(unsigned char lines)
{
	int p, c;

	for (p = 0; p < 8; p++)
	{
		if ((lines & (1 << p)) == 0)
			continue;

		// Select next page (line)
		disp_pos(0, p);
		// Clear content of one line (each column)
		disp_dc(DISP_MODE_DATA);
		spi_cs(1);
		for (c = 0; c < 128; c++)
			spi_wr(0x00);
		spi_wait();
		spi_cs(0);
	}
}

/**
 * @brief Set the current address into display RAM
 *
 * @param x Specify the horitontal position (multiplied by 8)
 * @param y Specify the current page
 */
void disp_pos(uint x, uint y)
{
	u8 cmd[3];

	// Set Adressing Mode : Page Adressing
	disp_cmd((u8 *)"\x20\x02", 2);
	// Set current page : 0xB0 + y
	cmd[0] = 0xB0 + y;
	disp_cmd((u8 *)cmd, 1);
	// Set lower column start address
	cmd[0] = 0x21;     /* Command */
	cmd[1] = (x << 3); /* Start   */
	cmd[2] = 0x7F;     /* End     */
	disp_cmd((u8 *)cmd, 3);
}

/**
 * @brief Draw a character at current position
 *
 * @param c Character do display (to draw)
 */
void disp_putc(char c)
{
	int index;
	int i;

	if (c & 0x80)
		return;

	disp_dc(DISP_MODE_DATA);
	index = (c - 0x20);
	spi_cs(1);
	for (i = 0; i < 8; i++)
		spi_wr( font[index][i] );
	spi_wait();
	spi_cs(0);
}

/**
 * @brief Display a text string to display at current position
 *
 * @param s Pointer to the nul terminated text-string to display
 */
void disp_puts(char *s)
{
	while(*s)
	{
		disp_putc(*s++);
	}
}

/**
 * @brief Test function, used to draw patterns to display
 *
 * @param type Identifier of the test pattern
 */
void disp_test(int type)
{
	int i;

	// Pattern test
	if (type == 0)
	{
		spi_cs(1);
		for (i = 0; i < 128; i++)
			spi_wr(i);
		spi_wait();
		spi_cs(0);
	}
}

/* -------------------------------------------------------------------------- */
/* --                       Private display function                       -- */
/* -------------------------------------------------------------------------- */

/**
 * @brief Send a command to the display
 *
 * @param cmd Pointer to an array with all bytes of the command
 * @param len NUmber of bytes to send
 */
static void disp_cmd(u8 *cmd, int len)
{
	// Set D/C pin to "command" mode
	disp_dc(DISP_MODE_CMD);

	// Send specified number of bytes
	spi_cs(1);
	while(len)
	{
		spi_wr(*cmd);
		cmd++;
		len--;
	}
	spi_wait();
	spi_cs(0);
}

/**
 * @brief Set the state of the Command/Data pin of the display
 *
 * @param mode New mode to set to the pin
 */
static void disp_dc(uint mode)
{
	if (mode == DISP_MODE_CMD)
		reg_wr(PORT_ADDR + 0x14, (1 << 2)); // D/C = 0
	else
		reg_wr(PORT_ADDR + 0x18, (1 << 2)); // D/C = 1
}

/* -------------------------------------------------------------------------- */
/* --                            SPI  functions                            -- */
/* -------------------------------------------------------------------------- */

/**
 * @brief Change state of Chip-Select signal
 *
 * @param state Positive value to activate CS (set to 0), Zero to desactivate
 */
static void spi_cs(uint state)
{
	if (state)
		reg_wr(PORT_ADDR + 0x14, (1 << 6)); // Set CS=0 to "start"
	else
		reg_wr(PORT_ADDR + 0x18, (1 << 6)); // Set CS=1 to "stop"
}

/**
 * @brief Initialize the SPI (sercom) port connected to display
 *
 */
static void spi_init(void)
{
	// Enable SERCOM0 clock (APBCMASK)
	reg_set(PM_ADDR + 0x20, (1 << 2));
	// Set GCLK for SERCOM0 (generic clock generator 1)
	reg16_wr (GCLK_ADDR + 0x02, (1 << 14) | (1 << 8) | 0x14);

	// Reset SPI (set SWRST)
	reg_wr((SPI_DISP + 0x00), 0x01);
	// Wait end of software reset
	while( reg_rd(SPI_DISP + 0x00) & 0x01)
		;

	// Configure SPI
	reg_wr(SPI_DISP + 0x00, (0 << 28) | // CPOL=0 CPHA=0
	                        (3 << 20) | // DIPO: Use PAD3 for MISO
	                        (3 << 2));  // SPI host
	// Set RXEN
	reg_wr(SPI_DISP + 0x04, (1 << 17));
	// Configure Baudrate (Max freq for display: 10MHz)
	reg8_wr(SPI_DISP + 0x0C, 8);

	// Set ENABLE into CTRLA
	reg_set(SPI_DISP + 0x00, (1 << 1));
}

/**
 * @brief Wait the end of current transfer (mainly for CS synchronization)
 *
 */
static void spi_wait(void)
{
	// Wait for Transmit Complete flag
	while((reg8_rd(SPI_DISP + 0x18) & 2) == 0)
		;
}

/**
 * @brief Push one single byte to the SPI tx fifo
 *
 * @param v Value to send
 */
static void spi_wr(unsigned char v)
{
	// Wait Data Register Empty
	while((reg8_rd(SPI_DISP + 0x18) & 1) == 0)
		;
	// Push data byte to send
	reg16_wr(SPI_DISP + 0x28, v);
}
/* EOF */
