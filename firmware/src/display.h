/**
 * @file  display.h
 * @brief Definitions and prototypes for display driver
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
#ifndef DISPLAY_H
#define DISPLAY_H

#define DISP_MODE_CMD  0
#define DISP_MODE_DATA 1

void disp_init(void);
void disp_clear(unsigned char lines);
void disp_pos(unsigned int x, unsigned int y);
void disp_putc(char c);
void disp_puts(char *s);

void disp_test(int type);

#endif
/* EOF */
