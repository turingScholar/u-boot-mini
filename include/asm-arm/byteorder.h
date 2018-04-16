/*
 *  linux/include/asm-arm/byteorder.h
 *
 * ARM Endian-ness.  In little endian mode, the data bus is connected such
 * that byte accesses appear as:
 *  0 = d0...d7, 1 = d8...d15, 2 = d16...d23, 3 = d24...d31
 * and word accesses (data or instruction) appear as:
 *  d0...d31
 *
 * When in big endian mode, byte accesses appear as:
 *  0 = d24...d31, 1 = d16...d23, 2 = d8...d15, 3 = d0...d7
 * and word accesses (data or instruction) appear as:
 *  d0...d31
 */
#ifndef __ASM_ARM_BYTEORDER_H
#define __ASM_ARM_BYTEORDER_H
#include <types.h>

#if !defined(__STRICT_ANSI__) || defined(__KERNEL__)
#define __BYTEORDER_HAS_U64__
#define __SWAB_64_THRU_32__
#endif


#define __le16_to_cpu(x) ((__u16)(x))
#define __le32_to_cpu(x) ((__u32)(x))

#define __cpu_to_le16(x) ((__u16)(x))
#define __cpu_to_le32(x) ((__u32)(x))


#define SWAP_LONG(x) \
	((__u32)( \
		(((__u32)(x) & (__u32)0x000000ffUL) << 24) | \
		(((__u32)(x) & (__u32)0x0000ff00UL) <<  8) | \
		(((__u32)(x) & (__u32)0x00ff0000UL) >>  8) | \
		(((__u32)(x) & (__u32)0xff000000UL) >> 24) ))


#define     ntohl(a)	SWAP_LONG(a)
#define     htonl(a)	SWAP_LONG(a)

#endif
