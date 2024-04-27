/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
*/


#include <linux/delay.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include "clk-gate.h"
#include "clk-mtk.h"
#include "clk-mux.h"

#include <dt-bindings/clock/mt6739-clk.h>

static DEFINE_SPINLOCK(mt6739_clk_lock);

/* Total 7 subsys */
void __iomem *top_base;
void __iomem *infra_base;
void __iomem *apmixed_base;

/* APMIXEDSYS Register */
#define AP_PLL_CON0		(apmixed_base + 0x0)
#define AP_PLL_CON1		(apmixed_base + 0x004)
#define AP_PLL_CON2		(apmixed_base + 0x008)
#define AP_PLL_CON3		(apmixed_base + 0x00C)
#define AP_PLL_CON4		(apmixed_base + 0x010)
#define AP_PLL_CON5		(apmixed_base + 0x014)
#define AP_PLL_CON6		(apmixed_base + 0x018)
#define AP_PLL_CON7		(apmixed_base + 0x01C)
#define AP_PLL_CON8		(apmixed_base + 0x020)
#define CLKSQ_STB_CON0		(apmixed_base + 0x024)
#define PLL_PWR_CON0		(apmixed_base + 0x028)
#define PLL_PWR_CON1		(apmixed_base + 0x02C)
#define PLL_ISO_CON0		(apmixed_base + 0x030)
#define PLL_ISO_CON1		(apmixed_base + 0x034)
#define PLL_STB_CON0		(apmixed_base + 0x038)
#define DIV_STB_CON0		(apmixed_base + 0x03C)
#define PLL_CHG_CON0		(apmixed_base + 0x040)
#define PLL_TEST_CON0		(apmixed_base + 0x044)
#define PLL_TEST_CON1		(apmixed_base + 0x048)
#define ARMPLL_LL_CON0		(apmixed_base + 0x200)
#define ARMPLL_LL_CON1		(apmixed_base + 0x204)
#define ARMPLL_LL_CON2		(apmixed_base + 0x208)
#define ARMPLL_LL_PWR_CON0	(apmixed_base + 0x20C)
#define MAINPLL_CON0		(apmixed_base + 0x220)
#define MAINPLL_CON1		(apmixed_base + 0x224)
#define MAINPLL_CON2		(apmixed_base + 0x228)
#define MAINPLL_PWR_CON0	(apmixed_base + 0x22C)
#define UNIVPLL_CON0		(apmixed_base + 0x230)
#define UNIVPLL_CON1		(apmixed_base + 0x234)
#define UNIVPLL_CON2		(apmixed_base + 0x238)
#define UNIVPLL_PWR_CON0	(apmixed_base + 0x23C)
#define MFGPLL_CON0		(apmixed_base + 0x240)
#define MFGPLL_CON1		(apmixed_base + 0x244)
#define MFGPLL_CON2		(apmixed_base + 0x248)
#define MFGPLL_PWR_CON0		(apmixed_base + 0x24C)
#define MSDCPLL_CON0		(apmixed_base + 0x250)
#define MSDCPLL_CON1		(apmixed_base + 0x254)
#define MSDCPLL_CON2		(apmixed_base + 0x258)
#define MSDCPLL_PWR_CON0	(apmixed_base + 0x25C)
#define MMPLL_CON0		(apmixed_base + 0x270)
#define MMPLL_CON1		(apmixed_base + 0x274)
#define MMPLL_CON2		(apmixed_base + 0x278)
#define MMPLL_PWR_CON0		(apmixed_base + 0x27C)
#define APLL1_CON0		(apmixed_base + 0x2A0)
#define APLL1_CON1		(apmixed_base + 0x2A4)
#define APLL1_CON2		(apmixed_base + 0x2A8)
#define APLL1_CON3		(apmixed_base + 0x2AC)
#define APLL1_PWR_CON0		(apmixed_base + 0x2B0)
#define AP_AUXADC_CON0		(apmixed_base + 0x400)
#define AP_AUXADC_CON1		(apmixed_base + 0x404)
#define AP_AUXADC_CON2		(apmixed_base + 0x408)
#define AP_AUXADC_CON3		(apmixed_base + 0x40C)
#define AP_AUXADC_CON4		(apmixed_base + 0x410)
#define AP_AUXADC_CON5		(apmixed_base + 0x414)
#define TS_CON0			(apmixed_base + 0x600)
#define TS_CON1			(apmixed_base + 0x604)
#define TS_CON2			(apmixed_base + 0x608)
#define AP_ABIST_MON_CON0	(apmixed_base + 0x800)
#define AP_ABIST_MON_CON1	(apmixed_base + 0x804)
#define AP_ABIST_MON_CON2	(apmixed_base + 0x808)
#define AP_ABIST_MON_CON3	(apmixed_base + 0x80C)
#define OCCSCAN_CON0		(apmixed_base + 0x810)
#define CLKDIV_CON0		(apmixed_base + 0x814)
#define OCCSCAN_CON1		(apmixed_base + 0x818)
#define OCCSCAN_CON2		(apmixed_base + 0x81C)
#define MCU_OCCSCAN_CON0	(apmixed_base + 0x820)
#define RSV_RW0_CON0		(apmixed_base + 0x900)
#define RSV_RW1_CON0		(apmixed_base + 0x904)
#define RSV_RO_CON0		(apmixed_base + 0x908)

