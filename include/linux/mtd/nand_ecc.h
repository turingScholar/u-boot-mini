/*
 *  drivers/mtd/nand_ecc.h
 *
 *  Copyright (C) 2000 Steven J. Hill (sjhill@realitydiluted.com)
 *
 * $Id: nand_ecc.h,v 1.2 2007/02/13 02:11:16 jsgood Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This file is the header for the ECC algorithm.
 */

#ifndef __MTD_NAND_ECC_H__
#define __MTD_NAND_ECC_H__

struct mtd_info;

/*
 * Calculate 3 byte ECC code for 256 byte block
 */
int nand_calculate_ecc(struct mtd_info *mtd, const uchar *dat, uchar *ecc_code);

/*
 * Detect and correct a 1 bit error for 256 byte block
 */
int nand_correct_data(struct mtd_info *mtd, uchar *dat, uchar *read_ecc, uchar *calc_ecc);

#endif /* __MTD_NAND_ECC_H__ */
