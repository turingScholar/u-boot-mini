#ifndef __NAND_H
#define __NAND_H

#include <common.h>

/* Initial parameter */

#define TACLS		3
#define TWRPH0		3
#define TWRPH1		3


/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ				0
#define NAND_CMD_READ_START		0x30

#define NAND_CMD_READ_ID			0x90
#define NAND_CMD_STATUS			0x70
#define NAND_CMD_RESET			0xff


#define NAND_DISABLE_CE()	(NFCONT_REG |= (1 << 1))
#define NAND_ENABLE_CE()	(NFCONT_REG &= ~(1 << 1))

#define NAND_CMD(cmd)		(NFCMD_REG = (cmd) & 0xff )
#define NAND_ADDR(addr)		(NFADDR_REG = (addr) & 0xff )
#define NAND_GET_BYTE()	(NFDATA8_REG)
#define NAND_GET_LONG()	(NFDATA_REG)
#define NAND_TRANS_RNB()	do { while(!(NFSTAT_REG & 1)); } while(0)




#define NAND_MAX_OOBSIZE	128
#define NAND_MAX_PAGESIZE	4096


#define NAND_NCE		0x01
#define NAND_CLE		0x02
#define NAND_ALE		0x04

#define NAND_CTRL_CLE		(NAND_NCE | NAND_CLE)
#define NAND_CTRL_ALE		(NAND_NCE | NAND_ALE)
#define NAND_CTRL_CHANGE	0x80



/* Extended commands for large page devices */
#define NAND_CMD_READSTART	0x30
#define NAND_CMD_RNDOUTSTART	0xE0
#define NAND_CMD_CACHEDPROG	0x15


#define NAND_CMD_DEPLETE1	0x100
#define NAND_CMD_DEPLETE2	0x38
#define NAND_CMD_STATUS_MULTI	0x71
#define NAND_CMD_STATUS_ERROR	0x72


/* multi-bank error status (banks 0-3) */
#define NAND_CMD_STATUS_ERROR0	0x73
#define NAND_CMD_STATUS_ERROR1	0x74
#define NAND_CMD_STATUS_ERROR2	0x75
#define NAND_CMD_STATUS_ERROR3	0x76
#define NAND_CMD_STATUS_RESET	0x7f
#define NAND_CMD_STATUS_CLEAR	0xff

#define NAND_CMD_NONE		-1

#endif