/* infracfg_ao Base address: (+10001000h) */
#define INFRA_GLOBALCON_DCMCTL			(infra_base + 0x50)
#define INFRA_BUS_DCM_CTRL				(infra_base + 0x70)
#define PERI_BUS_DCM_CTRL				(infra_base + 0x74)
#define MODULE_SW_CG_0_SET				(infra_base + 0x80)
#define MODULE_SW_CG_0_CLR				(infra_base + 0x84)
#define MODULE_SW_CG_1_SET				(infra_base + 0x88)
#define MODULE_SW_CG_1_CLR				(infra_base + 0x8C)
#define MODULE_SW_CG_0_STA				(infra_base + 0x90)
#define MODULE_SW_CG_1_STA				(infra_base + 0x94)
#define MODULE_CLK_SEL					(infra_base + 0x98)
#define MODULE_SW_CG_2_SET				(infra_base + 0xA4)
#define MODULE_SW_CG_2_CLR				(infra_base + 0xA8)
#define MODULE_SW_CG_2_STA				(infra_base + 0xAC)
#define MODULE_SW_CG_3_SET				(infra_base + 0xC0)
#define MODULE_SW_CG_3_CLR				(infra_base + 0xC4)
#define MODULE_SW_CG_3_STA				(infra_base + 0xC8)
#define INFRA_TOPAXI_PROTECTEN			(infra_base + 0x0220)
#define INFRA_TOPAXI_PROTECTEN_STA1		(infra_base + 0x0228)
#define INFRA_TOPAXI_PROTECTEN_1		(infra_base + 0x0250)
#define INFRA_TOPAXI_PROTECTEN_STA1_1	(infra_base + 0x0258)

/* TOPCKGEN Register */
#define CLK_MODE		(top_base + 0x0)
#define CLK_MISC_CFG_0		(top_base + 0x104)
#define CLK_MISC_CFG_1		(top_base + 0x108)
#define CLK_DBG_CFG		(top_base + 0x10C)
#define CLK_SCP_CFG_0		(top_base + 0x200)
#define CLK_SCP_CFG_1		(top_base + 0x204)
#define CLK26CALI_0		(top_base + 0x220)
#define CLK26CALI_1		(top_base + 0x224)
#define CKSTA_REG		(top_base + 0x230)
#define CKSTA_REG1		(top_base + 0x234)
#define CLKMON_CLK_SEL_REG	(top_base + 0x300)
#define CLKMON_K1_REG		(top_base + 0x304)
#define CLK_AUDDIV_0		(top_base + 0x320)
#define CLK_AUDDIV_1		(top_base + 0x324)
#define CLK_AUDDIV_2		(top_base + 0x328)
#define AUD_TOP_CFG		(top_base + 0x32C)
#define AUD_TOP_MON		(top_base + 0x330)
#define CLK_PDN_REG		(top_base + 0x400)
#define CLK_EXTCK_REG		(top_base + 0x500)

#define INFRA_CG0 0x9BBFFF20
#define INFRA_CG1 0x1E8F7F56
#define INFRA_CG2 0x07FCC7DD
#define INFRA_CG3 0x00000DFF

static const struct mtk_fixed_clk fixed_clks[] = {
	FIXED_CLK(CLK_TOP_CLK26M, "f_f26m_ck", "clk26m", 26000000),
};

