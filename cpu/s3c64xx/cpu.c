/*
 * (C) Copyright 2004 Texas Insturments
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <s3c6410.h>


#ifdef CONFIG_USE_IRQ
DECLARE_GLOBAL_DATA_PTR;
#endif


#define	APLL	0
#define	MPLL	1
#define	EPLL	2


#define	C1_MMU			(1<<0)		/* mmu off/on */
#define	C1_ALIGN		(1<<1)		/* alignment faults off/on */
#define	C1_DC			(1<<2)		/* dcache off/on */
#define	C1_WB			(1<<3)		/* merging write buffer on/off */
#define	C1_BIG_ENDIAN	(1<<7)		/* big endian off/on */
#define	C1_SYS_PROT	(1<<8)		/* system protection */
#define	C1_ROM_PROT	(1<<9)		/* ROM protection */
#define	C1_BRANCH		(1<<11)		/* branch prediction off/on */
#define	C1_IC			(1<<12)		/* icache off/on */
#define	C1_HIGH_VECTORS	(1<<13)	/* location of vectors: low/high addresses */
#define	RESERVED_1		(0xf << 3)	/* must be 111b for R/W */


int cpu_init (void)
{
#ifdef CONFIG_USE_IRQ
	IRQ_STACK_START = _armboot_start - CFG_MALLOC_LEN - CFG_GBL_DATA_SIZE - 4;
	FIQ_STACK_START = IRQ_STACK_START - CONFIG_STACKSIZE_IRQ;
#endif
	return 0;
}


void reset_cpu(ulong ignored)
{
	printf("reset... \n\n\n");

#if defined(CONFIG_S3C6400)
	SW_RST_REG = 0x6400;
#elif defined(CONFIG_S3C6410)
	SW_RST_REG = 0x6410;
#elif defined(CONFIG_S3C6430)
	SW_RST_REG = 0x6410;
#endif

	/* loop forever and wait for reset to happen */
	while (1)
	{
		if (serial_tstc())
		{
			serial_getc();
			break;
		}
	}
	/*NOTREACHED*/
}

int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	disable_interrupts ();
	reset_cpu (0);
	/*NOTREACHED*/
	return(0);
}


/* read co-processor 15, register #1 (control register) */
static unsigned long read_p15_c1 (void)
{
	unsigned long value;

	__asm__ __volatile__(
						"mrc	p15, 0, %0, c1, c0, 0   @ read control reg\n"
						: "=r" (value)
						:
						: "memory");
	return value;
}

/* write to co-processor 15, register #1 (control register) */
static void write_p15_c1 (unsigned long value)
{
	__asm__ __volatile__(
						"mcr	p15, 0, %0, c1, c0, 0   @ write it back\n"
						:
						: "r" (value)
						: "memory");

	read_p15_c1 ();
}

static void cp_delay (void)
{
	volatile int i;

	/* Many OMAP regs need at least 2 nops  */
	for (i = 0; i < 100; i++);
}

void icache_enable (void)
{
	ulong reg;

	reg = read_p15_c1 ();	/* get control reg. */
	cp_delay ();
	write_p15_c1 (reg | C1_IC);
}

void icache_disable (void)
{
	ulong reg;

	reg = read_p15_c1 ();
	cp_delay ();
	write_p15_c1 (reg & ~C1_IC);
}

int icache_status (void)
{
	return(read_p15_c1 () & C1_IC) != 0;
}


void dcache_enable (void)
{
	ulong reg;

	reg = read_p15_c1 ();
	cp_delay ();
	write_p15_c1 (reg | C1_DC);
}

void dcache_disable (void)
{
	ulong reg;

	reg = read_p15_c1 ();
	cp_delay ();
	reg &= ~C1_DC;
	write_p15_c1 (reg);
}

int dcache_status (void)
{
	return (read_p15_c1 () & C1_DC) != 0;
}

void branch_enable (void)
{
	ulong reg;

	reg = read_p15_c1 ();
	cp_delay ();
	write_p15_c1 (reg | C1_BRANCH);
}

