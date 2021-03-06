/*
 * (C) Copyright 2006 OpenMoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
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

#include <nand.h>
#include <asm/io.h>

#include "s5pv210_nand.h"

#define S5PV210_NFCONT_EN(x)    ((x)<<0)
#define S5PV210_NFCONT_CE(x)    ((x)<<1)

#define S5PV210_NFCONF_INITECC     (1<<12)

#define S5PV210_NFCONF_TACLS(x)    ((x)<<12)
#define S5PV210_NFCONF_TWRPH0(x)   ((x)<<8)
#define S5PV210_NFCONF_TWRPH1(x)   ((x)<<4)

#define	SP5V210_NFCONF_MLC(x)    ((x)<<3)
#define	SP5V210_NFCONF_PAGE_SIZE(x)    ((x)<<2)
#define	SP5V210_NFCONF_ADDR_CYCLE(x)    ((x)<<1)

#define	CONFIG_NAND_SPL

#ifdef CONFIG_NAND_SPL

/* in the early stage of NAND flash booting, printf() is not available */
#define printf(fmt, args...)

static void s5pv210_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	int i;
	struct nand_chip *this = mtd->priv;

	for (i = 0; i < len; i++)
		buf[i] = readb(this->IO_ADDR_R);
}

/**
 * s5pv210_nand_write_buf - [Intern] write buffer to chip
 * @mtd:	MTD device structure
 * @buf:	data buffer
 * @len:	number of bytes to write
 *
 * write function for 8bit buswith
 */
static void s5pv210_nand_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	int i;
	struct nand_chip *chip = mtd->priv;
	struct s5pv210_nand *nand = s5pv210_get_base_nand();
	chip->IO_ADDR_W = &nand->nfdata;

	for (i = 0; i < len; i++)
		writeb(buf[i], chip->IO_ADDR_W);
}
#endif

static void s5pv210_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;
	struct s5pv210_nand *nand = s5pv210_get_base_nand();

	debug("hwcontrol(): 0x%02x 0x%02x\n", cmd, ctrl);

	if (ctrl & NAND_CTRL_CHANGE) {

		if (ctrl & NAND_CLE)
			chip->IO_ADDR_W = &nand->nfcmd;
		if (ctrl & NAND_ALE)
			chip->IO_ADDR_W = &nand->nfaddr;

		if (ctrl & NAND_NCE)
			writel(readl(&nand->nfcont) & ~S5PV210_NFCONT_CE(1), &nand->nfcont);
		else
			writel(readl(&nand->nfcont) | S5PV210_NFCONT_CE(1), &nand->nfcont);
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, chip->IO_ADDR_W);
}

static void s5pv210_nand_select_chip(struct mtd_info *mtd, int chipnr)
{
	struct nand_chip *chip = mtd->priv;
	struct s5pv210_nand *nand = s5pv210_get_base_nand();

	switch (chipnr) {
	case -1:
		writel(readl(&nand->nfcont) | S5PV210_NFCONT_CE(1), &nand->nfcont);
		break;
	case 0:
		writel(readl(&nand->nfcont) & ~S5PV210_NFCONT_CE(1), &nand->nfcont);
		break;

	default:
		BUG();
	}
}

static int s5pv210_dev_ready(struct mtd_info *mtd)
{
	struct s5pv210_nand *nand = s5pv210_get_base_nand();
	debug("dev_ready\n");
	return readl(&nand->nfstat) & 0x01;
}

#ifdef CONFIG_S5PV210_NAND_HWECC
void s5pv210_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	struct s5pv210_nand *nand = s5pv210_get_base_nand();
	debug("s5pv210_nand_enable_hwecc(%p, %d)\n", mtd, mode);
	writel(readl(&nand->nfconf) | S5PV210_NFCONF_INITECC, &nand->nfconf);
}

static int s5pv210_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat,
				      u_char *ecc_code)
{
	struct s5pv210_nand *nand = s5pv210_get_base_nand();
	ecc_code[0] = readb(&nand->nfecc);
	ecc_code[1] = readb(&nand->nfecc + 1);
	ecc_code[2] = readb(&nand->nfecc + 2);
	debug("s5pv210_nand_calculate_hwecc(%p,): 0x%02x 0x%02x 0x%02x\n",
	       mtd , ecc_code[0], ecc_code[1], ecc_code[2]);

	return 0;
}

