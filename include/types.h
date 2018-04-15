#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H


#ifndef _LINUX_POSIX_TYPES_H
#define _LINUX_POSIX_TYPES_H
#endif

typedef unsigned int		size_t;


typedef unsigned char 		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;


typedef unsigned char 		__u8;
typedef unsigned short		__u16;
typedef unsigned int		__u32;
typedef unsigned long long	__u64;


typedef unsigned char 		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

typedef long long			int64_t;


typedef volatile unsigned char		vu_char;
typedef volatile unsigned short		vu_short;
typedef volatile unsigned long		vu_long;


typedef volatile unsigned char		S3C64XX_REG8;
typedef volatile unsigned short		S3C64XX_REG16;
typedef volatile unsigned long		S3C64XX_REG32;


#endif
