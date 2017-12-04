#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_	1


#ifndef CFG_ENV_OFFSET
#error "Need to define CFG_ENV_OFFSET when using CFG_ENV_IS_IN_NAND"
#endif

#ifndef CFG_ENV_SIZE
#error "Need to define CFG_ENV_SIZE when using CFG_ENV_IS_IN_NAND"
#endif


#define ENV_HEADER_SIZE	(sizeof(unsigned long))
#define ENV_SIZE (CFG_ENV_SIZE - ENV_HEADER_SIZE)

typedef	struct environment_s 
{
	unsigned long	crc;
	unsigned char	data[ENV_SIZE];
} env_t;


#endif	/* _ENVIRONMENT_H_ */
