/*
 * $Id: nand_cp.c,v 1.4 2008/04/08 00:41:40 jsgood Exp $
 *
 * (C) Copyright 2006 Samsung Electronics
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

#ifdef CONFIG_S3C64XX
#include <nand.h>
#include <s3c6410.h>


static int nandll_read_page (uchar *buf, unsigned long addr, int large_block)
{
       int i;
	int page_size = 512;

	if (large_block==1)
 		page_size = 2048;
	if (large_block==2)
		page_size = 4096;

        NAND_ENABLE_CE();

        NFCMD_REG = NAND_CMD_READ;

        /* Write Address */
        NFADDR_REG = 0;

	if (large_block)
	        NFADDR_REG = 0;

	NFADDR_REG = (addr) & 0xff;
	NFADDR_REG = (addr >> 8) & 0xff;
	NFADDR_REG = (addr >> 16) & 0xff;

	if (large_block)
		NFCMD_REG = NAND_CMD_READSTART;

	NAND_TRANS_RNB();
	
	/* for compatibility(2460). u32 cannot be used. by scsuh */
	for(i=0; i < page_size; i++) {
                *buf++ = NFDATA8_REG;
        }

        NAND_DISABLE_CE();
        return 0;
}

/*
 * Read data from NAND.
 */
static int nandll_read_blocks (unsigned long dst_addr, unsigned long size, int large_block)
{
        uchar *buf = (uchar *)dst_addr;
        int i;
	unsigned int page_shift = 9;

	if (large_block==1)
		page_shift = 11;

        /* Read pages */
	if(large_block==2)
		page_shift = 12;
 
	if(large_block == 2)
	{
		/* Read pages */
		for (i = 0; i < 4; i++, buf+=(1<<(page_shift-1))) {
		        nandll_read_page(buf, i, large_block);
		}


		/* Read pages */
		for (i = 4; i < (0x3c000>>page_shift); i++, buf+=(1<<page_shift)) {
		        nandll_read_page(buf, i, large_block);
		}
	}
	else
	{
		for (i = 0; i < (0x3c000>>page_shift); i++, buf+=(1<<page_shift)) {
		        nandll_read_page(buf, i, large_block);
		}
	}

        return 0;
}

int copy_uboot_to_ram (void)
{
	int large_block = 0;
	int i;
	volatile unsigned char id;
	
        NAND_ENABLE_CE();
        NFCMD_REG = NAND_CMD_READ_ID;
        NFADDR_REG =  0x00;

	/* wait for a while */
        for (i=0; i<200; i++);
	id = NFDATA8_REG;
	id = NFDATA8_REG;

	if (id > 0x80)
		large_block = 1;
	if(id == 0xd5)
		large_block = 2;


	/* read NAND Block.
	 * 128KB ->240KB because of U-Boot size increase. by scsuh
	 * So, read 0x3c000 bytes not 0x20000(128KB).
	 */
	return nandll_read_blocks(CFG_PHY_UBOOT_BASE, 0x3c000, large_block);
}

#endif

