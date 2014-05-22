/*
 * Copyright (C) 2009 Samsung Electronics
 * Kyungmin Park <kyungmin.park@samsung.com>
 * Minkyu Kang <mk7.kang@samsung.com>
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

#include <config.h>
#include <version.h>
#include <asm/arch/cpu.h>
#include <asm/arch/power.h>

/*
 * Register usages:
 *
 * r5 has zero always
 */

_TEXT_BASE:
	.word	CONFIG_SYS_TEXT_BASE

	.globl lowlevel_init
lowlevel_init:
	mov	r9, lr	

	/* r5 has always zero */
	mov	r5, #0

	ldr	r8, =S5PV210_GPIO_BASE			@0xE0200000

	/* Disable Watchdog */
	ldr	r0, =S5PV210_WATCHDOG_BASE		@0xEA200000
	orr	r0, r0, #0x0
	str	r5, [r0]

	/* setting SRAM */
	ldr	r0, =S5PV210_SROMC_BASE
	ldr	r1, =0x9
	str	r1, [r0]

	/* S5PV210 has 3 groups of interrupt sources */
	ldr	r0, =S5PV210_VIC0_BASE			@0xF2000000
	ldr	r1, =S5PV210_VIC1_BASE			@0xF2100000
	ldr	r2, =S5PV210_VIC2_BASE			@0xF2200000
	ldr	r3, =S5PV210_VIC3_BASE			@0xF2300000

	/* Disable all interrupts (VIC0, VIC1 and VIC2) */
	mvn	r4, #0x0
	str	r4, [r0, #0x14]				@INTENCLEAR
	str	r4, [r1, #0x14]				@INTENCLEAR
	str	r4, [r2, #0x14]				@INTENCLEAR
	str	r4, [r3, #0x14]				@INTENCLEAR

	/* Set all interrupts as IRQ */
	str	r5, [r0, #0xc]				@INTSELECT
	str	r5, [r1, #0xc]				@INTSELECT
	str	r5, [r2, #0xc]				@INTSELECT
	str	r5, [r3, #0xc]				@INTSELECT

	/* Pending Interrupt Clear */
	str	r5, [r0, #0xf00]			@INTADDRESS
	str	r5, [r1, #0xf00]			@INTADDRESS
	str	r5, [r2, #0xf00]			@INTADDRESS
	str	r5, [r3, #0xf00]			@INTADDRESS

	/* for UART */
	bl uart_asm_init

	/* for TZPC */
//	bl tzpc_asm_init

	/* init system clock */
//	bl system_clock_init
	bl	init_clock

	/* Memory initialize */
//        bl mem_ctrl_asm_init

1:
	mov	lr, r9
	mov	pc, lr

/*
 * system_clock_init: Initialize core clock and bus clock.
 * void system_clock_init(void)
 */
system_clock_init:
	ldr	r8, =S5PV210_CLOCK_BASE		@ 0xE0100000

	/* CLK_SRC0: set mux, using fin 24mhz */
	ldr	r1, =0x0
	str	r1, [r8, #0200]	

	/* CLK_DIV0: Set Clock divider */
	ldr 	r1, [r0, #0x0300]					
	ldr	r2, =0x7fffffff					
	bic	r1, r1, r2
	ldr	r2, =0x14131440						
	orr	r1, r1, r2
	str	r1, [r0, #0x0300]

	/* Set Lock Time */
	ldr	r1, =0xe10			@ Locktime : 0xe10 = 3600
	str	r1, [r8, #0x0000]		@ APLL_LOCK
	str	r1, [r8, #0x0008]		@ MPLL_LOCK
	str	r1, [r8, #0x0010]		@ EPLL_LOCK
	str	r1, [r8, #0x0020]		@ VPLL_LOCK

	/* APLL: FOUT = MDIV*FIN/(PDIV*2^(SDIV-1))=0x7d*24/(0x3*2^(1-1))=1000 MHz */
	@ PDIV 3, MDIV 125, SDIV 1 (1000MHz)	
	ldr	r1, =(1<<31 | 0x7d<<16 | 0x3<<8 | 0x1)		
	str	r1, [r8, #0x100]				@APLL_CON0 

	/* MPLL: FOUT = MDIV*FIN/(PDIV*2^SDIV)=0x29b*24/(0xc*2^1)= 667 MHz */
	@ PDIV 12, MDIV 667, SDIV 1 (667MHz)
	ldr	r1, =(1<<31 | 667<<16 | 12<<8 | 1)		
	str	r1, [r8, #0x108]				@MPLL_CON

	/* EPLL: FOUT = (MDIV + K/65536) X FIN / (PDIV X 2^SDIV) */
	@ PDIV 3, MDIV 48, SDIV 2, K 0 (96MHz)
	ldr	r1, =(1<<31 | 3<<8 | 48<<16 | 2)		
	str	r1, [r8, #0x110]				@EPLL_CON0
	ldr	r1, =0x0
	str	r1, [r8, #0x114]				@EPLL_CON1

	/* VPLL: FOUT = MDIV X FIN / (PDIV X 2^SDIV) */
	@ PDIV 6, MDIV 108, SDIV 3 (54MHz)
	ldr	r1, =(1<<31 | 6<<8 | 108<<16 | 3)		
	str	r1, [r8, #0x120]				@VPLL_CON

	/* Set Source Clock */
	ldr	r1, [r0, #0x200]
	ldr	r2, =0x10001111			@ A, M, E, VPLL Muxing
	orr	r1, r1, r2
	str	r1, [r8, #0x200]		@ CLK_SRC0


	ldr	r1, =0x1000001			@ Uart Clock & CLK48M Muxing
	str	r1, [r8, #0x204]		@ CLK_SRC1

	/* wait at least 200us to stablize all clock */
	mov	r2, #0x10000
1:	subs	r2, r2, #1
	bne	1b

	mov	pc, lr

/*
 * uart_asm_init: Initialize UART's pins
 */
uart_asm_init:
	mov	r0, r8
	ldr	r1, =0x22222222
	str	r1, [r0, #0x0]			@ GPA0_CON
	ldr	r1, =0x00022222
	str	r1, [r0, #0x20]			@ GPA1_CON

	ldr	r1, =0x0
	ldr	r1, [r0, #0xc]

	mov	pc, lr				@ UMCON0

/*
 * tzpc_asm_init: Initialize TZPC
 */
tzpc_asm_init:
	ldr	r0, =0xE3800000
	mov	r1, #0x0
	str	r1, [r0]
	mov	r1, #0xff
	str	r1, [r0, #0x804]
	str	r1, [r0, #0x810]

	ldr	r0, =0xE2800000
	str	r1, [r0, #0x804]
	str	r1, [r0, #0x810]
	str	r1, [r0, #0x81C]

	ldr	r0, =0xE2900000
	str	r1, [r0, #0x804]
	str	r1, [r0, #0x810]

	mov	pc, lr
