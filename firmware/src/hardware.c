/**
 * @file  hardware.c
 * @brief Low-level hardware configuration
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

static inline void hw_init_button(void);
static inline void hw_init_clock(void);
static inline void hw_init_display(void);
static inline void hw_init_leds(void);
static inline void hw_init_uart(void);

/**
 * @brief Called on startup to init processor, clocks and some peripherals
 *
 */
void hw_init(void)
{
	/* Use PM to configure clock sources */
	reg8_wr(PM_ADDR + 0x08, 0x00); /* CPU  clock select (CPUSEL)  */
	reg8_wr(PM_ADDR + 0x09, 0x00); /* APBA clock select (APBASEL) */
	reg8_wr(PM_ADDR + 0x0A, 0x00); /* APBB clock select (APBBSEL) */
	reg8_wr(PM_ADDR + 0x0B, 0x00); /* APBC clock select (APBCSEL) */

	hw_init_clock();

	hw_init_button();
	hw_init_display();
	hw_init_leds();
	hw_init_uart();
}

/**
 * @brief Initialize io for pushbutton
 *
 */
static inline void hw_init_button(void)
{
	/* Configure SW1 (PA27) */
	reg_wr (0x60000000 + 0x04, (1 << 27)); // DIR
	reg_wr (0x60000000 + 0x18, (1 << 27)); // Set out=1 for pull-up
	reg8_wr(0x60000000 + 0x5B,  0x06);     // PINCFG: Input with pull-up
	reg_set(0x60000000 + 0x24, (1 << 27)); // Continuous sampling

	/* Configure SW2 (PA11) */
	reg_wr (0x60000000 + 0x04, (1 << 11)); // DIR
	reg_wr (0x60000000 + 0x18, (1 << 11)); // Set out=1 for pull-up
	reg8_wr(0x60000000 + 0x4B,  0x06);     // PINCFG: Input with pull-up
	reg_set(0x60000000 + 0x24, (1 << 11)); // Continuous sampling

	/* Configure SW3 (PA14) */
	reg_wr (0x60000000 + 0x04, (1 << 14)); // DIR
	reg_wr (0x60000000 + 0x18, (1 << 14)); // Set out=1 for pull-up
	reg8_wr(0x60000000 + 0x4E,  0x06);     // PINCFG: Input with pull-up
	reg_set(0x60000000 + 0x24, (1 << 14)); // Continuous sampling

	/* Configure SW4 (PA10) */
	reg_wr (0x60000000 + 0x04, (1 << 10)); // DIR
	reg_wr (0x60000000 + 0x18, (1 << 10)); // Set out=1 for pull-up
	reg8_wr(0x60000000 + 0x4A,  0x06);     // PINCFG: Input with pull-up
	reg_set(0x60000000 + 0x24, (1 << 10)); // Continuous sampling

	/* Configure SW5 (PA15) */
	reg_wr (0x60000000 + 0x04, (1 << 15)); // DIR
	reg_wr (0x60000000 + 0x18, (1 << 15)); // Set out=1 for pull-up
	reg8_wr(0x60000000 + 0x4F,  0x06);     // PINCFG: Input with pull-up
	reg_set(0x60000000 + 0x24, (1 << 15)); // Continuous sampling
}

/**
 * @brief Configure oscillators and Generic Clock Controller (GCLK)
 *
 */
