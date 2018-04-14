#include <common.h>
#if defined(CONFIG_SMDK6410)

#define CONFIG_SUPPORT_MMC_PLUS
#define HCLK_OPERATION

#ifdef DEBUG_HSMMC
#define dbg(x...) printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif


#include <s3c6410.h>
#include <part.h>
#include <asm/io.h>
#include <hs_mmc.h>



static ulong mmc_bread (int dev_num, ulong blknr, ulong blkcnt, ulong* dst);


#define SDI_Tx_buffer_HSMMC		(0x51000000)
#define SDI_Rx_buffer_HSMMC		(0x51000000+(0x300000))
#define SDI_Compare_buffer_HSMMC	(0x51000000+(0x600000))

#define Card_OneBlockSize_ver1		512
#define MMC_BLOCK_SIZE Card_OneBlockSize_ver1
#define MMC_DEFAULT_RCA			(1<<16)

/*  Global variables */
static uint *Tx_buffer_HSMMC;
static uint *Rx_buffer_HSMMC;
static uint *Compare_buffer_HSMMC;

static vu_long rd_cnt_HSMMC;
static vu_long wt_cnt_HSMMC;
static vu_long BlockNum_HSMMC = 0;

static vu_long WriteBlockCnt_INT = 0;
static vu_long ReadBlockCnt_INT = 0;
static vu_long WRITEINT_DONE = 0;
static vu_long READINT_DONE = 0;
static vu_long COMPARE_INT_DONE = 0;
static vu_long CompareCnt_INT = 0;
static vu_long BufferBoundary_INT_Cnt = 0;

static vu_long HS_DMA_END = 0;
static vu_long HS_CARD_DETECT = 0;

static uint ocr_check = 0;
static uint mmc_card = 0;
static uint rca = 0;
static uint mmc_spec, sd_spec;

static ulong HCLK;
static uint card_mid = 0;

/* extern variables */
extern uint movi_hc;
enum card_type card_type;


/* extern functions */
extern ulong get_HCLK(void);
static block_dev_desc_t mmc_dev;
block_dev_desc_t* mmc_get_dev(int dev)
{
	return( (block_dev_desc_t*) &mmc_dev);
}

#define s3c_hsmmc_readl(x)	__REGl((ELFIN_HSMMC_BASE + (HSMMC_CHANNEL * 0x100000)) + (x))
#define s3c_hsmmc_readw(x)	__REGw((ELFIN_HSMMC_BASE + (HSMMC_CHANNEL * 0x100000)) + (x))
#define s3c_hsmmc_readb(x)	__REGb((ELFIN_HSMMC_BASE + (HSMMC_CHANNEL * 0x100000)) + (x))


#define s3c_hsmmc_writel(v,x)		writel((v),(ELFIN_HSMMC_BASE + (HSMMC_CHANNEL * 0x100000)) + (x))
#define s3c_hsmmc_writew(v,x)	writew((v),(ELFIN_HSMMC_BASE + (HSMMC_CHANNEL * 0x100000)) + (x))
#define s3c_hsmmc_writeb(v,x)	writeb((v),(ELFIN_HSMMC_BASE + (HSMMC_CHANNEL * 0x100000)) + (x))

#define UNSTUFF_BITS(resp,start,size)                                   \
        ({                                                              \
                const int __size = size;                                \
                const uint __mask = (__size < 32 ? 1 << __size : 0) - 1; \
                const int __off = 3 - ((start) / 32);                   \
                const int __shft = (start) & 31;                        \
                uint __res;                                              \
                                                                        \
                __res = resp[__off] >> __shft;                          \
                if (__size + __shft > 32)                               \
                        __res |= resp[__off-1] << ((32 - __shft) & 31); \
                __res & __mask;                                         \
        })


uint movi_hc = 0;
uint movi_sectors;


void movi_set_capacity(void)
{
	movi_hc = 1;
}

int movi_set_ofs(uint last)
{
	int changed = 0;

	if (ofsinfo.last != last) 
	{
		ofsinfo.last 	= last - (eFUSE_SIZE / MOVI_BLKSIZE);
		ofsinfo.bl1	= ofsinfo.last - MOVI_BL1_BLKCNT;
		ofsinfo.env	= ofsinfo.bl1 - MOVI_ENV_BLKCNT;
		ofsinfo.bl2	= ofsinfo.bl1 - (MOVI_BL2_BLKCNT + MOVI_ENV_BLKCNT);
		ofsinfo.kernel	= ofsinfo.bl2 - MOVI_ZIMAGE_BLKCNT;
		ofsinfo.rootfs	= ofsinfo.kernel - MOVI_ROOTFS_BLKCNT;
		changed = 1;
	}

	return changed;
}

int movi_init(void)
{
	hsmmc_set_gpio();
	hsmmc_reset();
	if (hsmmc_init()) {
		printf("\nCard Initialization failed.\n");
		return -1;
	}
	return 1;
}

void movi_write_env(ulong addr)
{
	movi_write((uint)addr, ofsinfo.env, MOVI_ENV_BLKCNT);
}

void movi_read_env(ulong addr)
{
	movi_read((uint)addr, ofsinfo.env, MOVI_ENV_BLKCNT);
}

void movi_bl2_copy(void)
{
	writel(readl(HM_CONTROL4) | (0x3 << 16), HM_CONTROL4);
	CopyMovitoMem(HSMMC_CHANNEL, MOVI_BL2_POS, MOVI_BL2_BLKCNT, (uint *)BL2_BASE, MOVI_INIT_REQUIRED);
}


static int wait_for_r_buf_ready (void)
{
	uint uLoop = 0;

	while (!(s3c_hsmmc_readw(HM_NORINTSTS) & 0x20)) {
		if (uLoop % 500000 == 0 && uLoop > 0) {
			return 0;
		}
		uLoop++;
	}
	return 1;
}

static int wait_for_cmd_done (void)
{
	ushort n_int, e_int;

	dbg("wait_for_cmd_done\n");
	while(1){

		n_int = s3c_hsmmc_readw(HM_NORINTSTS);
		dbg("  HM_NORINTSTS: %04x\n", n_int);
		if (n_int & 0x8000)
			/* any error */
			break;
		if (n_int & 0x0001)
			/* command complete */
			return 0;
	}

	e_int = s3c_hsmmc_readw(HM_ERRINTSTS);
	s3c_hsmmc_writew(e_int, HM_ERRINTSTS);
	s3c_hsmmc_writew(n_int, HM_NORINTSTS);

	printf("cmd error: 0x%04x, HM_NORINTSTS: 0x%04x\n", e_int, n_int);
	return 1;
}

