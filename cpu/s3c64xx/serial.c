/*
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#include <s3c6410.h>

#ifdef CONFIG_SERIAL1
#define UART_NR	S3C64XX_UART0

#elif defined(CONFIG_SERIAL2)
#define UART_NR	S3C64XX_UART1

#elif defined(CONFIG_SERIAL3)
#define UART_NR	S3C64XX_UART2

#elif defined(CONFIG_SERIAL4)
#define UART_NR	S3C64XX_UART3

#else
#error "Bad: you didn't configure serial ..."
#endif

void serial_setbrg(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	int i;
	for (i = 0; i < 100; i++);
}


int serial_init(void)
{
	serial_setbrg();

	return (0);
}


int serial_getc(void)
{
	S3C64XX_UART *const uart = S3C64XX_GetBase_UART(UART_NR);

	/* wait for character to arrive */
	while (!(uart->UTRSTAT & 0x1));

	return uart->URXH & 0xff;
}


void serial_putc(const char c)
{
	S3C64XX_UART *const uart = S3C64XX_GetBase_UART(UART_NR);

	while (!(uart->UTRSTAT & 0x2));


	uart->UTXH = c;

	if (c == '\n')
		serial_putc('\r');
}

int serial_tstc(void)
{
	S3C64XX_UART *const uart = S3C64XX_GetBase_UART(UART_NR);

	return uart->UTRSTAT & 0x1;
}

void serial_puts(const char *s)
{
	while (*s) {
		serial_putc(*s++);
	}
}