static inline void hw_init_clock(void)
{
	u32 dfll_coarse, dfll_cfg;
	u32 v;

	/* Configure internal 8MHz oscillator */
	v = reg_rd(SYSCTRL_ADDR + 0x20); /* Read OSC8M config register */
	v &= 0xFFFFFC3F;                 /* Clear prescaler and OnDemand flag */
	reg_wr(SYSCTRL_ADDR + 0x20, v);  /* Write-back OSC8M */
	/* Wait for internal 8MHz oscillator stable and ready */
	while( ! (reg_rd(SYSCTRL_ADDR + 0x0C) & 0x08))
		;

	/* Activate the internal 32kHz oscillator */
	v  = (((reg_rd(0x00806024) >> 6) & 0x7F) << 16); /* Calib bits 38:44 */
	v |= (1 << 1); /* Set enable bit */
	v |= (1 << 2); /* Output Enable */
	reg_wr(SYSCTRL_ADDR + 0x18, v);
	/* Wait for internal 32kHz oscillator stable and ready */
	while( ! (reg_rd(SYSCTRL_ADDR + 0x0C) & 0x04))
		;

	/* Set Divisor for GCLK0 : enabled, OSC8M, no divisor */
	reg_wr(GCLK_ADDR + 0x08, (1 << 8) | 0x00);
	reg_wr(GCLK_ADDR + 0x04, (1 << 16) | (0x06 << 8) | 0x00);
	/* Set Divisor for GCLK1 : enabled, OSC8M, no divisor */
	reg_wr(GCLK_ADDR + 0x08, (1 << 8) | 0x01);
	reg_wr(GCLK_ADDR + 0x04, (1 << 16) | (0x06 << 8) | 0x01);
	/* Set Divisor for GCLK2 : disabled */
	reg_wr(GCLK_ADDR + 0x08, (1 << 8) | 0x02);
	reg_wr(GCLK_ADDR + 0x04, (0 << 16) | (0x06 << 8) | 0x02);
	/* Set Divisor for GCLK3 : disabled */
	reg_wr(GCLK_ADDR + 0x08, (1 << 8) | 0x03);
	reg_wr(GCLK_ADDR + 0x04, (0 << 16) | (0x06 << 8) | 0x03);
	/* Set Divisor for GCLK4 : disabled */
	reg_wr(GCLK_ADDR + 0x08, (1 << 8) | 0x04);
	reg_wr(GCLK_ADDR + 0x04, (0 << 16) | (0x06 << 8) | 0x04);
	/* Set Divisor for GCLK5 : enabled, OSC32k, no divisor */
	reg_wr(GCLK_ADDR + 0x08, (1 << 8) | 0x05);
	reg_wr(GCLK_ADDR + 0x04, (1 << 16) | (0x04 << 8) | 0x05);
	/* Set Divisor for GCLK6 : disabled */
	reg_wr(GCLK_ADDR + 0x08, (1 << 8) | 0x06);
	reg_wr(GCLK_ADDR + 0x04, (0 << 16) | (0x06 << 8) | 0x06);
	/* Set Divisor for GCLK7 : enabled, DFLL48M, no divisor */
	reg_wr(GCLK_ADDR + 0x08, (1 << 8) | 0x07);
	reg_wr(GCLK_ADDR + 0x04, (1 << 16) | (0x07 << 8) | 0x07);
	/* Set Divisor for GCLK8 : disabled */
	reg_wr(GCLK_ADDR + 0x08, (1 << 8) | 0x08);
	reg_wr(GCLK_ADDR + 0x04, (0 << 16) | (0x06 << 8) | 0x08);

	/* Enable DFLL block */
	reg16_wr(SYSCTRL_ADDR + 0x24, (1 << 1));
	while ( ! (reg_rd(SYSCTRL_ADDR + 0x0C) & 0x00000010))
		;
	/* Configure DFLL multiplier (DFLLMUL) */
	reg_wr(SYSCTRL_ADDR + 0x2c, (1 << 20) | (1 << 16) | 0xBB80);
	/* Set DFLL coarse calibration value */
	dfll_coarse = (*(u32 *)0x00806024 >> 26) & 0x3F;
	reg_wr(SYSCTRL_ADDR + 0x28, (0x0000 << 16) | (dfll_coarse << 10) | 512);
	/* Configure DFLL */
	dfll_cfg = (1 << 10)  /* Bypass Coarse Lock      */
	         | (1 <<  9)  /* Quick Lock Disable      */
	         | (1 <<  5)  /* USB Clock Recovery Mode */
	         | (1 <<  2)  /* Mode: closed-loop       */
	         | (1 <<  1); /* Enable DFLL             */
	reg16_wr(SYSCTRL_ADDR + 0x24, dfll_cfg);
	/* Wait depending on DFLL mode (closed or open loop) */
	if (reg_rd(SYSCTRL_ADDR + 0x24) & 0x04)
	{
		u32 wait_mask = 0x10;
		/* Wait for DFLL to be ready */
		while((reg_rd(SYSCTRL_ADDR + 0X0C) & wait_mask) != wait_mask)
			;
	}
	else
	{
		/* Wait for DFLL to be ready */
		while((reg_rd(SYSCTRL_ADDR + 0X0C) & 0x10) == 0)
			;
	}
	/* Wait end of clock domains synchronization */
	while (reg8_rd(GCLK_ADDR + 0x01) & 0x80)
		;

	/* Update NVM (flash memory wait-state before switching to 48MHz) */
	reg_wr(NVM_ADDR + 0x04, (1 << 1)); /* 1 wait state (see table 37-40) */

	/* Set Divisor for GCLK0 : enabled, DFLL48M, no divisor */
	reg_wr(GCLK_ADDR + 0x08, (1 << 8) | 0x00);
	reg_wr(GCLK_ADDR + 0x04, (1 << 16) | (0x07 << 8) | 0x00);
}