/* XXX: must modify algorithm. it has bugs. by scsuh */
static int wait_for_data_done (void)
{
	while (!(s3c_hsmmc_readw(HM_NORINTSTS) & 0x2))
		return 1;

	return 0;
}

static void ClearCommandCompleteStatus(void)
{
	s3c_hsmmc_writew(1 << 0, HM_NORINTSTS);
	while (s3c_hsmmc_readw(HM_NORINTSTS) & 0x1) {
		s3c_hsmmc_writew(1 << 0, HM_NORINTSTS);
	}
}

static void ClearTransferCompleteStatus(void)
{
	s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTS) | (1 << 1), HM_NORINTSTS);
	while (s3c_hsmmc_readw(HM_NORINTSTS) & 0x2) {
		s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTS) | (1 << 1), HM_NORINTSTS);
	}
}

static void ClearDMAStatus(void)
{
	s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTS) | (1 << 3), HM_NORINTSTS);
	while (s3c_hsmmc_readw(HM_NORINTSTS) & 0x8) {
		s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTS) | (1 << 3), HM_NORINTSTS);
	}
}


static void ClearBufferReadReadyStatus(void)
{
	s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTS) | (1 << 5), HM_NORINTSTS);
	while (s3c_hsmmc_readw(HM_NORINTSTS) & 0x20)
		s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTS) | (1 << 5), HM_NORINTSTS);
}

static void card_irq_enable(ushort temp)
{
	s3c_hsmmc_writew((s3c_hsmmc_readw(HM_NORINTSTSEN) & 0xFEFF) | (temp << 8), HM_NORINTSTSEN);
}

void hsmmc_reset (void)
{
	s3c_hsmmc_writeb(0x3, HM_SWRST);
}

void hsmmc_set_gpio (void)
{
	uint reg;

#if (HSMMC_CHANNEL == 0)
	reg = readl(ELFIN_GPIO_BASE + GPGCON_OFFSET) & 0xf0000000;
	writel(reg | 0x02222222, ELFIN_GPIO_BASE + GPGCON_OFFSET);

	reg = readl(ELFIN_GPIO_BASE + GPGPUD_OFFSET) & 0xfffff000;
	writel(reg, ELFIN_GPIO_BASE + GPGPUD_OFFSET);
#elif (HSMMC_CHANNEL == 1)
	writel(0x00222222, ELFIN_GPIO_BASE + GPHCON0_OFFSET);
	writel(0x00000000, ELFIN_GPIO_BASE + GPHCON1_OFFSET);

	reg = readl(ELFIN_GPIO_BASE + GPHPUD_OFFSET) & 0xfffff000;
	writel(reg, ELFIN_GPIO_BASE + GPHPUD_OFFSET);
#else
	printf("### HS-MMC channel is not defined!\n");
#endif
}

static void set_transfer_mode_register (uint MultiBlk, uint DataDirection, uint AutoCmd12En, uint BlockCntEn, uint DmaEn)
{
	s3c_hsmmc_writew((s3c_hsmmc_readw(HM_TRNMOD) & ~(0xffff)) | (MultiBlk << 5)
		| (DataDirection << 4) | (AutoCmd12En << 2)
		| (BlockCntEn << 1) | (DmaEn << 0), HM_TRNMOD);
	dbg("\nHM_TRNMOD = 0x%04x\n", HM_TRNMOD);
}

static void set_arg_register (uint arg)
{
	s3c_hsmmc_writel(arg, HM_ARGUMENT);
}

static void set_blkcnt_register(ushort uBlkCnt)
{
	s3c_hsmmc_writew(uBlkCnt, HM_BLKCNT);
}

static void SetSystemAddressReg(uint SysAddr)
{
	s3c_hsmmc_writel(SysAddr, HM_SYSAD);
}

static void set_blksize_register(ushort uDmaBufBoundary, ushort uBlkSize)
{
	s3c_hsmmc_writew((uDmaBufBoundary << 12) | (uBlkSize), HM_BLKSIZE);
}

static void ClearErrInterruptStatus(void)
{
	while (s3c_hsmmc_readw(HM_NORINTSTS) & (0x1 << 15)) {
		s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTS), HM_NORINTSTS);
		s3c_hsmmc_writew(s3c_hsmmc_readw(HM_ERRINTSTS), HM_ERRINTSTS);
	}
}

static void InterruptEnable(ushort NormalIntEn, ushort ErrorIntEn)
{
	ClearErrInterruptStatus();
	s3c_hsmmc_writew(NormalIntEn, HM_NORINTSTSEN);
	s3c_hsmmc_writew(ErrorIntEn, HM_ERRINTSTSEN);
}

static void hsmmc_clock_onoff (int on)
{
	ushort reg16;

	if (on == 0) {
		reg16 = s3c_hsmmc_readw(HM_CLKCON) & ~(0x1<<2);
		s3c_hsmmc_writew(reg16, HM_CLKCON);
	} else {
		reg16 = s3c_hsmmc_readw(HM_CLKCON);
		s3c_hsmmc_writew(reg16 | (0x1<<2), HM_CLKCON);

		while (1) {
			reg16 = s3c_hsmmc_readw(HM_CLKCON);
			if (reg16 & (0x1<<3))	/*  SD_CLKSRC is Stable */
				break;
		}
	}
}

static void set_clock (uint clksrc, uint div)
{
	ushort reg16;
	uint i;

#if defined(CONFIG_S3C6400)
	s3c_hsmmc_writel(0xC0000100 | (clksrc << 4), HM_CONTROL2);	// feedback control off
	s3c_hsmmc_writel(0x00000000, HM_CONTROL3);
#else
	s3c_hsmmc_writel(0xC0004100 | (clksrc << 4), HM_CONTROL2);	// rx feedback control
	s3c_hsmmc_writel(0x00008080, HM_CONTROL3); 			// Low clock: 00008080
	s3c_hsmmc_writel(0x3 << 16, HM_CONTROL4);
#endif

	s3c_hsmmc_writew(s3c_hsmmc_readw(HM_CLKCON) & ~(0xff << 8), HM_CLKCON);

	/* SDCLK Value Setting + Internal Clock Enable */
	s3c_hsmmc_writew(((div<<8) | 0x1), HM_CLKCON);

	/* CheckInternalClockStable */
	for (i=0; i<0x10000; i++) {
		reg16 = s3c_hsmmc_readw(HM_CLKCON);
		if (reg16 & 0x2)
			break;
	}
	if (i == 0x10000)
		printf("internal clock stabilization failed\n");

	dbg("HM_CONTROL2(0x80) = 0x%08x\n", s3c_hsmmc_readl(HM_CONTROL2));
	dbg("HM_CONTROL3(0x84) = 0x%08x\n", s3c_hsmmc_readl(HM_CONTROL3));
	dbg("HM_CLKCON  (0x2c) = 0x%04x\n", s3c_hsmmc_readw(HM_CLKCON));

	hsmmc_clock_onoff(1);
}