void branch_disable (void)
{
	ulong reg;

	reg = read_p15_c1 ();
	cp_delay ();
	reg &= ~C1_BRANCH;
	write_p15_c1 (reg);
}

int branch_status (void)
{
	return (read_p15_c1 () & C1_BRANCH) != 0;
}



static ulong get_PLLCLK(int pllreg)
{
	ulong r, m, p, s;

	if (pllreg == APLL)
		r = APLL_CON_REG;
	else if (pllreg == MPLL)
		r = MPLL_CON_REG;
	else if (pllreg == EPLL)
		r = EPLL_CON0_REG;
	else
		hang();

	m = (r>>16) & 0x3ff;
	p = (r>>8) & 0x3f;
	s = r & 0x7;

	return (m * (CONFIG_SYS_CLK_FREQ / (p * (1 << s))));
}

/* return ARMCORE frequency */
ulong get_ARMCLK(void)
{
	ulong div;

	div = CLK_DIV0_REG;
	
	return (get_PLLCLK(APLL) / ((div & 0x7) + 1));
}

/* return FCLK frequency */
ulong get_FCLK(void)
{
	return (get_PLLCLK(APLL));
}

/* return HCLK frequency */
ulong get_HCLK(void)
{
	ulong fclk;

	uint hclkx2_div = ((CLK_DIV0_REG>>9) & 0x7) + 1;
	uint hclk_div = ((CLK_DIV0_REG>>8) & 0x1) + 1;
	
	if(OTHERS_REG & 0x80)
		fclk = get_FCLK();		// SYNC Mode
	else
		fclk = get_PLLCLK(MPLL);	// ASYNC Mode

	return fclk/(hclk_div * hclkx2_div);
}

/* return PCLK frequency */
ulong get_PCLK(void)
{
	ulong fclk;
	uint hclkx2_div = ((CLK_DIV0_REG>>9) & 0x7) + 1;
	uint pre_div = ((CLK_DIV0_REG>>12) & 0xf) + 1;

	if(OTHERS_REG & 0x80)
		fclk = get_FCLK();		// SYNC Mode
	else
		fclk = get_PLLCLK(MPLL);	// ASYNC Mode

	return fclk/(hclkx2_div * pre_div);
}

/* return UCLK frequency */
ulong get_UCLK(void)
{
	return (get_PLLCLK(EPLL));
}


int print_cpuinfo(void)
{
	printf("****************************************\r\n");
	printf("**    u-boot 1.1.6                    **\r\n");
	printf("**    Updated for TE6410 Board        **\r\n");
	printf("**    Version 1.0 (10-01-15)          **\r\n");
	printf("**    OEM: Forlinx Embedded           **\r\n");
	printf("**    Web: http://www.witech.com.cn   **\r\n");	
	printf("****************************************\r\n");

	printf("\nCPU:     S3C6410 @%dMHz\n", get_ARMCLK()/1000000);
	printf("         Fclk = %dMHz, Hclk = %dMHz, Pclk = %dMHz",
			get_FCLK()/1000000, get_HCLK()/1000000, get_PCLK()/1000000);

	puts(", Serial = CLKUART ");


	if(OTHERS_REG & 0x80)
		printf("(SYNC Mode) \n");
	else
		printf("(ASYNC Mode) \n");
	return 0;
}


int cleanup_before_linux (void)
{
	/*
	 * this function is called just before we call linux
	 * it prepares the processor for linux
	 *
	 * we turn off caches etc ...
	 */

	unsigned long i;

	disable_interrupts ();

#ifdef CONFIG_LCD
	{
		extern void lcd_disable(void);
		extern void lcd_panel_disable(void);

		lcd_disable(); /* proper disable of lcd & panel */
		lcd_panel_disable();
	}
#endif

	/* turn off I/D-cache */
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (i));
	i &= ~(C1_DC | C1_IC);
	asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (i));

	/* flush I/D-cache */
	i = 0;
	asm ("mcr p15, 0, %0, c7, c7, 0": :"r" (i));  /* invalidate both caches and flush btb */
	asm ("mcr p15, 0, %0, c7, c10, 4": :"r" (i)); /* mem barrier to sync things */
	return(0);
}



