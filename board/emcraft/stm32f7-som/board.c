/*
 * (C) Copyright 2011-2015
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
 * Pavel Boldin, Emcraft Systems, paboldin@emcraft.com
 * Vladimir Skvortsov, Emcraft Systems, vskvortsov@emcraft.com
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

/*
 * Board specific code for the Emcraft STM32F7 SOM
 */

#include <common.h>
#include <netdev.h>
#include <ili932x.h>
#include <asm/arch/stm32.h>
#include <asm/arch/stm32f2_gpio.h>
#include <asm/arch/fmc.h>
#include <flash.h>
#include <asm/io.h>
#include <asm/system.h>

#include <asm/arch/fsmc.h>

DECLARE_GLOBAL_DATA_PTR;

static const struct stm32f2_gpio_dsc ext_ram_fsmc_fmc_gpio[] = {
	/* K15, FMC_D15 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_10,	STM32F2_GPIO_ROLE_FMC},
	/* L14, FMC_D14 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_9,	STM32F2_GPIO_ROLE_FMC},
	/* L15, FMC_D13 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_8,	STM32F2_GPIO_ROLE_FMC},
	/* R11, FMC_D12 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_15,	STM32F2_GPIO_ROLE_FMC},
	/* P11, FMC_D11 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_14,	STM32F2_GPIO_ROLE_FMC},
	/* R12, FMC_D10 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_13,	STM32F2_GPIO_ROLE_FMC},
	/* R10, FMC_D9 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_12,	STM32F2_GPIO_ROLE_FMC},
	/* P10, FMC_D8 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_11,	STM32F2_GPIO_ROLE_FMC},
	/* R9, FMC_D7 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_10,	STM32F2_GPIO_ROLE_FMC},
	/* P9, FMC_D6 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_9,	STM32F2_GPIO_ROLE_FMC},
	/* N9, FMC_D5 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_8,	STM32F2_GPIO_ROLE_FMC},
	/* R8, FMC_D4 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_7,	STM32F2_GPIO_ROLE_FMC},
	/* C12, FMC_D3 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_1,	STM32F2_GPIO_ROLE_FMC},
	/* B12, FMC_D2 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_0,	STM32F2_GPIO_ROLE_FMC},
	/* K13, FMC_D1 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_15,	STM32F2_GPIO_ROLE_FMC},
	/* L12, FMC_D0 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_14,	STM32F2_GPIO_ROLE_FMC},

	/* A5, FMC_NBL1 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_1,	STM32F2_GPIO_ROLE_FMC},
	/* A6, FMC_NBL0 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_0,	STM32F2_GPIO_ROLE_FMC},

	/* D11, FMC_NOE */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_4,	STM32F2_GPIO_ROLE_FMC},
	/* C10, FMC_NWE */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_5,	STM32F2_GPIO_ROLE_FMC},

	/* B2, FMC_A22 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_6,	STM32F2_GPIO_ROLE_FMC},
	/* B1, FMC_A21 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_5,	STM32F2_GPIO_ROLE_FMC},
	/* A1, FMC_A20 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_4,	STM32F2_GPIO_ROLE_FMC},
	/* A2, FMC_A19 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_3,	STM32F2_GPIO_ROLE_FMC},
	/* M11, FMC_A18 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_13,	STM32F2_GPIO_ROLE_FMC},
	/* M10, FMC_A17 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_12,	STM32F2_GPIO_ROLE_FMC},
	/* N10, FMC_A16 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_11,	STM32F2_GPIO_ROLE_FMC},

	/* N11, FMC_A15, BA1 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_5,	STM32F2_GPIO_ROLE_FMC},
	/* N12, FMC_A14, BA0 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_4,	STM32F2_GPIO_ROLE_FMC},
	/* M12, FMC_A13 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_3,	STM32F2_GPIO_ROLE_FMC},
	/* M13, FMC_A12 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_2,	STM32F2_GPIO_ROLE_FMC},
	/* M7, FMC_A11 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_1,	STM32F2_GPIO_ROLE_FMC},
	/* N7, FMC_A10 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_0,	STM32F2_GPIO_ROLE_FMC},
	/* M8, FMC_A9 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_15,	STM32F2_GPIO_ROLE_FMC},
	/* P6, FMC_A8 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_14,	STM32F2_GPIO_ROLE_FMC},
	/* N6, FMC_A7 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_13,	STM32F2_GPIO_ROLE_FMC},
	/* M6, FMC_A6 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_12,	STM32F2_GPIO_ROLE_FMC},
	/* K3, FMC_A5 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_5,	STM32F2_GPIO_ROLE_FMC},
	/* J2, FMC_A4 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_4,	STM32F2_GPIO_ROLE_FMC},
	/* H2, FMC_A3 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_3,	STM32F2_GPIO_ROLE_FMC},
	/* G2, FMC_A2 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_2,	STM32F2_GPIO_ROLE_FMC},
	/* E2, FMC_A1 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_1,	STM32F2_GPIO_ROLE_FMC},
	/* D2, FMC_A0 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_0,	STM32F2_GPIO_ROLE_FMC},

	/* SDRAM */
	/* M4, SDRAM_NE */
	{STM32F2_GPIO_PORT_C, STM32F2_GPIO_PIN_2,	STM32F2_GPIO_ROLE_FMC},
	/* P8, SDRAM_NRAS */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_11,	STM32F2_GPIO_ROLE_FMC},
	/* B7, SDRAM_NCAS */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_15,	STM32F2_GPIO_ROLE_FMC},
	/* J3, SDRAM_NWE */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_5,	STM32F2_GPIO_ROLE_FMC},
	/* L4, SDRAM_CKE */
	{STM32F2_GPIO_PORT_C, STM32F2_GPIO_PIN_3,	STM32F2_GPIO_ROLE_FMC},

	/* H14, SDRAM_CLK */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_8,	STM32F2_GPIO_ROLE_FMC},

