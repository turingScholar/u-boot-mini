#ifndef __HS_MMC_H__
#define __HS_MMC_H__


extern uint movi_hc;
extern uint movi_sectors;


struct sd_cid 
{
	char			pnm_0;	/* product name */
	char			oid_1;	/* OEM/application ID */
	char			oid_0;
	uchar		mid;	/* manufacturer ID */
	char			pnm_4;
	char			pnm_3;
	char			pnm_2;
	char			pnm_1;
	uchar		psn_2;	/* product serial number */
	uchar		psn_1;
	uchar		psn_0;	/* MSB */
	uchar		prv;	/* product revision */
	uchar		crc;	/* CRC7 checksum, b0 is unused and set to 1 */
	uchar		mdt_1;	/* manufacturing date, LSB, RRRRyyyy yyyymmmm */
	uchar		mdt_0;	/* MSB */
	uchar		psn_3;	/* LSB */
};

enum card_type 
{
	CARDTYPE_NONE = 0,
	CARDTYPE_MMC,
	CARDTYPE_SD,
	CARDTYPE_SD20,
	CARDTYPE_SDHC
};



#define	SD_HCLK		1
#define	SD_EPLL			2
#define	SD_EXTCLK		3

#define	NORMAL	0
#define	HIGH	1

//Normal Interrupt Signal Enable
#define	READWAIT_SIG_INT_EN		(1<<10)
#define	CARD_SIG_INT_EN			(1<<8)
#define	CARD_REMOVAL_SIG_INT_EN		(1<<7)
#define	CARD_INSERT_SIG_INT_EN		(1<<6)
#define	BUFFER_READREADY_SIG_INT_EN	(1<<5)
#define	BUFFER_WRITEREADY_SIG_INT_EN	(1<<4)
#define	DMA_SIG_INT_EN			(1<<3)
#define	BLOCKGAP_EVENT_SIG_INT_EN	(1<<2)
#define	TRANSFERCOMPLETE_SIG_INT_EN	(1<<1)
#define	COMMANDCOMPLETE_SIG_INT_EN	(1<<0)

//Normal Interrupt Status Enable
#define	READWAIT_STS_INT_EN		(1<<10)
#define	CARD_STS_INT_EN			(1<<8)
#define	CARD_REMOVAL_STS_INT_EN		(1<<7)
#define	CARD_INSERT_STS_INT_EN		(1<<6)
#define	BUFFER_READREADY_STS_INT_EN	(1<<5)
#define	BUFFER_WRITEREADY_STS_INT_EN	(1<<4)
#define	DMA_STS_INT_EN			(1<<3)
#define	BLOCKGAP_EVENT_STS_INT_EN	(1<<2)
#define	TRANSFERCOMPLETE_STS_INT_EN	(1<<1)
#define	COMMANDCOMPLETE_STS_INT_EN	(1<<0)




#define MMC_RSP_PRESENT	(1 << 0)
#define MMC_RSP_136		(1 << 1)		/* 136 bit response */
#define MMC_RSP_CRC		(1 << 2)		/* expect valid crc */
#define MMC_RSP_BUSY		(1 << 3)		/* card may send busy */
#define MMC_RSP_OPCODE	(1 << 4)		/* response contains opcode */

#define MMC_CMD_MASK		(3 << 5)
#define MMC_CMD_AC			(0 << 5)
#define MMC_CMD_ADTC		(1 << 5)
#define MMC_CMD_BC			(2 << 5)
#define MMC_CMD_BCR		(3 << 5)


#define MMC_RSP_NONE	(0)
#define MMC_RSP_R1	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1B	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY)
#define MMC_RSP_R2	(MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
#define MMC_RSP_R3	(MMC_RSP_PRESENT)
#define MMC_RSP_R4	(MMC_RSP_PRESENT)
#define MMC_RSP_R5	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R6	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R7	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)

#define mmc_resp_type(f) ((f) & (MMC_RSP_PRESENT | MMC_RSP_136 | MMC_RSP_CRC |\
				 MMC_RSP_BUSY | MMC_RSP_OPCODE))
#define mmc_cmd_type(f) ((f) & MMC_CMD_MASK)



#define MMC_DATA_WRITE	(1 << 8)
#define MMC_DATA_READ	(1 << 9)
#define MMC_DATA_STREAM	(1 << 10)
#define MMC_DATA_MULTI	(1 << 11)





#define SD_SEND_IF_COND           8   /* bcr  [11:0] See below   R7  */


   /* class 1 */
#define MMC_GO_IDLE_STATE         0   /* bc                          */
#define MMC_SEND_OP_COND          1   /* bcr  [31:0] OCR         R3  */
#define MMC_ALL_SEND_CID          2   /* bcr                     R2  */
#define MMC_SET_RELATIVE_ADDR     3   /* ac   [31:16] RCA        R1  */
#define MMC_SET_DSR               4   /* bc   [31:16] RCA            */
#define MMC_SWITCH                6   /* ac                      R1b */
#define MMC_SELECT_CARD           7   /* ac   [31:16] RCA        R1  */
#define MMC_SEND_EXT_CSD          8   /* adtc                    R1  */
#define MMC_SEND_CSD              9   /* ac   [31:16] RCA        R2  */
#define MMC_SEND_CID             10   /* ac   [31:16] RCA        R2  */
#define MMC_READ_DAT_UNTIL_STOP  11   /* adtc [31:0] dadr        R1  */
#define MMC_STOP_TRANSMISSION    12   /* ac                      R1b */
#define MMC_SEND_STATUS	         13   /* ac   [31:16] RCA        R1  */
#define MMC_BUSTEST_R            14   /* adtc                    R1  */
#define MMC_GO_INACTIVE_STATE    15   /* ac   [31:16] RCA            */
#define MMC_BUSTEST_W            19   /* adtc                    R1  */

  /* class 2 */