static const struct mtk_fixed_factor top_divs[] = {
	FACTOR(CLK_TOP_SYSPLL, "syspll_ck", "mainpll", 1, 1),
	FACTOR(CLK_TOP_SYSPLL_D2, "syspll_d2", "syspll_ck", 1, 2),
	FACTOR(CLK_TOP_SYSPLL1_D2, "syspll1_d2", "syspll_d2", 1, 2),
	FACTOR(CLK_TOP_SYSPLL1_D4, "syspll1_d4", "syspll_d2", 1, 4),
	FACTOR(CLK_TOP_SYSPLL1_D8, "syspll1_d8", "syspll_d2", 1, 8),
	FACTOR(CLK_TOP_SYSPLL1_D16, "syspll1_d16", "syspll_d2", 1, 16),
	FACTOR(CLK_TOP_SYSPLL_D3, "syspll_d3", "mainpll", 1, 3),
	FACTOR(CLK_TOP_SYSPLL2_D2, "syspll2_d2", "syspll_d3", 1, 2),
	FACTOR(CLK_TOP_SYSPLL2_D4, "syspll2_d4", "syspll_d3", 1, 4),
	FACTOR(CLK_TOP_SYSPLL_D5, "syspll_d5", "mainpll", 1, 5),
	FACTOR(CLK_TOP_SYSPLL3_D2, "syspll3_d2", "syspll_d5", 1, 2),
	FACTOR(CLK_TOP_SYSPLL3_D4, "syspll3_d4", "syspll_d5", 1, 4),
	FACTOR(CLK_TOP_SYSPLL_D7, "syspll_d7", "mainpll", 1, 7),
	FACTOR(CLK_TOP_SYSPLL4_D2, "syspll4_d2", "syspll_d7", 1, 2),
	FACTOR(CLK_TOP_SYSPLL4_D4, "syspll4_d4", "syspll_d7", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL, "univpll_ck", "univpll", 1, 1),
	FACTOR(CLK_TOP_UNIVPLL_D26, "univpll_d26", "univpll_ck", 1, 26),
	FACTOR(CLK_TOP_UNIVPLL_48M_D2, "univpll_48m_d2", "univpll_d26", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL_48M_D4, "univpll_48m_d4", "univpll_d26", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL_48M_D8, "univpll_48m_d8", "univpll_d26", 1, 8),
	FACTOR(CLK_TOP_UNIVPLL_D2, "univpll_d2", "univpll", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL1_D2, "univpll1_d2", "univpll_d2", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL1_D4, "univpll1_d4", "univpll_d2", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL_D3, "univpll_d3", "univpll", 1, 3),
	FACTOR(CLK_TOP_UNIVPLL2_D2, "univpll2_d2", "univpll_d3", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL2_D4, "univpll2_d4", "univpll_d3", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL2_D8, "univpll2_d8", "univpll_d3", 1, 8),
	FACTOR(CLK_TOP_UNIVPLL2_D32, "univpll2_d32", "univpll_d3", 1, 32),
	FACTOR(CLK_TOP_UNIVPLL_D5, "univpll_d5", "univpll", 1, 5),
	FACTOR(CLK_TOP_UNIVPLL3_D2, "univpll3_d2", "univpll_d5", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL3_D4, "univpll3_d4", "univpll_d5", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL3_D8, "univpll3_d8", "univpll_d5", 1, 8),
	FACTOR(CLK_TOP_MMPLL, "mmpll_ck", "mfgpll", 1, 1),
	FACTOR(CLK_TOP_VENCPLL, "vencpll_ck", "mmpll", 1, 1),
	FACTOR(CLK_TOP_MSDCPLL, "msdcpll_ck", "msdcpll", 1, 1),
	FACTOR(CLK_TOP_MSDCPLL_D2, "msdcpll_d2", "msdcpll", 1, 2),
	FACTOR(CLK_TOP_APLL1, "apll1_ck", "apll1", 1, 1),
	FACTOR(CLK_TOP_APLL1_D2, "apll1_d2", "apll1", 1, 2),
	FACTOR(CLK_TOP_APLL1_D4, "apll1_d4", "apll1", 1, 4),
	FACTOR(CLK_TOP_APLL1_D8, "apll1_d8", "apll1", 1, 8),
};

static const char * const axi_parents[] = {
	"clk26m",
	"syspll_d7",
	"syspll1_d4",
	"syspll3_d2"
};

static const char * const mem_parents[] = {
	"clk26m",
	"dmpll_ck",
	"apll1_ck"
};

static const char * const ddrphycfg_parents[] = {
	"clk26m",
	"syspll1_d8"
};

static const char * const mm_parents[] = {
	"clk26m",
	"vencpll_ck",
	"syspll1_d2",
	"syspll_d5",
	"syspll1_d4",
	"univpll_d5",
	"syspll2_d2",
	"univpll2_d2"
};

static const char * const mfg_parents[] = {
	"clk26m",
	"mmpll_ck",
	"syspll_d3",
	"syspll_d5"
};

static const char * const camtg_parents[] = {
	"clk26m",
	"univpll_48m_d2",
	"univpll2_d8",
	"univpll_d26",
	"univpll2_d32",
	"univpll_48m_d4",
	"univpll_48m_d8"
};

static const char * const uart_parents[] = {
	"clk26m",
	"univpll2_d8"
};

static const char * const spi_parents[] = {
	"clk26m",
	"syspll3_d2",
	"syspll4_d2",
	"syspll2_d4"
};

static const char * const msdc5hclk_parents[] = {
	"clk26m",
	"syspll1_d2",
	"syspll2_d2"
};

static const char * const msdc50_0_parents[] = {
	"clk26m",
	"msdcpll_ck",
	"syspll2_d2",
	"syspll4_d2",
	"univpll1_d2",
	"syspll1_d2",
	"univpll_d5",
	"univpll1_d4"
};

static const char * const msdc30_1_parents[] = {
	"clk26m",
	"msdcpll_d2",
	"univpll2_d2",
	"syspll2_d2",
	"syspll1_d4",
	"univpll1_d4",
	"univpll_d26",
	"syspll2_d4"
};

static const char * const audio_parents[] = {
	"clk26m",
	"syspll3_d4",
	"syspll4_d4",
	"syspll1_d16"
};

static const char * const aud_intbus_parents[] = {
	"clk26m",
	"syspll1_d4",
	"syspll4_d2"
};