static int s5pv210_nand_correct_data(struct mtd_info *mtd, u_char *dat,
				     u_char *read_ecc, u_char *calc_ecc)
{
	if (read_ecc[0] == calc_ecc[0] &&
	    read_ecc[1] == calc_ecc[1] &&
	    read_ecc[2] == calc_ecc[2])
		return 0;

	printf("s5pv210_nand_correct_data: not implemented\n");
	return -1;
}
#endif

int board_nand_init(struct nand_chip *nand)
{
	u_int32_t cfg;
	u_int8_t tacls, twrph0, twrph1;
//	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();
	struct s5pv210_nand *nand_reg = s5pv210_get_base_nand();

	debug("board_nand_init()\n");

//	writel(readl(&clk_power->clkcon) | (1 << 4), &clk_power->clkcon);

	/* initialize hardware */
#if defined(CONFIG_S5PV210_CUSTOM_NAND_TIMING)
	tacls  = CONFIG_S5PV210_TACLS;
	twrph0 = CONFIG_S5PV210_TWRPH0;
	twrph1 =  CONFIG_S5PV210_TWRPH1;
#else
	tacls = 1;
	twrph0 = 5;
	twrph1 = 2;
#endif

#define MP0_1CON 		( *((volatile unsigned long *)0xE02002E0) )
#define MP0_2CON 		( *((volatile unsigned long *)0xE0200300) )
#define MP0_3CON 		( *((volatile unsigned long *)0xE0200320) )
	/* configure the pins function to nand NFCSn */
	MP0_1CON = 0x22333322;
	MP0_2CON = 0x00002222;
	MP0_3CON = 0x22222222;
	
	/* configuration register */
	cfg |= S5PV210_NFCONF_TACLS(tacls);
	cfg |= S5PV210_NFCONF_TWRPH0(twrph0 - 1);
	cfg |= S5PV210_NFCONF_TWRPH1(twrph1 - 1);
	cfg |= SP5V210_NFCONF_MLC(1);
	cfg |= SP5V210_NFCONF_PAGE_SIZE(1);
	cfg |= SP5V210_NFCONF_ADDR_CYCLE(1);
	writel(cfg, &nand_reg->nfconf);

	/* control register */
	cfg = S5PV210_NFCONT_CE(0x3);
	cfg |= S5PV210_NFCONT_EN(1);
	writel(cfg, &nand_reg->nfcont);

	/* initialize nand_chip data structure */
	nand->IO_ADDR_R = (void *)&nand_reg->nfdata;
	nand->IO_ADDR_W = (void *)&nand_reg->nfdata;

	nand->select_chip = s5pv210_nand_select_chip;

	/* read_buf and write_buf are default */
	/* read_byte and write_byte are default */
#ifdef CONFIG_NAND_SPL
	nand->read_buf = s5pv210_nand_read_buf;
	nand->write_buf = s5pv210_nand_write_buf;
#endif

	/* hwcontrol always must be implemented */
	nand->cmd_ctrl = s5pv210_hwcontrol;

	nand->dev_ready = s5pv210_dev_ready;

#ifdef CONFIG_S5PV210_NAND_HWECC
	nand->ecc.hwctl = s5pv210_nand_enable_hwecc;
	nand->ecc.calculate = s5pv210_nand_calculate_ecc;
	nand->ecc.correct = s5pv210_nand_correct_data;
	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.size = CONFIG_SYS_NAND_ECCSIZE;
	nand->ecc.bytes = CONFIG_SYS_NAND_ECCBYTES;
#else
	nand->ecc.mode = NAND_ECC_SOFT;
#endif

#ifdef CONFIG_S5PV210_NAND_BBT
	nand->options = NAND_USE_FLASH_BBT;
#else
	nand->options = 0;
#endif

	debug("end of nand_init\n");

	return 0;
}