#ifdef CONFIG_FSMC_NOR_PSRAM_CS1_ENABLE
	/* A11, FMC_NE1 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_7,	STM32F2_GPIO_ROLE_FMC},
#endif
#ifdef CONFIG_FSMC_NOR_PSRAM_CS2_ENABLE
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_9,	STM32F2_GPIO_ROLE_FMC},
#endif
#ifdef CONFIG_FSMC_NOR_PSRAM_CS3_ENABLE
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_10,	STM32F2_GPIO_ROLE_FMC},
#endif
#ifdef CONFIG_FSMC_NOR_PSRAM_CS4_ENABLE
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_12,	STM32F2_GPIO_ROLE_FMC},
#endif
};

#ifdef CONFIG_VIDEO_STM32F4_LTDC
static const struct stm32f2_gpio_dsc ltdc_iomux[] = {
	/* PI14 = LCD_CLK */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_14,	STM32F2_GPIO_ROLE_LTDC},
	/* PK7  = LCD_DE */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_7,	STM32F2_GPIO_ROLE_LTDC},
	/* PI12 = LCD_HSYNC */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_12,	STM32F2_GPIO_ROLE_LTDC},
	/* PI13 = LCD_VSYNC */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_13,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ12 = LCD_B0 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_12,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ13 = LCD_B1 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_13,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ14 = LCD_B2 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_14,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ15 = LCD_B3 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_15,	STM32F2_GPIO_ROLE_LTDC},
	/* PK3  = LCD_B4 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_3,	STM32F2_GPIO_ROLE_LTDC},
	/* PK4  = LCD_B5 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_4,	STM32F2_GPIO_ROLE_LTDC},
	/* PK5  = LCD_B6 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_5,	STM32F2_GPIO_ROLE_LTDC},
	/* PK6  = LCD_B7 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_6,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ7  = LCD_G0 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_7,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ8  = LCD_G1 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_8,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ9  = LCD_G2 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_9,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ10 = LCD_G3 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_10,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ11 = LCD_G4 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_11,	STM32F2_GPIO_ROLE_LTDC},
	/* PK0  = LCD_G5 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_0,	STM32F2_GPIO_ROLE_LTDC},
	/* PK1  = LCD_G6 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_1,	STM32F2_GPIO_ROLE_LTDC},
	/* PK2  = LCD_G7 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_2,	STM32F2_GPIO_ROLE_LTDC},
	/* PI15 = LCD_R0 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_15,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ0  = CD_R1 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_0,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ1  = LCD_R2 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_1,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ2  = LCD_R3 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_2,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ3  = LCD_R4 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_3,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ4  = LCD_R5 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_4,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ5  = LCD_R6 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_5,	STM32F2_GPIO_ROLE_LTDC},
	/* PJ6  = LCD_R7 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_6,	STM32F2_GPIO_ROLE_LTDC},
};
#endif /* CONFIG_VIDEO_STM32F4_LTDC */

/*
 * Init FMC/FSMC GPIOs based
 */
static int fmc_fsmc_setup_gpio(void)
{
	int rv = 0;
	int i;

	/*
	 * Connect GPIOs to FMC controller
	 */
	for (i = 0; i < ARRAY_SIZE(ext_ram_fsmc_fmc_gpio); i++) {
		rv = stm32f2_gpio_config(&ext_ram_fsmc_fmc_gpio[i]);
		if (rv)
			goto out;
	}

	fsmc_gpio_init_done = 1;
out:
	return rv;
}

