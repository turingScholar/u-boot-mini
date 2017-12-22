#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 */
#define CONFIG_S3C6410			1
#define CONFIG_S3C64XX			1
#define CONFIG_SMDK6410		1



#define MEMORY_BASE_ADDRESS		0x50000000

#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1				MEMORY_BASE_ADDRESS
#define PHYS_SDRAM_1_SIZE			0x10000000



#define CONFIG_STACKSIZE		0x40000


#define CFG_MALLOC_LEN			(CFG_ENV_SIZE + 512*1024)
#define CFG_GBL_DATA_SIZE		128

#define CFG_STACK_SIZE			512*1024


#define CONFIG_IDENT_STRING	" for SMDK6410"
#define CFG_UBOOT_SIZE			(2*1024*1024)
#define CFG_PHY_UBOOT_BASE	MEMORY_BASE_ADDRESS + 0x7e00000
#define CFG_ENV_OFFSET			0x00080000
#define CFG_ENV_SIZE			0x80000




#ifdef CONFIG_ENABLE_MMU
#define CFG_UBOOT_BASE		0xc7e00000
#else
#define CFG_UBOOT_BASE		0x57e00000
#endif



#define CONFIG_ENABLE_MMU
#ifdef CONFIG_ENABLE_MMU
#define virt_to_phys(x)	virt_to_phy_smdk6410(x)
#else
#define virt_to_phys(x)	(x)
#endif



#define CONFIG_MEMORY_UPPER_CODE
#define BOARD_LATE_INIT


/*
 * Architecture magic and machine type
 */
#define MACH_TYPE		1626


#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO



#define CONFIG_SERIAL1			1

#define CONFIG_CMDLINE_EDITING
#define CONFIG_ENV_OVERWRITE
#define CONFIG_BAUDRATE		115200



/*
  * Command Settings
  */
#define CONFIG_COMMANDS \
			(CFG_CMD_NAND			| \
			CFG_CMD_MOVINAND		| \			
			CFG_CMD_LOADS			| \
			CFG_CMD_LOADB			| \
			CFG_CMD_ENV			| \
			CFG_CMD_USB)

#define CFG_CMD_NAND		0x00000001ULL
#define CFG_CMD_MOVINAND 	0x00000002ULL
#define CFG_CMD_LOADS		0x00000004ULL
#define CFG_CMD_LOADB		0x00000008ULL
#define CFG_CMD_ENV		0x00000010ULL
#define CFG_CMD_USB		0x00000020ULL
						


#define CONFIG_BOOTDELAY	8
#define CONFIG_BOOTARGS    	"root=/dev/mtdblock2 rootfstype=cramfs console=ttySAC0,115200"

#define CONFIG_ETHADDR			00:40:5c:26:0a:5b
#define CONFIG_NETMASK			255.255.255.0
#define CONFIG_IPADDR			192.168.1.20
#define CONFIG_SERVERIP		192.168.1.10
#define CONFIG_GATEWAYIP		192.168.1.1

#define CONFIG_ZERO_BOOTDELAY_CHECK



#define CFG_LONGHELP
#define CFG_PROMPT				"SMDK6410-DBG # "
#define CFG_CBSIZE				256
#define CFG_PBSIZE				384
#define CFG_MAXARGS			16
#define CFG_LOAD_ADDR			MEMORY_BASE_ADDRESS




/*
 * Clock & Power Settings
 */
#define CONFIG_SYS_CLK_FREQ	12000000
#define CFG_HZ					1562500



#define CONFIG_CLK_532_133_66
#define CONFIG_CLKSRC_CLKUART


#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)


#if defined(CONFIG_CLK_666_133_66)
#define APLL_MDIV	333
#define APLL_PDIV	3
#define APLL_SDIV	1
#undef  CONFIG_SYNC_MODE

#elif defined(CONFIG_CLK_532_133_66)
#define APLL_MDIV	266
#define APLL_PDIV	3
#define APLL_SDIV	1
#define CONFIG_SYNC_MODE

#elif defined(CONFIG_CLK_400_133_66) || defined(CONFIG_CLK_800_133_66)
#define APLL_MDIV	400
#define APLL_PDIV	3
#define APLL_SDIV	1
#define CONFIG_SYNC_MODE
#elif defined(CONFIG_CLK_400_100_50)
#define APLL_MDIV	400
#define APLL_PDIV	3
#define APLL_SDIV	2
#define CONFIG_SYNC_MODE
#elif defined(CONFIG_CLK_OTHERS)
#define APLL_MDIV	266
#define APLL_PDIV	3
#define APLL_SDIV	1
#define CONFIG_SYNC_MODE
#else
#error "Not Support Fequency or Mode!! you have to setup right configuration."
#endif


#if defined(CONFIG_CLK_800_133_66)
#define Startup_APLLdiv		0
#define Startup_HCLKx2div	2
#elif defined(CONFIG_CLK_400_133_66)
#define Startup_APLLdiv		1
#define Startup_HCLKx2div	2
#else
#define Startup_APLLdiv		0
#define Startup_HCLKx2div	1
#endif


#define CONFIG_UART_66

#define APLL_VAL			set_pll(APLL_MDIV, APLL_PDIV, APLL_SDIV)
#define Startup_APLL		(CONFIG_SYS_CLK_FREQ/(APLL_PDIV<<APLL_SDIV)*APLL_MDIV)