static void set_cmd_register (ushort cmd, uint data, uint flags)
{
	ushort val = (cmd << 8);

	if (cmd == 12)
		val |= (3 << 6);

	if (flags & MMC_RSP_136)	/* Long RSP */
		val |= 0x01;
	else if (flags & MMC_RSP_BUSY)	/* R1B */
		val |= 0x03;
	else if (flags & MMC_RSP_PRESENT)	/* Normal RSP */
		val |= 0x02;

	if (flags & MMC_RSP_OPCODE)
		val |= (1<<4);

	if (flags & MMC_RSP_CRC)
		val |= (1<<3);

	if (data)
		val |= (1<<5);

	dbg("cmdreg =  0x%04x\n", val);
	s3c_hsmmc_writew(val, HM_CMDREG);
}

static int issue_command (ushort cmd, uint arg, uint data, uint flags)
{
	int i;

	dbg("### issue_command: %d, %08x, %d, %08x\n", cmd, arg, data, flags);
	/* Check CommandInhibit_CMD */
	for (i=0; i<0x1000000; i++) {
		if (!(s3c_hsmmc_readl(HM_PRNSTS) & 0x1))
			break;
	}
	if (i == 0x1000000) {
		printf("@@@@@@1 rHM_PRNSTS: %08lx\n", s3c_hsmmc_readl(HM_PRNSTS));
	}

	/* Check CommandInhibit_DAT */
	if (flags & MMC_RSP_BUSY) {
		for (i=0; i<0x1000000; i++) {
			if (!(s3c_hsmmc_readl(HM_PRNSTS) & 0x2))
				break;
		}
		if (i == 0x1000000) {
			printf("@@@@@@2 rHM_PRNSTS: %08lx\n", s3c_hsmmc_readl(HM_PRNSTS));
		}
	}

	s3c_hsmmc_writel(arg, HM_ARGUMENT);

	set_cmd_register(cmd, data, flags);

	if (wait_for_cmd_done())
		return 0;

	ClearCommandCompleteStatus();

	if (!(s3c_hsmmc_readw(HM_NORINTSTS) & 0x8000)) {
		return 1;
	} else {
		if (ocr_check == 1)
			return 0;
		else {
			printf("Command = %d, Error Stat = 0x%04x\n", (s3c_hsmmc_readw(HM_CMDREG) >> 8), s3c_hsmmc_readw(HM_ERRINTSTS));
			return 0;
		}
	}

}

static void set_mmc_speed (uint eSDSpeedMode)
{
	uint ucSpeedMode, arg;

	ucSpeedMode = (eSDSpeedMode == HIGH) ? 1 : 0;
	arg = (3 << 24) | (185 << 16) | (ucSpeedMode << 8);	/*  Change to the high-speed mode */
	while (!issue_command(MMC_SWITCH, arg, 0, MMC_RSP_R1B));
}

static void set_sd_speed (uint eSDSpeedMode)
{
	uint temp;
	uint arg = 0;
	uchar ucSpeedMode;
	int i;

	ucSpeedMode = (eSDSpeedMode == HIGH) ? 1 : 0;

	if (!issue_command(MMC_SET_BLOCKLEN, 64, 0, MMC_RSP_R1)) {
		printf("CMD16 fail\n");
	} else {
		set_blksize_register(7, 64);
		set_blkcnt_register(1);
		set_arg_register(0 * 64);

		set_transfer_mode_register(0, 1, 0, 0, 0);

		arg = (0x1 << 31) | (0xffff << 8) | (ucSpeedMode << 0);

		if (!issue_command(MMC_SWITCH, arg, 0, MMC_RSP_R1B))
			printf("CMD6 fail\n");
		else {
			wait_for_r_buf_ready();
			ClearBufferReadReadyStatus();

			for (i = 0; i < 16; i++) {
				temp = s3c_hsmmc_readl(HM_BDATA);
			}

			if (!wait_for_data_done())
				printf(("Transfer NOT Complete\n"));

			ClearTransferCompleteStatus();
		}
	}
}

static int get_sd_scr (void)
{
	uint uSCR1, uSCR2;

	if (!issue_command(MMC_SET_BLOCKLEN, 8, 0, MMC_RSP_R1))
		return 0;
	else {
		set_blksize_register(7, 8);
		set_blkcnt_register(1);
		set_arg_register(0 * 8);

		set_transfer_mode_register(0, 1, 0, 0, 0);
		if (!issue_command(MMC_APP_CMD, rca<<16, 0, MMC_RSP_R1))
			return 0;
		else {
			if (!issue_command(SD_APP_SEND_SCR, 0, 1, MMC_RSP_R1))
				return 0;
			else {
				wait_for_r_buf_ready();
				ClearBufferReadReadyStatus();

				uSCR1 = s3c_hsmmc_readl(HM_BDATA);
				uSCR2 = s3c_hsmmc_readl(HM_BDATA);

				if (!wait_for_data_done())
					printf(("Transfer NOT Complete\n"));

				ClearTransferCompleteStatus();

				if (uSCR1 & 0x1)
					sd_spec = 1;	/*  Version 1.10, support cmd6 */
				else
					sd_spec = 0;	/*  Version 1.0 ~ 1.01 */

				dbg("sd_spec = %d(0x%08x)\n", sd_spec, uSCR1);
				return 1;
			}
		}
	}
}

static int check_card_status(void)
{
	if (!issue_command(MMC_SEND_STATUS, rca<<16, 0, MMC_RSP_R1)) {
		return 0;
	} else {
		if (((s3c_hsmmc_readl(HM_RSPREG0) >> 9) & 0xf) == 4) {
			dbg("Card is transfer status\n");
			return 1;
		}
	}

	return 1;
}

