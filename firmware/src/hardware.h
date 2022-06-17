/**
 * @file  hardware.h
 * @brief Headers and definitions for low-level hardware configuration
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
#ifndef HARDWARE_H
#define HARDWARE_H
#include "types.h"

/* AHB-APB Bridge A */
#define PAC0_ADDR    ((u32)0x40000000)
#define PM_ADDR      ((u32)0x40000400)
#define SYSCTRL_ADDR ((u32)0x40000800)
#define GCLK_ADDR    ((u32)0x40000C00)
#define WDT_ADDR     ((u32)0x40001000)
#define RTC_ADDR     ((u32)0x40001400)
#define EIC_ADDR     ((u32)0x40001800)
/* AHB-APB Bridge B */
#define PAC1_ADDR    ((u32)0x41000000)
#define DSU_ADDR     ((u32)0x41002000)
#define NVM_ADDR     ((u32)0x41004000)
#define PORT_ADDR    ((u32)0x41004400)
#define DMAC_ADDR    ((u32)0x41004800)
#define USB_ADDR     ((u32)0x41005000)
#define MTB_ADDR     ((u32)0x41006000)
/* AHB-APB Bridge C */
#define PAC2_ADDR    ((u32)0x42000000)
#define EVSYS_ADDR   ((u32)0x42000400)
#define SERCOM0_ADDR ((u32)0x42000800)
#define SERCOM1_ADDR ((u32)0x42000C00)
#define SERCOM2_ADDR ((u32)0x42001000)
#define SERCOM3_ADDR ((u32)0x42001400)
#define SERCOM4_ADDR ((u32)0x42001800)
#define SERCOM5_ADDR ((u32)0x42001C00)
#define TCC0_ADDR    ((u32)0x42002000)
#define TCC1_ADDR    ((u32)0x42002400)
#define TCC2_ADDR    ((u32)0x42002800)
#define TC3_ADDR     ((u32)0x42002C00)
#define TC4_ADDR     ((u32)0x42003000)
#define TC5_ADDR     ((u32)0x42003400)
#define TC6_ADDR     ((u32)0x42003800)
#define TC7_ADDR     ((u32)0x42003C00)
#define ADC_ADDR     ((u32)0x42004000)
#define AC_ADDR      ((u32)0x42004400)
#define DAC_ADDR     ((u32)0x42004800)
#define PTC_ADDR     ((u32)0x42004C00)
#define I2S_ADDR     ((u32)0x42005000)
#define AC1_ADDR     ((u32)0x42005400)
#define TCC3_ADDR    ((u32)0x42006000)

void hw_init(void);

/**
 * @brief Read the value of a 32bits memory mapped register
 *
 * @param  reg Address of the register to read
 * @return u32 Value of the register (32 bits)
 */
static inline u32 reg_rd(u32 reg)
{
	return( *(volatile u32 *)reg );
}

/**
 * @brief Read the value of a 8bits memory mapped register
 *
 * @param  reg Address of the register to read
 * @return u8  Value of the register (8 bits)
 */
static inline u8 reg8_rd(u32 reg)
{
	return( *(volatile u8 *)reg );
}

/**
 * @brief Read the value of a 16bits memory mapped register
 *
 * @param  reg Address of the register to read
 * @return u16 Value of the register (16 bits)
 */
static inline u16 reg16_rd(u32 reg)
{
	return( *(volatile u16 *)reg );
}

/**
 * @brief Write a 32 bits value to a memory mapped register
 *
 * @param reg   Address of the register to update
 * @param value New value to write into the register
 */
static inline void reg_wr(u32 reg, u32 value)
{
	*(volatile u32 *)reg = value;
}

/**
 * @brief Write a 16bits value to a memory mapped register
 *
 * @param reg   Address of the register to update
 * @param value New (16 bits) value to write into the register
 */
static inline void reg16_wr (u32 reg, u16 value)
{
	*(volatile u16 *)reg = value;
}

/**
 * @brief Write a 8bits value to a memory mapped register
 *
 * @param reg   Address of the register to update
 * @param value New (8 bits) value to write into the register
 */
static inline void reg8_wr(u32 reg, u8 value)
{
	*(volatile u8 *)reg = value;
}

/**
 * @brief Set some bits into a memory mapped register
 *
 * @param reg   Address of the register to update
 * @param value Mask of bits to set into the register
 */
static inline void reg_set(u32 reg, u32 value)
{
  *(volatile u32 *)reg = (*(volatile u32 *)reg | value);
}

#endif