/**
 * @brief Initialize IOs used to control display
 *
 */
static inline void hw_init_display(void)
{
	/* Configure DISP_RST (PA03) */
	reg_wr(0x60000000 + 0x14, (1 << 03)); // OUT=0 (active reset !)
	reg_wr(0x60000000 + 0x08, (1 << 03)); // DIR: output
	reg8_wr(0x60000000 + 0x43, 0x00); // PINCFG: normal, no-pull, no pmux

	/* Configure DISP_DC (PA02) */
	reg_wr(0x60000000 + 0x14, (1 << 02)); // OUT=0
	reg_wr(0x60000000 + 0x08, (1 << 02)); // DIR: output
	reg8_wr(0x60000000 + 0x42, 0x00); // PINCFG: normal, no-pull, no pmux

	/* Configure DISP_RW (PA01) */
	reg_wr(0x60000000 + 0x14, (1 << 01)); // OUT=0
	reg_wr(0x60000000 + 0x08, (1 << 01)); // DIR: output
	reg8_wr(0x60000000 + 0x41, 0x00); // PINCFG: normal, no-pull, no pmux

	/* Configure DISP_ERD (PA00) */
	reg_wr(0x60000000 + 0x14, (1 << 00)); // OUT=0
	reg_wr(0x60000000 + 0x08, (1 << 00)); // DIR: output
	reg8_wr(0x60000000 + 0x40, 0x00); // PINCFG: normal, no-pull, no pmux

	/* Configure DISP_NSS (PA06) */
	reg_wr(0x60000000 + 0x18, (1 << 06)); // OUT=1
	reg_wr(0x60000000 + 0x08, (1 << 06)); // DIR: output
	reg8_wr(0x60000000 + 0x46, 0x00); // PINCFG: normal, no-pull, no pmux

	/* Configure SPI lines (PA04, PA05, PA06, PA07) */
	reg8_wr(0x60000000 + 0x44, 0x01); // PINCFG: Set PMUX for PA04 (MOSI)
	reg8_wr(0x60000000 + 0x45, 0x01); // PINCFG: Set PMUX for PA05 (SCK)
	reg8_wr(0x60000000 + 0x46, 0x00); // PINCFG: No-PMUX  for PA06 (NSS)
	reg8_wr(0x60000000 + 0x47, 0x01); // PINCFG: Set PMUX for PA07 (MISO)
	reg8_wr(0x60000000 + 0x32, (0x03 << 4) | (0x03 << 0)); // PMUX: D
	reg8_wr(0x60000000 + 0x33, (0x03 << 4) | (0x03 << 0)); // PMUX: D
}

/**
 * @brief Initialize the LED io
 *
 */
static inline void hw_init_leds(void)
{
	/* Set LED "OFF" (pin output=1) */
	reg_wr(0x60000000 + 0x18, (1 << 28));
	/* DIR: Set PA28 as output */
	reg_wr(0x60000000 + 0x08, (1 << 28));
	/* PINCFG: Configure PA28 (strong strength, no pull, no pmux) */
	reg8_wr(0x60000000 + 0x5C, 0x40);
}

/**
 * @brief Initialize UART ios
 *
 */
static inline void hw_init_uart(void)
{
	/* Configure main UART (SERCOM3) */
	reg8_wr(0x60000000 + 0x56, 0x01);    // PINCFG: Set PMUX for PA22 (TX)
	reg8_wr(0x60000000 + 0x57, 0x01);    // PINCFG: Set PMUX for PA23 (RX)
	reg8_wr(0x60000000 + 0x3B, (0x02 << 4) | (0x02 << 0)); // PMUX: C

	/* Configure console/debug port (SERCOM2) */
	reg8_wr(0x60000000 + 0x48, 0x01);    // PINCFG: Set PMUX for PA08 (TX)
	reg8_wr(0x60000000 + 0x49, 0x01);    // PINCFG: Set PMUX for PA09 (RX)
	reg8_wr(0x60000000 + 0x34, (0x03 << 4) | (0x03 << 0)); // PMUX: D
}
/* EOF */