static const char * const dbi0_parents[] = {
	"clk26m",
	"univpll3_d2",
	"univpll2_d4",
	"syspll4_d2",
	"univpll2_d8"
};

static const char * const scam_parents[] = {
	"clk26m",
	"syspll3_d2",
	"univpll2_d4"
};

static const char * const aud_1_parents[] = {
	"clk26m",
	"apll1_ck"
};

static const char * const disp_pwm_parents[] = {
	"clk26m",
	"univpll2_d4",
	"univpll2_d8",
	"univpll3_d8"
};

static const char * const nfi2x_parents[] = {
	"clk26m",
	"syspll2_d2",
	"syspll_d7",
	"syspll_d3",
	"syspll2_d4",
	"msdcpll_d2",
	"univpll1_d2",
	"univpll_d5"
};

static const char * const nfiecc_parents[] = {
	"clk26m",
	"syspll4_d2",
	"univpll2_d4",
	"syspll_d7",
	"univpll1_d2",
	"syspll1_d2",
	"univpll2_d2",
	"syspll_d5"
};

static const char * const usb_top_parents[] = {
	"clk26m",
	"univpll3_d4"
};

static const char * const i2c_parents[] = {
	"clk26m",
	"univpll_d26",
	"univpll3_d4"
};

static const char * const senif_parents[] = {
	"clk26m",
	"univpll1_d4",
	"univpll2_d2"
};

static const char * const dxcc_parents[] = {
	"clk26m",
	"syspll1_d2",
	"syspll1_d4",
	"syspll1_d8"
};

static const char * const aud_engen1_parents[] = {
	"clk26m",
	"apll1_d2",
	"apll1_d4",
	"apll1_d8"
};