#define MMC_SET_BLOCKLEN         16   /* ac   [31:0] block len   R1  */
#define MMC_READ_SINGLE_BLOCK    17   /* adtc [31:0] data addr   R1  */
#define MMC_READ_MULTIPLE_BLOCK  18   /* adtc [31:0] data addr   R1  */

  /* class 3 */
#define MMC_WRITE_DAT_UNTIL_STOP 20   /* adtc [31:0] data addr   R1  */

  /* class 4 */
#define MMC_SET_BLOCK_COUNT      23   /* adtc [31:0] data addr   R1  */
#define MMC_WRITE_BLOCK          24   /* adtc [31:0] data addr   R1  */
#define MMC_WRITE_MULTIPLE_BLOCK 25   /* adtc                    R1  */
#define MMC_PROGRAM_CID          26   /* adtc                    R1  */
#define MMC_PROGRAM_CSD          27   /* adtc                    R1  */

  /* class 6 */
#define MMC_SET_WRITE_PROT       28   /* ac   [31:0] data addr   R1b */
#define MMC_CLR_WRITE_PROT       29   /* ac   [31:0] data addr   R1b */
#define MMC_SEND_WRITE_PROT      30   /* adtc [31:0] wpdata addr R1  */

  /* class 5 */
#define MMC_ERASE_GROUP_START    35   /* ac   [31:0] data addr   R1  */
#define MMC_ERASE_GROUP_END      36   /* ac   [31:0] data addr   R1  */
#define MMC_ERASE                38   /* ac                      R1b */

  /* class 9 */
#define MMC_FAST_IO              39   /* ac   <Complex>          R4  */
#define MMC_GO_IRQ_STATE         40   /* bcr                     R5  */

  /* class 7 */
#define MMC_LOCK_UNLOCK          42   /* adtc                    R1b */

  /* class 8 */
#define MMC_APP_CMD              55   /* ac   [31:16] RCA        R1  */
#define MMC_GEN_CMD              56   /* adtc [0] RD/WR          R1  */

/* SD commands                           type  argument     response */
  /* class 8 */
/* This is basically the same command as for MMC with some quirks. */
#define SD_SEND_RELATIVE_ADDR     3   /* ac                      R6  */

  /* Application commands */
#define SD_APP_SET_BUS_WIDTH      6   /* ac   [1:0] bus width    R1  */
#define SD_APP_OP_COND           41   /* bcr  [31:0] OCR         R3  */
#define SD_APP_SEND_SCR          51   /* adtc                    R1  */







#ifndef __MOVI_H__
#define __MOVI_H__

#define HSMMC_CHANNEL			0
#define MOVI_INIT_REQUIRED		0

#if defined(CONFIG_S3C6410)
#define	TCM_BASE		0x0C004000
#define	BL2_BASE		(CFG_PHY_UBOOT_BASE)
#endif


#if defined(CONFIG_S3C6410)
#define CopyMovitoMem(a,b,c,d,e)	(((int(*)(int, uint, ushort, uint *, int))(*((uint *)(TCM_BASE + 0x8))))(a,b,c,d,e))
#endif

/* size information */
#if defined(CONFIG_S3C6410)
#define SS_SIZE			(8 * 1024)
#define eFUSE_SIZE		(1 * 1024)	// 0.5k eFuse, 0.5k reserved`
#endif

/* movinand definitions */
#define MOVI_BLKSIZE		512


#define MOVI_TOTAL_BLKCNT	7864320 // 7864320 // 3995648 // 1003520 /* static movinand total block count: for writing to movinand when nand boot */
#define MOVI_HIGH_CAPACITY	0


/* partition information */
#define PART_SIZE_BL		(256 * 1024)
#define PART_SIZE_KERNEL	(4 * 1024 * 1024)
#define PART_SIZE_ROOTFS	(8 * 1024 * 1024)

#define MOVI_LAST_BLKPOS	(MOVI_TOTAL_BLKCNT - (eFUSE_SIZE / MOVI_BLKSIZE))
#define MOVI_BL1_BLKCNT		(SS_SIZE / MOVI_BLKSIZE)
#define MOVI_ENV_BLKCNT		(CFG_ENV_SIZE / MOVI_BLKSIZE)
#define MOVI_BL2_BLKCNT		(PART_SIZE_BL / MOVI_BLKSIZE)
#define MOVI_ZIMAGE_BLKCNT	(PART_SIZE_KERNEL / MOVI_BLKSIZE)
#define MOVI_BL2_POS		(MOVI_LAST_BLKPOS - MOVI_BL1_BLKCNT - MOVI_BL2_BLKCNT - MOVI_ENV_BLKCNT)
#define MOVI_ROOTFS_BLKCNT	(PART_SIZE_ROOTFS / MOVI_BLKSIZE)

struct movi_offset_t 
{
	uint	last;
	uint	bl1;
	uint	env;
	uint	bl2;
	uint	kernel;
	uint	rootfs;
};

/* external functions */
extern void hsmmc_set_gpio(void);
extern void hsmmc_reset (void);
extern int hsmmc_init (void);
extern int movi_init(void);
extern void movi_set_capacity(void);
extern int movi_set_ofs(uint last);
extern void movi_write (uint addr, uint start_blk, uint blknum);
extern void movi_read (uint addr, uint start_blk, uint blknum);
extern void movi_write_env(ulong addr);
extern void movi_read_env(ulong addr);
extern void movi_bl2_copy(void);


extern ulong virt_to_phy_smdk6410(ulong addr);


extern uint movi_hc;
extern struct movi_offset_t ofsinfo;

#endif



#endif /*__HS_MMC_H__*/