static void set_hostctl_speed (uchar mode)
{
	uchar reg8;

	reg8 = s3c_hsmmc_readb(HM_HOSTCTL) & ~(0x1<<2);
	s3c_hsmmc_writeb(reg8 | (mode<<2), HM_HOSTCTL);
}

/* return 0: OK
 * return -1: error
 */
static int set_bus_width (uint width)
{
	uint arg = 0;
	uchar reg = s3c_hsmmc_readb(HM_HOSTCTL);
	uchar bitmode = 0;

	dbg("bus width: %d\n", width);

	switch (width) {
	case 8:
		width = mmc_card ? 8 : 4;
		break;
	case 4:
	case 1:
		break;
	default:
		return -1;
	}

	card_irq_enable(0);	// Disable sd card interrupt

	if (mmc_card) { /* MMC Card */
		/* MMC Spec 4.x or above */
		if (mmc_spec == 4) {
			if (width == 1) bitmode = 0;
			else if (width == 4) bitmode = 1;
			else if (width == 8) bitmode = 2;
			else {
				printf("#### unknown mode\n");
				return -1;
			}

			arg = ((3 << 24) | (183 << 16) | (bitmode << 8));
			while (!issue_command(MMC_SWITCH, arg, 0, MMC_RSP_R1B));
		} else
			bitmode = 0;
	} else { /* SD Card */
		if (!issue_command(MMC_APP_CMD, rca<<16, 0, MMC_RSP_R1))
			return -1;
		else {
			if (width == 1) {	// 1-bits
				bitmode = 0;
				if (!issue_command(MMC_SWITCH, 0, 0, MMC_RSP_R1B))
					return -1;
			} else {	// 4-bits
				bitmode = 1;
				if (!issue_command(MMC_SWITCH, 2, 0, MMC_RSP_R1B))
					return -1;
			}
		}
	}

	if (bitmode == 2)
		reg |= 1 << 5;
	else
		reg |= bitmode << 1;

	s3c_hsmmc_writeb(reg, HM_HOSTCTL);
	card_irq_enable(1);
	dbg(" transfer rHM_HOSTCTL(0x28) = 0x%02x\n", s3c_hsmmc_readb(HM_HOSTCTL));

	return 0;
}

static int set_sd_ocr (void)
{
	uint i, ocr;

	issue_command(MMC_APP_CMD, 0x0, 0, MMC_RSP_R1);
	issue_command(SD_APP_OP_COND, 0x0, 0, MMC_RSP_R3);
	ocr = s3c_hsmmc_readl(HM_RSPREG0);
	dbg("ocr1: %08x\n", ocr);

	for (i = 0; i < 250; i++) {
		issue_command(MMC_APP_CMD, 0x0, 0, MMC_RSP_R1);
		issue_command(SD_APP_OP_COND, ocr, 0, MMC_RSP_R3);

		ocr = s3c_hsmmc_readl(HM_RSPREG0);
		dbg("ocr2: %08x\n", ocr);
		if (ocr & (0x1 << 31)) {
			dbg("Voltage range: ");
			if (ocr & (1 << 21))
				dbg("2.7V ~ 3.4V");
			else if (ocr & (1 << 20))
				dbg("2.7V ~ 3.3V");
			else if (ocr & (1 << 19))
				dbg("2.7V ~ 3.2V");
			else if (ocr & (1 << 18))
				dbg("2.7V ~ 3.1V");

			if (ocr & (1 << 7))
				dbg(", 1.65V ~ 1.95V\n");
			else
				dbg('\n');

			mmc_card = 0;
			return 1;
		}
		udelay(1000);
	}

	// The current card is MMC card, then there's time out error, need to be cleared.
	ClearErrInterruptStatus();
	return 0;
}

static int set_mmc_ocr (void)
{
	uint i, ocr;

	for (i = 0; i < 250; i++) {
		issue_command(MMC_SEND_OP_COND, 0x40FF8000, 0, MMC_RSP_R3);

		ocr = s3c_hsmmc_readl(HM_RSPREG0);
		dbg("ocr1: %08x\n", ocr);

		if (ocr & (0x1 << 31)) {
			dbg("Voltage range: ");
			if (ocr & (1 << 21))
				dbg("2.7V ~ 3.4V");
			else if (ocr & (1 << 20))
				dbg("2.7V ~ 3.3V");
			else if (ocr & (1 << 19))
				dbg("2.7V ~ 3.2V");
			else if (ocr & (1 << 18))
				dbg("2.7V ~ 3.1V");
			mmc_card = 1;
			if (ocr & (1 << 7))
				dbg(", 1.65V ~ 1.95V\n");
			else
				dbg('\n');
			return 1;
		}
	}

	// The current card is SD card, then there's time out error, need to be cleared.
	ClearErrInterruptStatus();
	return 0;
}

#ifdef CONFIG_SUPPORT_MMC_PLUS

static void clock_config (uint clksrc, uint div)
{
	uint freq, base_freq = 0;

	if (clksrc == SD_HCLK)
		base_freq = HCLK;
	else if (clksrc == SD_EPLL)	/* Epll Out 48MHz */
		base_freq = 100000000;
	else
		clksrc = HCLK;

	freq = base_freq / (div * 2);
	printf("         Freq = %dMHz\n", freq / (1000000));


	/* When Higher than 25MHz, it is necessary
	 * to enable high speed mode of the host controller.
	 */
	if (freq > 25000000) {
		set_hostctl_speed(HIGH);
	} else {
		set_hostctl_speed(NORMAL);
	}

	/* when change the sd clock frequency, need to stop sd clock. */
	hsmmc_clock_onoff(1);
	set_clock(clksrc, div);
	dbg("clock config rHM_HOSTCTL(0x28) = 0x%02x\n", s3c_hsmmc_readb(HM_HOSTCTL));

}
#else