static const struct mtk_mux top_muxes[] = {
	/* CLK_CFG_0 */
	MUX_GATE_CLR_SET_UPD_FLAGS(CLK_TOP_AXI_SEL, "axi_sel",
		axi_parents, 0x40, 0x44, 0x48, 0, 2, 7, 0x004, 0, CLK_IS_CRITICAL),
	MUX_GATE_CLR_SET_UPD_FLAGS(CLK_TOP_MEM_SEL, "mem_sel",
		mem_parents, 0x40, 0x44, 0x48, 8, 2, 15, 0x004, 1, CLK_IS_CRITICAL),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_DDRPHYCFG_SEL, "ddrphycfg_sel",
		ddrphycfg_parents, 0x40, 0x44, 0x48, 16, 1, 23, 0x004, 2),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MM_SEL, "mm_sel",
		mm_parents, 0x40, 0x44, 0x48, 24, 3, 31, 0x004, 3),
	/* CLK_CFG_1 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MFG_SEL, "mfg_sel",
		mfg_parents, 0x50, 0x54, 0x58, 24, 2, 31, 0x004, 7),
	/* CLK_CFG_2 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_CAMTG_SEL, "camtg_sel",
		camtg_parents, 0x60, 0x64, 0x68, 0, 3, 7, 0x004, 8),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_UART_SEL, "uart_sel",
		uart_parents, 0x60, 0x64, 0x68, 8, 1, 15, 0x004, 9),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SPI_SEL, "spi_sel",
		spi_parents, 0x60, 0x64, 0x68, 16, 2, 23, 0x004, 10),
	/* CLK_CFG_3 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MSDC50_0_HCLK_SEL, "msdc5hclk",
		msdc5hclk_parents, 0x70, 0x74, 0x78, 8, 2, 15, 0x004, 12),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MSDC50_0_SEL, "msdc50_0_sel",
		msdc50_0_parents, 0x70, 0x74, 0x78, 16, 3, 23, 0x004, 13),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MSDC30_1_SEL, "msdc30_1_sel",
		msdc30_1_parents, 0x70, 0x74, 0x78, 24, 3, 31, 0x004, 14),
	/* CLK_CFG_4 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUDIO_SEL, "audio_sel",
		audio_parents, 0x80, 0x84, 0x88, 16, 2, 23, 0x004, 17),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUD_INTBUS_SEL, "aud_intbus_sel",
		aud_intbus_parents, 0x80, 0x84, 0x88, 24, 2, 31, 0x004, 18),
	/* CLK_CFG_6 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_DBI0_SEL, "dbi0_sel",
		dbi0_parents, 0xa0, 0xa4, 0xa8, 0, 3, 7, 0x004, 23),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SCAM_SEL, "scam_sel",
		scam_parents, 0xa0, 0xa4, 0xa8, 8, 2, 15, 0x004, 24),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUD_1_SEL, "aud_1_sel",
		aud_1_parents, 0xa0, 0xa4, 0xa8, 16, 1, 23, 0x004, 25),
	/* CLK_CFG_7 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_DISP_PWM_SEL, "disp_pwm_sel",
		disp_pwm_parents, 0xb0, 0xb4, 0xb8, 0, 2, 7, 0x004, 27),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_NFI2X_SEL, "nfi2x_sel",
		nfi2x_parents, 0xb0, 0xb4, 0xb8, 8, 3, 15, 0x004, 28),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_NFIECC_SEL, "nfiecc_sel",
		nfiecc_parents, 0xb0, 0xb4, 0xb8, 16, 3, 23, 0x004, 29),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_USB_TOP_SEL, "usb_top_sel",
		usb_top_parents, 0xb0, 0xb4, 0xb8, 24, 1, 31, 0x004, 30),
	/* CLK_CFG_8 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SPM_SEL, "spm_sel",
		ddrphycfg_parents, 0xc0, 0xc4, 0xc8, 0, 1, 7, 0x004, 31),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_I2C_SEL, "i2c_sel",
		i2c_parents, 0xc0, 0xc4, 0xc8, 16, 2, 23, 0x008, 1),
	/* CLK_CFG_9 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SENIF_SEL, "senif_sel"
		senif_parents, 0xd0, 0xd4, 0xd8, 8, 2, 15, 0x008, 3),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_DXCC_SEL, "dxcc_sel",
		dxcc_parents, 0xd0, 0xd4, 0xd8, 16, 2, 23,0x008, 4),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_CAMTG2_SEL, "camtg2_sel",
		camtg_parents, 0xd0, 0xd4, 0xd8, 24, 3, 31, 	0x008, 9),
	/* CLK_CFG_10 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUD_ENGEN1_SEL, "aud_engen1_sel",
		aud_engen1_parents, 0xe0, 0xe4, 0xe8, 0, 2, 7, 0x008, 5),
};

static const struct mtk_gate_regs infra0_cg_regs = {
	.set_ofs = 0x80,
	.clr_ofs = 0x84,
	.sta_ofs = 0x90,
};

static const struct mtk_gate_regs infra1_cg_regs = {
	.set_ofs = 0x88,
	.clr_ofs = 0x8c,
	.sta_ofs = 0x94,
};

static const struct mtk_gate_regs infra2_cg_regs = {
	.set_ofs = 0xa4,
	.clr_ofs = 0xa8,
	.sta_ofs = 0xac,
};

static const struct mtk_gate_regs infra3_cg_regs = {
	.set_ofs = 0xc0,
	.clr_ofs = 0xc4,
	.sta_ofs = 0xc8,
};

#define GATE_INFRA0(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &infra0_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

#define GATE_INFRA1(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &infra1_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

#define GATE_INFRA2(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &infra2_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

#define GATE_INFRA3(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &infra3_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
	}

static const struct mtk_gate infra_clks[] = {
	/* INFRA0 */
	GATE_INFRA0(CLK_INFRA_PMIC_TMR, "infra_pmic_tmr", "clk26m", 0),
	GATE_INFRA0(CLK_INFRA_PMIC_AP, "infra_pmic_ap", "clk26m", 1),
	GATE_INFRA0(CLK_INFRA_PMIC_MD, "infra_pmic_md", "clk26m", 2),
	GATE_INFRA0(CLK_INFRA_PMIC_CONN, "infra_pmic_conn", "clk26m", 3),
	GATE_INFRA0(CLK_INFRA_SEJ, "infra_sej", "clk26m", 5),
	GATE_INFRA0(CLK_INFRA_APXGPT, "infra_apxgpt", "axi_sel", 6),
	GATE_INFRA0(CLK_INFRA_ICUSB, "infra_icusb", "axi_sel", 8),
	GATE_INFRA0(CLK_INFRA_GCE, "infra_gce", "axi_sel", 9),
	GATE_INFRA0(CLK_INFRA_THERM, "infra_therm", "axi_sel", 10),
	GATE_INFRA0(CLK_INFRA_I2C0, "infra_i2c0", "i2c_sel", 11),
	GATE_INFRA0(CLK_INFRA_I2C1, "infra_i2c1", "i2c_sel", 12),
	GATE_INFRA0(CLK_INFRA_I2C2, "infra_i2c2", "i2c_sel", 13),
	GATE_INFRA0(CLK_INFRA_I2C3, "infra_i2c3", "i2c_sel", 14),
	GATE_INFRA0(CLK_INFRA_PWM_HCLK, "infra_pwm_hclk", "axi_sel", 15),
	GATE_INFRA0(CLK_INFRA_PWM1, "infra_pwm1", "i2c_sel", 16),
	GATE_INFRA0(CLK_INFRA_PWM2, "infra_pwm2", "i2c_sel", 17),
	GATE_INFRA0(CLK_INFRA_PWM3, "infra_pwm3", "i2c_sel", 18),
	GATE_INFRA0(CLK_INFRA_PWM4, "infra_pwm4", "i2c_sel", 19),
	GATE_INFRA0(CLK_INFRA_PWM5, "infra_pwm5", "i2c_sel", 20),
	GATE_INFRA0(CLK_INFRA_PWM, "infra_pwm", "i2c_sel", 21),
	GATE_INFRA0(CLK_INFRA_UART0, "infra_uart0", "uart_sel", 22),
	GATE_INFRA0(CLK_INFRA_UART1, "infra_uart1", "uart_sel", 23),
	GATE_INFRA0(CLK_INFRA_UART2, "infra_uart2", "uart_sel", 24),
	GATE_INFRA0(CLK_INFRA_UART3, "infra_uart3", "uart_sel", 25),
	GATE_INFRA0(CLK_INFRA_GCE_26M, "infra_gce_26m", "axi_sel", 27),
	GATE_INFRA0(CLK_INFRA_CQ_DMA_FPC, "infra_dma", "axi_sel", 28),
	GATE_INFRA0(CLK_INFRA_BTIF, "infra_btif", "axi_sel", 31),
	/* INFRA1 */
	GATE_INFRA1(CLK_INFRA_SPI0, "infra_spi0", "spi_sel", 1),
	GATE_INFRA1(CLK_INFRA_MSDC0, "infra_msdc0", "axi_sel", 2),
	GATE_INFRA1(CLK_INFRA_MSDC1, "infra_msdc1", "axi_sel", 4),
	GATE_INFRA1(CLK_INFRA_NFIECC_312M, "infra_nfiecc", "nfiecc_sel", 6),
	GATE_INFRA1(CLK_INFRA_DVFSRC, "infra_dvfsrc", "clk26m", 7),
	GATE_INFRA1(CLK_INFRA_GCPU, "infra_gcpu", "axi_sel", 8),
	GATE_INFRA1(CLK_INFRA_TRNG, "infra_trng", "axi_sel", 9),
	GATE_INFRA1(CLK_INFRA_AUXADC, "infra_auxadc", "clk26m", 10),
	GATE_INFRA1(CLK_INFRA_CPUM, "infra_cpum", "axi_sel", 11),
	GATE_INFRA1(CLK_INFRA_CCIF1_AP, "infra_ccif1_ap", "axi_sel", 12),
	GATE_INFRA1(CLK_INFRA_CCIF1_MD, "infra_ccif1_md", "axi_sel", 13),
	GATE_INFRA1(CLK_INFRA_AUXADC_MD, "infra_auxadc_md", "clk26m", 14),
	GATE_INFRA1(CLK_INFRA_NFI, "infra_nfi", "nfi2x_sel", 16),
	GATE_INFRA1(CLK_INFRA_NFI_1X, "infra_nfi_1x", "nfi2x_sel", 17),
	GATE_INFRA1(CLK_INFRA_AP_DMA, "infra_ap_dma", "axi_sel", 18),
	GATE_INFRA1(CLK_INFRA_XIU, "infra_xiu", "axi_sel", 19),
	GATE_INFRA1(CLK_INFRA_DEVICE_APC, "infra_dapc", "axi_sel", 20),
	GATE_INFRA1(CLK_INFRA_CCIF_AP, "infra_ccif_ap", "axi_sel", 23),
	GATE_INFRA1(CLK_INFRA_DEBUGSYS, "infra_debugsys", "axi_sel", 24),
	GATE_INFRA1(CLK_INFRA_AUDIO, "infra_audio", "axi_sel", 25),
	GATE_INFRA1(CLK_INFRA_CCIF_MD, "infra_ccif_md", "axi_sel", 26),
	GATE_INFRA1(CLK_INFRA_DXCC_SEC_CORE, "infra_secore", "dxcc_sel", 27),
	GATE_INFRA1(CLK_INFRA_DXCC_AO, "infra_dxcc_ao", "dxcc_sel", 28),
	GATE_INFRA1(CLK_INFRA_DRAMC_F26M, "infra_dramc26", "clk26m", 31),
	/* INFRA2 */
	GATE_INFRA2(CLK_INFRA_RG_PWM_FBCLK6, "infra_pwmfb", "clk26m", 0),
	GATE_INFRA2(CLK_INFRA_DISP_PWM, "infra_disp_pwm", "disp_pwm_sel", 2),
	GATE_INFRA2(CLK_INFRA_CLDMA_BCLK, "infra_cldmabclk", "axi_sel", 3),
	GATE_INFRA2(CLK_INFRA_AUDIO_26M_BCLK, "infra_audio26m", "clk26m", 4),
	GATE_INFRA2(CLK_INFRA_SPI1, "infra_spi1", "spi_sel", 6),
	GATE_INFRA2(CLK_INFRA_I2C4, "infra_i2c4", "i2c_sel", 7),
	GATE_INFRA2(CLK_INFRA_MODEM_TEMP_SHARE, "infra_mdtemp", "clk26m", 8),
	GATE_INFRA2(CLK_INFRA_SPI2, "infra_spi2", "spi_sel", 9),
	GATE_INFRA2(CLK_INFRA_SPI3, "infra_spi3", "spi_sel", 10),
	GATE_INFRA2(CLK_INFRA_I2C5, "infra_i2c5", "i2c_sel", 18),
	GATE_INFRA2(CLK_INFRA_I2C5_ARBITER, "infra_i2c5a", "i2c_sel", 19),
	GATE_INFRA2(CLK_INFRA_I2C5_IMM, "infra_i2c5_imm", "i2c_sel", 20),
	GATE_INFRA2(CLK_INFRA_I2C1_ARBITER, "infra_i2c1a", "i2c_sel", 21),
	GATE_INFRA2(CLK_INFRA_I2C1_IMM, "infra_i2c1_imm", "i2c_sel", 22),
	GATE_INFRA2(CLK_INFRA_I2C2_ARBITER, "infra_i2c2a", "i2c_sel", 23),
	GATE_INFRA2(CLK_INFRA_I2C2_IMM, "infra_i2c2_imm", "i2c_sel", 24),
	GATE_INFRA2(CLK_INFRA_SPI4, "infra_spi4", "spi_sel", 25),
	GATE_INFRA2(CLK_INFRA_SPI5, "infra_spi5", "spi_sel", 26),
	GATE_INFRA2(CLK_INFRA_CQ_DMA, "infra_cq_dma", "axi_sel", 27),
	/* INFRA3 */
	GATE_INFRA3(CLK_INFRA_MSDC0_SELF, "infra_msdc0sf", "msdc50_0_sel", 0),
	GATE_INFRA3(CLK_INFRA_MSDC1_SELF, "infra_msdc1sf", "msdc50_0_sel", 1),
	GATE_INFRA3(CLK_INFRA_MSDC2_SELF, "infra_msdc2sf", "msdc50_0_sel", 2),
	GATE_INFRA3(CLK_INFRA_I2C6, "infra_i2c6", "i2c_sel", 6),
	GATE_INFRA3(CLK_INFRA_AP_MSDC0, "infra_ap_msdc0", "msdc50_0_sel", 7),
	GATE_INFRA3(CLK_INFRA_MD_MSDC0, "infra_md_msdc0", "msdc50_0_sel", 8),
	/*DUMMY CG*/
	GATE_INFRA3(CLK_INFRA_MSDC0_SRC, "infra_msdc0_clk", "msdc50_0_sel", 31),
	GATE_INFRA3(CLK_INFRA_MSDC1_SRC, "infra_msdc1_clk", "msdc30_1_sel", 10),
	GATE_INFRA3(CLK_INFRA_MSDC2_SRC, "infra_msdc2_clk", "msdc50_0_sel", 11),
};