#define MPLL_MDIV		266
#define MPLL_PDIV		3
#define MPLL_SDIV		1

#define MPLL_VAL		set_pll(MPLL_MDIV, MPLL_PDIV, MPLL_SDIV)
#define Startup_MPLL		((CONFIG_SYS_CLK_FREQ)/(MPLL_PDIV<<MPLL_SDIV)*MPLL_MDIV)


#define	Startup_PCLKdiv		3
#define	Startup_HCLKdiv		1
#define	Startup_MPLLdiv		1


#define CLK_DIV_VAL	((Startup_PCLKdiv<<12)|(Startup_HCLKx2div<<9)|(Startup_HCLKdiv<<8)|(Startup_MPLLdiv<<4)|Startup_APLLdiv)


#if defined(CONFIG_SYNC_MODE)
#define Startup_HCLK	(Startup_APLL/(Startup_HCLKx2div+1)/(Startup_HCLKdiv+1))
#else
#define Startup_HCLK	(Startup_MPLL/(Startup_HCLKx2div+1)/(Startup_HCLKdiv+1))
#endif



/*-----------------------------------------------------------------------
 * Physical Memory Map
 */

#define DMC1_MEM_CFG		0x0001001A
#define DMC1_MEM_CFG2		0xB45
#define DMC1_CHIP0_CFG		0x150F0
#define DMC_DDR_32_CFG		0x0 


#define DDR_tREFRESH	7800
#define DDR_tRAS		45
#define DDR_tRC 			68
#define DDR_tRCD		23
#define DDR_tRFC		80
#define DDR_tRP 			23
#define DDR_tRRD		15
#define DDR_tWR 		15
#define DDR_tXSR		120
#define DDR_CASL		3


#define DMC_DDR_BA_EMRS 			2
#define DMC_DDR_MEM_CASLAT		3
#define DMC_DDR_CAS_LATENCY		(DDR_CASL<<1)	//6   Set Cas Latency to 3
#define DMC_DDR_t_DQSS				1				// Min 0.75 ~ 1.25
#define DMC_DDR_t_MRD				2				//Min 2 tck
#define DMC_DDR_t_RAS				(((Startup_HCLK / 1000 * DDR_tRAS) - 1) / 1000000 + 1)	//7, Min 45ns
#define DMC_DDR_t_RC				(((Startup_HCLK / 1000 * DDR_tRC) - 1) / 1000000 + 1) 	//10, Min 67.5ns
#define DMC_DDR_t_RCD				(((Startup_HCLK / 1000 * DDR_tRCD) - 1) / 1000000 + 1) 	//4,5(TRM), Min 22.5ns
#define DMC_DDR_schedule_RCD		((DMC_DDR_t_RCD - 3) << 3)
#define DMC_DDR_t_RFC				(((Startup_HCLK / 1000 * DDR_tRFC) - 1) / 1000000 + 1) 	//11,18(TRM) Min 80ns
#define DMC_DDR_schedule_RFC		((DMC_DDR_t_RFC - 3) << 5)
#define DMC_DDR_t_RP				(((Startup_HCLK / 1000 * DDR_tRP) - 1) / 1000000 + 1) 	//4, 5(TRM) Min 22.5ns
#define DMC_DDR_schedule_RP		((DMC_DDR_t_RP - 3) << 3)
#define DMC_DDR_t_RRD				(((Startup_HCLK / 1000 * DDR_tRRD) - 1) / 1000000 + 1)	//3, Min 15ns
#define DMC_DDR_t_WR				(((Startup_HCLK / 1000 * DDR_tWR) - 1) / 1000000 + 1)	//Min 15ns
#define DMC_DDR_t_WTR				2
#define DMC_DDR_t_XP				2							//1tck + tIS(1.5ns)
#define DMC_DDR_t_XSR				(((Startup_HCLK / 1000 * DDR_tXSR) - 1) / 1000000 + 1)	//17, Min 120ns
#define DMC_DDR_t_ESR				DMC_DDR_t_XSR
#define DMC_DDR_REFRESH_PRD		(((Startup_HCLK / 1000 * DDR_tREFRESH) - 1) / 1000000) 	// TRM 2656
#define DMC_DDR_USER_CONFIG		1							// 2b01 : mDDR



#define	CONFIG_MOVINAND


#define	CONFIG_S3C_USBD
#define	USBD_DOWN_ADDR               0xc0000000



#define	CONFIG_BOOT_NAND
#define	CONFIG_NAND
#define	CONFIG_NAND_BL1_8BIT_ECC


#if defined(CONFIG_BOOT_NAND)

#define CFG_ENV_IS_IN_NAND
#define CFG_NAND_HWECC
#define CONFIG_BOOTCOMMAND	"nand read 0xc0008000 0x100000 0x500000;bootm 0xc0008000"

#elif defined(CONFIG_BOOT_MOVINAND)

#define CFG_ENV_IS_IN_MOVINAND
#define CONFIG_BOOTCOMMAND	"movi read kernel c0008000;movi read rootfs c0800000;bootm c0008000"

#else
# error Define one of CONFIG_BOOT_{NAND|MOVINAND|ONENAND|ONENAND_IROM}
#endif

#endif