static void clock_config (uint clksrc, uint Divisior)
{
	uint SrcFreq, WorkingFreq;

	if (clksrc == SD_HCLK)
		base_freq = HCLK;
	else if (clksrc == SD_EPLL)	/* Epll Out 48MHz */
		base_freq = 100000000;
	else
		clksrc = HCLK;

	WorkingFreq = SrcFreq / (Divisior * 2);
	printf("Card Working Frequency = %dMHz\n", WorkingFreq / (1000000));

	if (mmc_card) {
		if (mmc_spec == 4) {
			if (WorkingFreq > 20000000) {	// It is necessary to enable the high speed mode in the card before changing the clock freq to a freq higher than 20MHz.
				set_mmc_speed(HIGH);
				printf("\nSet MMC High speed mode OK!!\n");
			} else {
				set_mmc_speed(NORMAL);
				printf("\nSet MMC Normal speed mode OK!!\n");
			}
		} else		// old version
			printf("Old version MMC card can not support working frequency higher than 25MHz");
	}

	if (WorkingFreq > 25000000) {	// Higher than 25MHz, it is necessary to enable high speed mode of the host controller.
		set_hostctl_speed(HIGH);
	} else {
		set_hostctl_speed(NORMAL);
	}

	hsmmc_clock_onoff(0);		// when change the sd clock frequency, need to stop sd clock.
	set_clock(clksrc, Divisior);
	printf("clock config rHM_HOSTCTL = 0x%02x\n", s3c_hsmmc_readb(HM_HOSTCTL));

}
#endif

static void check_dma_int (void)
{
	HS_DMA_END = 0;
#if 0
	for (i = 0; i < 5000000; i++) {
		udelay(1);//5s timeout
#else
	while(1){
#endif
		if (s3c_hsmmc_readw(HM_NORINTSTS) & 0x0008) {
			dbg("DMA interrupt\n");
			HS_DMA_END = 1;
			s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTS) | 0x0008, HM_NORINTSTS);
			goto ret;
		}

		if (s3c_hsmmc_readw(HM_NORINTSTS) & 0x0002) {
			dbg("Transfer Complete\n");
			HS_DMA_END = 2;
			s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTS) | 0x0002, HM_NORINTSTS);
			goto ret;
		}

		if (s3c_hsmmc_readw(HM_NORINTSTS) & 0x8000) {
			printf("error found: %04x\n", s3c_hsmmc_readw(HM_ERRINTSTS));
			HS_DMA_END = -1;
			goto ret;
		}
	}

	printf("ERROR: DMA interrupt timeout\n");
ret:
	return;
}

static uint process_ext_csd (void)
{
	uchar ext_csd[512];

	memset(ext_csd, 0, sizeof(ext_csd));

	if (ext_csd >= (uchar *)0xc0000000)
		SetSystemAddressReg(virt_to_phys((ulong)ext_csd));
	else
		SetSystemAddressReg((ulong)ext_csd);

	set_blksize_register(7, 512);
	set_blkcnt_register(1);
	set_transfer_mode_register(0, 1, 0, 1, 1);

	while(!issue_command(MMC_SEND_EXT_CSD, 0, 1, MMC_RSP_R1 | MMC_CMD_ADTC));

	check_dma_int();
	while (!HS_DMA_END);

	return (((ext_csd[215] << 24) | (ext_csd[214] << 16) | (ext_csd[213] << 8) | ext_csd[212]) / (2 * 1024));
}

static void print_sd_cid(const struct sd_cid *cid)
{
	switch (card_type) {
	case CARDTYPE_NONE:
		puts("(None) / ");
		break;
	case CARDTYPE_MMC:
		puts("MMC / ");
		break;
	case CARDTYPE_SD:
		puts("SD / ");
		break;
	case CARDTYPE_SD20:
		puts("SD 2.0 / ");
		break;
	case CARDTYPE_SDHC:
		puts("SD 2.0 SDHC / ");
		break;
	}

	printf("Manufacturer: 0x%02X",cid->mid);
	printf(",OEM: \"%c%c/%c%c%c%c%c",cid->oid_0,cid->oid_1,cid->pnm_0,cid->pnm_1,cid->pnm_2,cid->pnm_3,cid->pnm_4);
	printf("\",REV: %d.%d",cid->prv >> 4,cid->prv & 15);
	printf(",S/N: %d",cid->psn_0 << 24 | cid->psn_1 << 16 | cid->psn_2 << 8 | cid->psn_3);
	printf(",DATE: %d/%d",2000 + ((cid->mdt_0 & 15) << 4)+((cid->mdt_1 & 0xf0) >> 4),cid->mdt_1 & 15);
	printf("\n");
}


static void DataRead_ForCompare (int StartAddr)
{
	uint i = 0, j = 0;
	COMPARE_INT_DONE = 0;

	s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSIGEN) & ~(0xffff), HM_NORINTSIGEN);

	Compare_buffer_HSMMC = (uint *) SDI_Compare_buffer_HSMMC;
	for (i = 0; i < (512 * BlockNum_HSMMC) / 4; i++)
		*(Compare_buffer_HSMMC + i) = 0x0;

	dbg("Polling mode data read1\n");
	dbg("Read BlockNum = %d\n", BlockNum_HSMMC);

	while (!check_card_status());

	/*  Maximum DMA Buffer Size, Block Size */
	set_blksize_register(7, 512);
	/*  Block Numbers to Write */
	set_blkcnt_register(BlockNum_HSMMC);

	if (movi_hc)
		set_arg_register(StartAddr);
	else
		set_arg_register(StartAddr * 512);

	if (BlockNum_HSMMC == 1) {
		dbg("Single block read\n");
		set_transfer_mode_register(0, 1, 1, 1, 0);
		/* MMC_READ_SINGLE_BLOCK */
		set_cmd_register(17, 1, MMC_RSP_R1);
	} else {
		dbg("Multi block read\n");
		set_transfer_mode_register(1, 1, 1, 1, 0);
		/* MMC_READ_MULTIPLE_BLOCK */
		set_cmd_register(18, 1, MMC_RSP_R1);
	}

	if (wait_for_cmd_done()) {
		printf("Command is NOT completed1\n");
	}
	ClearCommandCompleteStatus();

	for (j = 0; j < BlockNum_HSMMC; j++) {
		if (!wait_for_r_buf_ready())
			printf("ReadBuffer NOT Ready\n");
		else
			ClearBufferReadReadyStatus();
		for (i = 0; i < 512 / 4; i++) {
			*Compare_buffer_HSMMC++ = s3c_hsmmc_readl(HM_BDATA);
			CompareCnt_INT++;
		}
	}

	dbg("Read count=0x%08x\n", CompareCnt_INT);
	if (!wait_for_data_done()) {
		printf(("Transfer NOT Complete\n"));
	}
	ClearTransferCompleteStatus();

	dbg("\n\nHM_NORINTSTS = %x", s3c_hsmmc_readw(HM_NORINTSTS));
}