#ifdef CONFIG_VIDEO_STM32F4_LTDC
/*
 * Initialize LCD pins
 */
static int ltdc_setup_iomux(void)
{
	int rv = 0;
	int i;

	/*
	 * Connect GPIOs to FMC controller
	 */
	for (i = 0; i < ARRAY_SIZE(ltdc_iomux); i++) {
		rv = stm32f2_gpio_config(&ltdc_iomux[i]);
		if (rv)
			break;
	}

	return rv;
}
#endif /* CONFIG_VIDEO_STM32F4_LTDC */

/*
 * Early hardware init.
 */
int board_init(void)
{
	int rv;

	rv = fmc_fsmc_setup_gpio();
	if (rv)
		return rv;

#if !defined(CONFIG_SYS_NO_FLASH)

	if ((rv = fsmc_nor_psram_init(CONFIG_SYS_FLASH_CS,
			CONFIG_SYS_FSMC_FLASH_BCR,
			CONFIG_SYS_FSMC_FLASH_BTR,
			CONFIG_SYS_FSMC_FLASH_BWTR))) {
		goto Done;
	}

#endif

#ifdef CONFIG_VIDEO_STM32F4_LTDC
	rv = ltdc_setup_iomux();
	if (rv)
		return rv;
#endif /* CONFIG_VIDEO_STM32F4_LTDC */

Done:
	return 0;
}

/*
 * Dump pertinent info to the console.
 */
int checkboard(void)
{
	printf("Board: STM32F7 SOM Rev %s, www.emcraft.com\n",
		CONFIG_SYS_BOARD_REV_STR);

	return 0;
}

/*
 * STM32 RCC FMC specific definitions
 */
#define STM32_RCC_ENR_FMC		(1 << 0)	/* FMC module clock  */

static int dram_initialized = 0;

static inline u32 _ns2clk(u32 ns, u32 freq)
{
	uint32_t tmp = freq/1000000;
	return (tmp * ns) / 1000;
}

#define NS2CLK(ns) (_ns2clk(ns, freq))

/*
 * Following are timings for M12L2561616A-6BI, from corresponding datasheet
 */
#define SDRAM_CAS	3
#define SDRAM_NB	1	/* Number of banks */
#define SDRAM_MWID	1	/* 16 bit memory */

#define SDRAM_NR	0x2	/* 13-bit row */
#define SDRAM_NC	0x1	/* 9-bit col */

#define SDRAM_TRRD	NS2CLK(12)
#define SDRAM_TRCD	NS2CLK(18)
#define SDRAM_TRP	NS2CLK(18)
#define SDRAM_TRAS	NS2CLK(42)
#define SDRAM_TRC	NS2CLK(60)
#define SDRAM_TRFC	NS2CLK(60)
#define SDRAM_TCDL	(1 - 1)
#define SDRAM_TRDL	NS2CLK(12)
#define SDRAM_TBDL	(1 - 1)
#define SDRAM_TREF	(NS2CLK(64000000 / 8192) - 20)
#define SDRAM_TCCD	(1 - 1)

#define SDRAM_TXSR	SDRAM_TRFC	/* Row cycle time after precharge */
#define SDRAM_TMRD	(3 - 1)		/* Page 10, Mode Register Set */

/* Last data in to row precharge, need also comply ineq on page 1648 */
#define SDRAM_TWR	max(\
	(int)max((int)SDRAM_TRDL, (int)(SDRAM_TRAS - SDRAM_TRCD)), \
	(int)(SDRAM_TRC - SDRAM_TRCD - SDRAM_TRP)\
)

