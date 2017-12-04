/*
 *  linux/include/asm-arm/hardware.h
 *
 *  Copyright (C) 1996 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  Common hardware definitions
 */

#ifndef __ASM_HARDWARE_H
#define __ASM_HARDWARE_H


#define UData(Data)	((unsigned long) (Data))

#define __REG(x)	(*(volatile unsigned long *)(x))
#define __REGl(x)	(*(volatile unsigned long *)(x))
#define __REGw(x)	(*(volatile unsigned short *)(x))
#define __REGb(x)	(*(volatile unsigned char *)(x))
#define __REG2(x,y)	(*(volatile unsigned long *)((x) + (y)))

#define read(x)	(*(volatile unsigned long *)(x))
#define readl(x)	(*(volatile unsigned long *)(x))
#define readw(x)	(*(volatile unsigned short *)(x))
#define readb(x)	(*(volatile unsigned char *)(x))


#define writeb(v,a)		(*(volatile unsigned char *)(a) = (v))
#define writew(v,a)		(*(volatile unsigned short *)(a) = (v))
#define writel(v,a)		(*(volatile unsigned int *)(a) = (v))



#define IF_TYPE_UNKNOWN		0
#define IF_TYPE_IDE				1
#define IF_TYPE_SCSI				2
#define IF_TYPE_ATAPI			3
#define IF_TYPE_USB				4
#define IF_TYPE_DOC				5
#define IF_TYPE_MMC				6
#define IF_TYPE_SD				7
#define IF_TYPE_SATA			8


#define PART_TYPE_UNKNOWN	0x00
#define PART_TYPE_MAC			0x01
#define PART_TYPE_DOS			0x02
#define PART_TYPE_ISO			0x03
#define PART_TYPE_AMIGA		0x04



#define Fld(Size, Shft)	(((Size) << 16) + (Shft))

#define FSize(Field)	((Field) >> 16)
#define FShft(Field)	((Field) & 0x0000FFFF)
#define FMsk(Field)	(((UData (1) << FSize (Field)) - 1) << FShft (Field))
#define FAlnMsk(Field)	((UData (1) << FSize (Field)) - 1)
#define F1stBit(Field)	(UData (1) << FShft (Field))

#define FClrBit(Data, Bit)	(Data = (Data & ~(Bit)))
#define FClrFld(Data, Field)	(Data = (Data & ~FMsk(Field)))

#define FInsrt(Value, Field) \
                	(UData (Value) << FShft (Field))

#define FExtr(Data, Field) \
                	((UData (Data) >> FShft (Field)) & FAlnMsk (Field))

#endif