static void DataCompare_HSMMC (uint a0, uint a1, uint bytes)
{
	uint *pD0 = (uint *) a0;
	uint *pD1 = (uint *) a1;
	uint ErrCnt = 0;
	uint i;

	for (i = 0; i < bytes; i++) {
		if (*pD0 != *pD1) {
			dbg("\n%08x=%02x <-> %08x=%02x", pD0, *pD0, pD1, *pD1);
			ErrCnt++;
		}
		pD0++;
		pD1++;
	}
	dbg("\nTotal Error cnt = %d", ErrCnt);

	if (ErrCnt == 0)
		dbg("\nData Compare Ok\n");
}

int hsmmc_init (void)
{
	uint reg;
	uint width;
	int resp;
	int hcs;
	int retries = 50;
	uchar response[16];
	unsigned int r1[4];
	struct sd_cid *sd_cid = (struct sd_cid *)response;
	struct mmc_csd *csd = (struct mmc_csd *)response;
	uchar *p8 = (uchar *)&r1[0];
	unsigned int sd_sectors = 0;
	/* we need to shift result by 8 bits spread over 4 x 32-bit regs */
	uchar mangle[] = { 7, 0, 1, 2, 11, 4, 5, 6, 15, 8, 9, 10, 0, 12, 13, 14 };
	int n;

	width = 4;

	HCLK = get_HCLK();
	dbg("HCLK = %08lu\n", HCLK);

	hsmmc_clock_onoff(0);

	reg = readl(SCLK_GATE);
	writel(reg | (1<<27), SCLK_GATE);

	set_clock(SD_EPLL, 0x80);
	s3c_hsmmc_writeb(0xe, HM_TIMEOUTCON);
	set_hostctl_speed(NORMAL);

	InterruptEnable(0xff, 0xff);

	dbg("HM_NORINTSTS = %x\n", s3c_hsmmc_readw(HM_NORINTSTS));

	/* MMC_GO_IDLE_STATE */
	issue_command(MMC_GO_IDLE_STATE, 0x00, 0, 0);

	udelay(100000);
	udelay(100000);
	udelay(100000);
	udelay(100000);

	/* SDHC card? */

	resp = issue_command(SD_SEND_IF_COND, 0x000001aa,
		0, MMC_CMD_BCR | MMC_RSP_R7);
	if (resp && ((s3c_hsmmc_readl(HM_RSPREG0) & 0xff) == 0xaa)) {
		card_type = CARDTYPE_SD20; /* 2.0 SD, may not be SDHC */
		hcs = 0x40000000;
	}

	/* Well, either way let's say hello in SD card protocol */

	while (retries--) {

		udelay(100000);
		udelay(100000);
		udelay(100000);

		resp = issue_command(MMC_APP_CMD, 0x00000000, 0,
			MMC_RSP_R1);
		if (!resp)
			continue;
		resp = issue_command(SD_APP_OP_COND, hcs | 0x00300000, 0,
			MMC_RSP_R3);
		if (!resp)
			continue;

		if ((s3c_hsmmc_readl(HM_RSPREG0) >> 24) & (1 << 6)) { /* asserts block addressing */
			retries = -2;
			card_type = CARDTYPE_SDHC;
		}

		if ((s3c_hsmmc_readl(HM_RSPREG0) >> 24) & (1 << 7)) { /* not busy */
			retries = -2;
			if (card_type == CARDTYPE_NONE)
				card_type = CARDTYPE_SD;
			break;
		}
	}
	if (retries == -1) {
		puts("no response\n");
		return -2;
	}

	if (!issue_command(MMC_ALL_SEND_CID, 0, 0, MMC_RSP_R2)) {
		puts("CID broken\n");
		return -3;
	}

	r1[0] = s3c_hsmmc_readl(HM_RSPREG3);
	r1[1] = s3c_hsmmc_readl(HM_RSPREG2);
	r1[2] = s3c_hsmmc_readl(HM_RSPREG1);
	r1[3] = s3c_hsmmc_readl(HM_RSPREG0);

	for (n = 0; n < 16; n++)
		response[n] = p8[mangle[n]];

	switch (card_type) {
	case CARDTYPE_SD:
	case CARDTYPE_SD20:
	case CARDTYPE_SDHC:

		print_sd_cid(sd_cid);
		resp = issue_command(SD_SEND_RELATIVE_ADDR, MMC_DEFAULT_RCA,
				0, MMC_RSP_R6);
		rca = s3c_hsmmc_readl(HM_RSPREG0) >> 16;
		break;

	default:
		return 1;
	}

	/* grab the CSD */

	resp = issue_command(MMC_SEND_CSD, rca << 16, 0, MMC_RSP_R2);
	if (resp) {

		r1[0] = s3c_hsmmc_readl(HM_RSPREG3);
		r1[1] = s3c_hsmmc_readl(HM_RSPREG2);
		r1[2] = s3c_hsmmc_readl(HM_RSPREG1);
		r1[3] = s3c_hsmmc_readl(HM_RSPREG0);
		for (n = 0; n < 16; n++)
			response[n] = p8[mangle[n]];

		switch (card_type) {
		case CARDTYPE_SDHC:
			puts("         SDHC size: ");
			sd_sectors = (UNSTUFF_BITS(((uint *)&response[0]), 48, 22)
								    + 1) << 10;
			movi_sectors = sd_sectors;
			printf("%d",sd_sectors);
			break;
			
		default:
			puts("         MMC/SD size: ");
#if 0
			sd_sectors = ((((unsigned long)1 << csd->c_size_mult1) *
					(unsigned long)(csd->c_size)) >> 9);
			printf("%d",sd_sectors / 2048);
#else
			uint c_size,c_size_multi,read_bl_len;
			uint card_size;
			read_bl_len = ((r1[1] >> 8) & 0xf);
			c_size_multi = ((r1[2] >> 7) & 0x7);
			c_size = ((r1[1] & 0x3) << 10) | ((r1[2] >> 22) & 0x3ff);
			card_size = (1 << read_bl_len) * (c_size + 1) * (1 << (c_size_multi + 2)) / 1048576;
			printf("%d",card_size);
#endif
			break;
		}
		puts(" Sectors\n");
	} else
		puts("CSD grab broken\n");

	resp = issue_command(MMC_SELECT_CARD, rca<<16, 0, MMC_RSP_R1);
	if (!resp)
		return 1;

	/* Operating Clock setting */
	if (movi_hc){
		clock_config(SD_EPLL, 1);	// Divisor 1 = Base clk /2      ,Divisor 2 = Base clk /4, Divisor 4 = Base clk /8 ...
	}
	else{
		clock_config(SD_EPLL, 2);
	}

	while (set_bus_width(width));
	while (!check_card_status());

	/* MMC_SET_BLOCKLEN */
	while (!issue_command(MMC_SET_BLOCKLEN, 512, 0, MMC_RSP_R1));

	s3c_hsmmc_writew(0xffff, HM_NORINTSTS);

	mmc_dev.if_type = IF_TYPE_MMC;
	mmc_dev.part_type = PART_TYPE_DOS;
	mmc_dev.dev = 0;
	mmc_dev.blksz = MMC_BLOCK_SIZE;
	sprintf(mmc_dev.vendor, "Man %02x %02x %02x Snr %02x %02x %02x", 0x00, 0x01, 0x02, 0x01, 0x02, 0x03);
	sprintf(mmc_dev.product, "%s", "MMC4");
	sprintf(mmc_dev.revision, "%x %x", 0x01, 0x01);
	mmc_dev.block_read = mmc_bread;

	return 0;
}