int dram_init(void)
{
	u32 freq;
	int rv;

	/*
	 * Enable FMC interface clock
	 */
	STM32_RCC->ahb3enr |= STM32_RCC_ENR_FMC;

	/*
	 * Get frequency for NS2CLK calculation.
	 */
	freq = clock_get(CLOCK_HCLK) / CONFIG_SYS_RAM_FREQ_DIV;

	STM32_SDRAM_FMC->sdcr1 = (
		CONFIG_SYS_RAM_FREQ_DIV << FMC_SDCR_SDCLK_SHIFT |
		SDRAM_CAS << FMC_SDCR_CAS_SHIFT |
		SDRAM_NB << FMC_SDCR_NB_SHIFT |
		SDRAM_MWID << FMC_SDCR_MWID_SHIFT |
		SDRAM_NR << FMC_SDCR_NR_SHIFT |
		SDRAM_NC << FMC_SDCR_NC_SHIFT |
		0 << FMC_SDCR_RPIPE_SHIFT |
		1 << FMC_SDCR_RBURST_SHIFT
	);

	STM32_SDRAM_FMC->sdtr1 = (
		SDRAM_TRCD << FMC_SDTR_TRCD_SHIFT |
		SDRAM_TRP << FMC_SDTR_TRP_SHIFT |
		SDRAM_TWR << FMC_SDTR_TWR_SHIFT |
		SDRAM_TRC << FMC_SDTR_TRC_SHIFT |
		SDRAM_TRAS << FMC_SDTR_TRAS_SHIFT |
		SDRAM_TXSR << FMC_SDTR_TXSR_SHIFT |
		SDRAM_TMRD << FMC_SDTR_TMRD_SHIFT
	);

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_START_CLOCK;

	udelay(200);	/* 200 us delay, page 10, "Power-Up" */
	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_PRECHARGE;

	udelay(100);
	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = (
		FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_AUTOREFRESH |
		7 << FMC_SDCMR_NRFS_SHIFT
	);

	udelay(100);
	FMC_BUSY_WAIT();


#define SDRAM_MODE_BL_SHIFT		0
#define SDRAM_MODE_CAS_SHIFT		4

#define SDRAM_MODE_BL			0
#define SDRAM_MODE_CAS			SDRAM_CAS

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 |
	(
		SDRAM_MODE_BL << SDRAM_MODE_BL_SHIFT |
		SDRAM_MODE_CAS << SDRAM_MODE_CAS_SHIFT
	) << FMC_SDCMR_MODE_REGISTER_SHIFT | FMC_SDCMR_MODE_WRITE_MODE;

	udelay(100);

	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_NORMAL;

	FMC_BUSY_WAIT();

	/* Refresh timer */
	STM32_SDRAM_FMC->sdrtr = SDRAM_TREF;

	/*
	 * Fill in global info with description of SRAM configuration
	 */
	gd->bd->bi_dram[0].start = CONFIG_SYS_RAM_BASE;
	gd->bd->bi_dram[0].size  = CONFIG_SYS_RAM_SIZE;

	rv = 0;

	dram_initialized = 1;

	return rv;
}

/*
 * STM32 Flash bug workaround.
 */
extern char	_mem_ram_buf_base, _mem_ram_buf_size;

#define SOC_RAM_BUFFER_BASE	(ulong)(&_mem_ram_buf_base)
#define SOC_RAM_BUFFER_SIZE	(ulong)((&_mem_ram_buf_size) - 0x100)

void stop_ram(void)
{
	if (!dram_initialized)
		return;

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_SELFREFRESH;

	FMC_BUSY_WAIT();
}

void start_ram(void)
{
	if (!dram_initialized)
		return;

	/*
	 * Precharge according to chip requirement, page 12.
	 */

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_PRECHARGE;
	FMC_BUSY_WAIT();


	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_NORMAL;
	FMC_BUSY_WAIT();

	udelay(60);
}

#define NOP10()		do {	nop(); nop(); nop(); nop(); nop(); \
				nop(); nop(); nop(); nop(); nop(); \
			} while(0);

u16 flash_read16(void *addr)
{
	u16 value;
	stop_ram();
	value = __raw_readw(addr);
	NOP10();
	start_ram();
	return value;
}

void flash_write16(u16 value, void *addr)
{
	stop_ram();
	__raw_writew(value, addr);
	NOP10();
	NOP10();
	start_ram();
}

__attribute__((noinline)) void copy_one(volatile u16* src, volatile u16* dst)
{
	*dst = *src;
}

u32 flash_write_buffer(void *src, void *dst, int cnt, int portwidth)
{
	u32 retval = 0;

	if (portwidth != FLASH_CFI_16BIT) {
		retval = ERR_INVAL;
		goto out;
	}

	memcpy((void*)SOC_RAM_BUFFER_BASE, (void*)src, cnt * portwidth);

	stop_ram();
	__asm__ __volatile__("": : :"memory");

	src = (void*) SOC_RAM_BUFFER_BASE;

	while(cnt-- > 0) {
		copy_one(src, dst);
		src += 2, dst += 2;
		NOP10();
		NOP10();
	}

	__asm__ __volatile__("": : :"memory");
	start_ram();
out:
	return retval;
}

u32 flash_check_flag(void *src, void *dst, int cnt, int portwidth)
{
	u32 flag = 1;

	if (portwidth != FLASH_CFI_16BIT) {
		flag = 0;
		goto out;
	}

	stop_ram();

	while((cnt-- > 0) && (flag == 1)) {
		flag = *(u16*)dst == 0xFFFF;
		dst += 2;
	}

	start_ram();

out:
	return flag;
}

#ifdef CONFIG_STM32_ETH
/*
 * Register ethernet driver
 */
int board_eth_init(bd_t *bis)
{
	return stm32_eth_init(bis);
}
#endif