#define MT6739_PLL_FMAX		(3000UL * MHZ)
#define MT6739_PLL_FMIN		(1000UL * MHZ)

#define CON0_MT6739_RST_BAR	BIT(24)

#define PLL_B(_id, _name, _reg, _pwr_reg, _en_mask, _flags, \
		_pcwbits, _pd_reg, _pd_shift, _tuner_reg,\
		_pcw_reg, _pcw_shift, _div_table, _pcwintbits,\
		_pcwchgreg) {\
		.id = _id,						\
		.name = _name,						\
		.reg = _reg,						\
		.pwr_reg = _pwr_reg,					\
		.en_mask = _en_mask,					\
		.flags = _flags,					\
		.rst_bar_mask = CON0_MT6739_RST_BAR,			\
		.fmax = MT6739_PLL_FMAX,				\
		.fmin = MT6739_PLL_FMIN,			\
		.pcwbits = _pcwbits,					\
		.pcwibits = _pcwintbits,			\
		.pcw_chg_reg = _pcwchgreg,			\
		.pd_reg = _pd_reg,					\
		.pd_shift = _pd_shift,					\
		.tuner_reg = _tuner_reg,				\
		.pcw_reg = _pcw_reg,					\
		.pcw_shift = _pcw_shift,				\
		.div_table = _div_table,				\
	}