void movi_write (uint addr, uint start_blk, uint blknum)
{
	uint blksize;

	wt_cnt_HSMMC = 0;
	BlockNum_HSMMC = 0;
	wt_cnt_HSMMC = 0;
	WriteBlockCnt_INT = 0;
	HS_DMA_END = 0;

	BlockNum_HSMMC = blknum;
	blksize = Card_OneBlockSize_ver1;

	s3c_hsmmc_writew((s3c_hsmmc_readw(HM_NORINTSTSEN) & ~(0xffff)) |
		BUFFER_READREADY_STS_INT_EN |
		BUFFER_WRITEREADY_STS_INT_EN |
		TRANSFERCOMPLETE_STS_INT_EN | COMMANDCOMPLETE_STS_INT_EN, HM_NORINTSTSEN);

	s3c_hsmmc_writew((s3c_hsmmc_readw(HM_NORINTSIGEN) & ~(0xffff)) | TRANSFERCOMPLETE_SIG_INT_EN, HM_NORINTSIGEN);

	SetSystemAddressReg(addr);		// AHB System Address For Write
	set_blksize_register(7, 512);		// Maximum DMA Buffer Size, Block Size
	set_blkcnt_register(BlockNum_HSMMC);	// Block Numbers to Write

	if (movi_hc)
		set_arg_register(start_blk);
	else
		set_arg_register(start_blk * 512);

	if (BlockNum_HSMMC == 1) {
		set_transfer_mode_register(0, 0, 1, 1, 1);
		set_cmd_register(24, 1, MMC_RSP_R1);
	} else {
		set_transfer_mode_register(1, 0, 1, 1, 1);
		set_cmd_register(25, 1, MMC_RSP_R1);
	}

	if (wait_for_cmd_done()) {
		printf("\nCommand is NOT completed3\n");
	}
	ClearCommandCompleteStatus();

	/* wait for DMA transfer */
	check_dma_int();
	while (!HS_DMA_END);

	if (!wait_for_data_done()) {
		printf(("Transfer is NOT Complete\n"));
	}
	ClearTransferCompleteStatus();

	s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTS) | (1 << 3), HM_NORINTSTS);

	BlockNum_HSMMC = 0;
	wt_cnt_HSMMC = 0;
	WriteBlockCnt_INT = 0;
	HS_DMA_END = 0;
	BufferBoundary_INT_Cnt = 0;

	CompareCnt_INT = 0;
	Compare_buffer_HSMMC = 0;
}

void movi_read (uint addr, uint start_blk, uint blknum)
{
	uint dma=0,cmd,multi_blk,i,count,spare;

	/*Enable interrupt*/
	s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTSEN) & ~(DMA_STS_INT_EN | BLOCKGAP_EVENT_STS_INT_EN), HM_NORINTSTSEN);
	s3c_hsmmc_writew((HM_NORINTSIGEN & ~(0xffff)) | TRANSFERCOMPLETE_SIG_INT_EN, HM_NORINTSIGEN);

	/*clear interrupt*/
	ClearCommandCompleteStatus();

	ClearDMAStatus();
	ClearTransferCompleteStatus();

	spare = blknum;
	count = ( (blknum * Card_OneBlockSize_ver1) - 1 ) / (Card_OneBlockSize_ver1 * 512) + 1;

	for(i=0;i<count;i++){
		while(!check_card_status());

		SetSystemAddressReg(addr);                           //AHB System Address For Write
		addr += Card_OneBlockSize_ver1 * 512;                //256K ++

		set_blksize_register(6,Card_OneBlockSize_ver1);      //Maximum DMA Buffer Size, Block Size
		set_blkcnt_register((spare > 512) ? 512 : spare);    //Block Numbers to read

		/*Card Start Block Address to read*/
		if (movi_hc){
			set_arg_register(start_blk);
		}else{
			set_arg_register(start_blk*Card_OneBlockSize_ver1);
		}

		start_blk += 512;

		dma = 1;
		multi_blk = (spare > 1);
		cmd = multi_blk ? 18 : 17;

		if(spare>512){
			spare -= 512;
		}

		/*CMD17 signle block, CMD18 mutil block read*/
		set_transfer_mode_register(multi_blk,1,1,multi_blk,dma);
		set_cmd_register(cmd,1,MMC_RSP_R1);

		if (wait_for_cmd_done()){
			printf("ERROR: Command NOT Complete\n");
	        goto error;
		}

		/*wait for transfer complete*/
		check_dma_int();

error:
		ClearCommandCompleteStatus();

		ClearDMAStatus();
		ClearTransferCompleteStatus();
	}

	return;
}

static ulong mmc_bread (int dev_num, ulong blknr, ulong blkcnt, ulong* dst)
{
	/* Note: blknr is NOTHING like blknum! */

	if (dst >= 0xc0000000)
		dst = virt_to_phys(dst);

	if (blkcnt != 0)
		movi_read((uint) dst, (uint) blknr, (uint) blkcnt);

	return blkcnt;
}

