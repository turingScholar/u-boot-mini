#ifndef	__ASM_GBL_DATA_H
#define __ASM_GBL_DATA_H


typedef struct bd_info 
{
    int			bi_baudrate;	/* serial console baudrate */
    unsigned long	bi_ip_addr;	/* IP Address */
    unsigned char	bi_enetaddr[6]; /* Ethernet adress */
    struct environment_s	       *bi_env;
    uint32_t	        bi_arch_number;	/* unique id for this board */
    uint32_t	        bi_boot_params;	/* where this board expects params */
    struct				/* RAM configuration */
    {
	uint32_t start;
	uint32_t size;
    } bi_dram[CONFIG_NR_DRAM_BANKS];
	
} bd_t;



typedef	struct	global_data 
{
	bd_t		*bd;
	unsigned long	flags;
	unsigned long	baudrate;
	unsigned long	have_console;	/* serial_init() was called */
	unsigned long	reloc_off;	/* Relocation Offset */
	unsigned long	env_addr;	/* Address  of Environment struct */
	unsigned long	env_valid;	/* Checksum of Environment valid? */
	unsigned long	fb_base;	/* base address of frame buffer */
	void		**jt;		/* jump table */
} gd_t;



#define	GD_FLG_RELOC	0x00001		/* Code was relocated to RAM		*/
#define	GD_FLG_DEVINIT	0x00002		/* Devices have been initialized	*/
#define	GD_FLG_SILENT	0x00004		/* Silent mode				*/

#define DECLARE_GLOBAL_DATA_PTR     register volatile gd_t *gd asm ("r8")

#endif /* __ASM_GBL_DATA_H */