#define PLL(_id, _name, _reg, _pwr_reg, _en_mask, _flags, \
		_pcwbits, _pd_reg, _pd_shift, _tuner_reg,\
		_pcw_reg, _pcw_shift, _pcwintbits, _pcwchgreg)\
	PLL_B(_id, _name, _reg, _pwr_reg, _en_mask, _flags, \
		_pcwbits,	_pd_reg, _pd_shift, _tuner_reg,\
		_pcw_reg, _pcw_shift, NULL, _pcwintbits, _pcwchgreg)

static const struct mtk_pll_data plls[] = {
	PLL(CLK_APMIXED_MAINPLL, "mainpll", 0x0220, 0x022C, 0x80000111,
		HAVE_RST_BAR | PLL_AO, 21, 0x0224, 24, 0, 0x0224, 0, 7, 0x4),
	PLL(CLK_APMIXED_MFGPLL, "mfgpll", 0x0240, 0x024C, 0x40000111, 0,
		21, 0x0244, 24, 0, 0x0244, 0, 7, 0x4),
	PLL(CLK_APMIXED_MMPLL, "mmpll", 0x0270, 0x027C, 0x40000111, 0,
		21, 0x0274, 24, 0, 0x0274, 0, 7, 0x4),
	PLL(CLK_APMIXED_UNIVPLL, "univpll", 0x0230, 0x023C, 0xf0000111,
		HAVE_RST_BAR, 21, 0x0234, 24, 0, 0x0234, 0, 7, 0x4),
	PLL(CLK_APMIXED_MSDCPLL, "msdcpll", 0x0250, 0x025C, 0x40000111, 0,
		21, 0x0254, 24, 0, 0x0254, 0, 7, 0x4),
	PLL(CLK_APMIXED_APLL1, "apll1", 0x02A0, 0x02B0, 0x40000111, 0,
		32, 0x02A0, 1, 0, 0x02A4, 0, 8, 0x0),
};