static void write_test (uint test, uint start_blk, uint blknum)
{
	uint i, blksize; //j,
	uint uTxBufAddr = SDI_Tx_buffer_HSMMC;
	uint uCompareBufAddr = SDI_Compare_buffer_HSMMC;

	wt_cnt_HSMMC = 0;
	BlockNum_HSMMC = 0;
	wt_cnt_HSMMC = 0;
	WriteBlockCnt_INT = 0;
	HS_DMA_END = 0;

	printf("\nHS-MMC block Write test: %d, 0x%x 0x%x\n", test, start_blk, blknum);

	BlockNum_HSMMC = blknum;

	blksize = Card_OneBlockSize_ver1;

	Tx_buffer_HSMMC = (uint *) SDI_Tx_buffer_HSMMC;
	for (i = 0; i < (blksize * BlockNum_HSMMC) / 4; i++) {
		*(Tx_buffer_HSMMC + i) = ((start_blk<<16) + i);
	}

	printf("## using DMA\n");
	s3c_hsmmc_writew((s3c_hsmmc_readw(HM_NORINTSTSEN) & ~(0xffff)) |
		BUFFER_READREADY_STS_INT_EN |
		BUFFER_WRITEREADY_STS_INT_EN |
		TRANSFERCOMPLETE_STS_INT_EN | COMMANDCOMPLETE_STS_INT_EN, HM_NORINTSTSEN);

	s3c_hsmmc_writew((s3c_hsmmc_readw(HM_NORINTSIGEN) & ~(0xffff)) | TRANSFERCOMPLETE_SIG_INT_EN, HM_NORINTSIGEN);

	SetSystemAddressReg(SDI_Tx_buffer_HSMMC);	// AHB System Address For Write
	set_blksize_register(7, 512);	// Maximum DMA Buffer Size, Block Size
	set_blkcnt_register(BlockNum_HSMMC);	// Block Numbers to Write

	if (movi_hc)
		set_arg_register(start_blk);		// Card Start Block Address to Write
	else
		set_arg_register(start_blk * 512);	// Card Start Block Address to Write

	if (BlockNum_HSMMC == 1) {
		set_transfer_mode_register(0, 0, 1, 1, 1);
		set_cmd_register(24, 1, MMC_RSP_R1);
	} else {
		set_transfer_mode_register(1, 0, 1, 1, 1);
		set_cmd_register(25, 1, MMC_RSP_R1);
	}

	if (wait_for_cmd_done()) {
		printf("\nCommand is NOT completed3\n");
	}
	ClearCommandCompleteStatus();

	/* wait for DMA transfer */
	check_dma_int();
	while (!HS_DMA_END);

	if (!wait_for_data_done()) {
		printf(("Transfer is NOT Complete\n"));
	}
	ClearTransferCompleteStatus();

	s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTS) | (1 << 3), HM_NORINTSTS);

	printf(("\nDMA Write End\n"));

	DataRead_ForCompare(start_blk);
	DataCompare_HSMMC(uTxBufAddr, uCompareBufAddr, BlockNum_HSMMC * 128);

	BlockNum_HSMMC = 0;
	wt_cnt_HSMMC = 0;
	WriteBlockCnt_INT = 0;
	HS_DMA_END = 0;
	BufferBoundary_INT_Cnt = 0;

	CompareCnt_INT = 0;
	Compare_buffer_HSMMC = 0;
}

static void read_test (uint test, uint start_blk, uint blknum)
{
	uint i, blksize; //j, , Addr_temp = start_blk;
	uint dma = 0, cmd, multi; //, TotalReadByte, read_blk_cnt = 0;

	rd_cnt_HSMMC = 0;
	HS_DMA_END = 0;
	BlockNum_HSMMC = 0;
	rd_cnt_HSMMC = 0;
	ReadBlockCnt_INT = 0;

	printf("\nHS-MMC block Read test: %d, 0x%x 0x%x\n", test, start_blk, blknum);

	BlockNum_HSMMC = blknum;

	blksize = Card_OneBlockSize_ver1;

	Rx_buffer_HSMMC = (uint *) SDI_Rx_buffer_HSMMC;
	for (i = 0; i < (blksize * BlockNum_HSMMC) / 4; i++)
		*(Rx_buffer_HSMMC + i) = 0x0;

	while (!check_card_status());

	s3c_hsmmc_writew(s3c_hsmmc_readw(HM_NORINTSTSEN) & ~(DMA_STS_INT_EN | BLOCKGAP_EVENT_STS_INT_EN), HM_NORINTSTSEN);
	s3c_hsmmc_writew((HM_NORINTSIGEN & ~(0xffff)) | TRANSFERCOMPLETE_SIG_INT_EN, HM_NORINTSIGEN);

	SetSystemAddressReg(SDI_Rx_buffer_HSMMC);	// AHB System Address For Write
	dma = 1;

	set_blksize_register(7, 512);	// Maximum DMA Buffer Size, Block Size
	set_blkcnt_register(BlockNum_HSMMC);	// Block Numbers to Write

	if (movi_hc)
		set_arg_register(start_blk);		// Card Start Block Address to Write
	else
		set_arg_register(start_blk * 512);	// Card Start Block Address to Write

	cmd = (blknum > 1) ? 18 : 17;
	multi = (blknum > 1);

	set_transfer_mode_register(multi, 1, multi, 1, dma);
	set_cmd_register(cmd, 1, MMC_RSP_R1);

	if (wait_for_cmd_done()) {
		printf(("Command NOT Complete\n"));
	} else
		ClearCommandCompleteStatus();


	check_dma_int();
	while (!HS_DMA_END);
	printf(("\nDMA Read End\n"));

	HS_DMA_END = 0;
	BlockNum_HSMMC = 0;
	rd_cnt_HSMMC = 0;
	ReadBlockCnt_INT = 0;
}

void test_hsmmc (uint width, uint test, uint start_blk, uint blknum)
{
	hsmmc_set_gpio();

#if 0
	HS_MMC_CardDetect();
#endif

	hsmmc_reset();

	if (hsmmc_init()) {
		printf("\nCard Initialization FAILED\n");
	}

	memset((ulong *)0x51300000, 0, 0x100000);
	memset((ulong *)0x51000000, 0, 0x100000);
	memset((ulong *)0x51600000, 0, 0x100000);
	write_test(test, start_blk, blknum);
	read_test(test, start_blk, blknum);
}

#endif
