#ifndef __ASM_IO_H
#define __ASM_IO_H

#define readb(a) (*((vu_char *)(a)))
#define writeb(a, b) (*((vu_char *)(b)) = ((vu_char)a))

#define readw(a) (*((vu_short *)(a)))
#define writew(a, b) (*((vu_short *)(b)) = ((vu_short)a))

#define readl(a) (*((vu_long *)(a)))
#define writel(a, b) (*((vu_long *)(b)) = ((vu_long)a))

#endif