static int clk_mt6739_apmixed_probe(struct platform_device *pdev)
{
	struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	struct device_node *node = pdev->dev.of_node;
	struct clk_onecell_data *clk_data;
	unsigned int armpll_ll_enable = 0;
	void __iomem *base;
	unsigned int val;
	int r;

	base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(base)) {
		pr_err("%s(): ioremap failed\n", __func__);
		return PTR_ERR(base);
	}

	clk_data = mtk_alloc_clk_data(CLK_APMIXED_NR_CLK);
	if (!clk_data)
		return -ENOMEM;

	mtk_clk_register_gates(node, apmixed_clks,
		ARRAY_SIZE(apmixed_clks), clk_data);
	r = of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);

	if (r) {
		kfree(clk_data);
		pr_err("%s(): could not register clock provider: %d\n",
				__func__, r);
	}

	apmixed_base = base;
	/*MAINPLL*/
	clk_clrl(MAINPLL_CON0, 0x70000000);
	/* BY_MAINDIV_DLY */
	clk_clrl(AP_PLL_CON6, 0x00020000);

	return r;
}

static int clk_mt6739_top_probe(struct platform_device *pdev)
{
	struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	struct device_node *node = pdev->dev.of_node;
	struct clk_onecell_data *clk_data;
	void __iomem *base;
	int r;

	base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(base)) {
		pr_err("%s(): ioremap failed\n", __func__);
		return PTR_ERR(base);
	}

	clk_data = mtk_alloc_clk_data(CLK_TOP_NR_CLK);
	if (!clk_data)
		return -ENOMEM;

	mtk_clk_register_fixed_clks(fixed_clks, ARRAY_SIZE(fixed_clks),
			clk_data);

	mtk_clk_register_factors(top_divs, ARRAY_SIZE(top_divs),
			clk_data);

	mtk_clk_register_muxes(top_muxes, ARRAY_SIZE(top_muxes), node,
			&mt6739_clk_lock, clk_data);

	r = of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);

	if (r) {
		kfree(clk_data);
		pr_err("%s(): could not register clock provider: %d\n",
				__func__, r);
	}
	
	top_base = base;
	/*[10]:no need*/
	writel(readl(CLK_SCP_CFG_0) | 0x3EF, CLK_SCP_CFG_0);
	/*[1,2,3,8]: no need*/
	writel(readl(CLK_SCP_CFG_1) | 0x1, CLK_SCP_CFG_1);

	return r;
}

static int clk_mt6739_infracfg_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	struct clk_onecell_data *clk_data;
	int r;

	clk_data = mtk_alloc_clk_data(CLK_INFRA_NR_CLK);
	if (!clk_data)
		return -ENOMEM;

	mtk_clk_register_gates(node, infra_clks,
		ARRAY_SIZE(infra_clks), clk_data);
	r = of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);

	if (r) {
		kfree(clk_data);
		pr_err("%s(): could not register clock provider: %d\n",
				__func__, r);
	}

	infra_base = base;

	writel(INFRA_CG0, MODULE_SW_CG_0_SET);
	writel(INFRA_CG1, MODULE_SW_CG_1_SET);
	writel(INFRA_CG2, MODULE_SW_CG_2_SET);
	writel(INFRA_CG3, MODULE_SW_CG_3_SET);

	return r;
}

static const struct of_device_id of_match_clk_mt6739[] = {
	{
		.compatible = "mediatek,mt6739-apmixed",
		.data = clk_mt6739_apmixed_probe,
	}, {
		.compatible = "mediatek,mt6739-topckgen",
		.data = clk_mt6739_top_probe,
	}, {
		.compatible = "mediatek,mt6739-infracfg",
		.data = clk_mt6739_infracfg_probe,
	}
};

static int clk_mt6739_probe(struct platform_device *pdev)
{
	int (*clk_probe)(struct platform_device *pdev);
	int r;

	clk_probe = of_device_get_match_data(&pdev->dev);
	if (!clk_probe)
		return -EINVAL;

	r = clk_probe(pdev);
	if (r)
		dev_err(&pdev->dev,
			"could not register clock provider: %s: %d\n",
			pdev->name, r);

	return r;
}

static struct platform_driver clk_mt6739_drv = {
	.probe = clk_mt6739_probe,
	.driver = {
		.name = "clk-mt6739",
		.owner = THIS_MODULE,
		.of_match_table = of_match_clk_mt6739,
	},
};

static int __init clk_mt6739_init(void)
{
	return platform_driver_register(&clk_mt6739_drv);
}

arch_initcall(clk_mt6739_init);
MODULE_LICENSE("GPL");
