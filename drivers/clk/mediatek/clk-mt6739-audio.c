// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2018 MediaTek Inc.
 * Author: Owen Chen <owen.chen@mediatek.com>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mt6739-clk.h>

static const struct mtk_gate_regs audio0_cg_regs = {
	.set_ofs = 0x0,
	.clr_ofs = 0x0,
	.sta_ofs = 0x0,
};

static const struct mtk_gate_regs audio1_cg_regs = {
	.set_ofs = 0x4,
	.clr_ofs = 0x4,
	.sta_ofs = 0x4,
};

#define GATE_AUDIO0(_id, _name, _parent, _shift)		\
	GATE_MTK(_id, _name, _parent, &audio0_cg_regs, _shift, &mtk_clk_gate_ops_no_setclr)

#define GATE_AUDIO1(_id, _name, _parent, _shift)		\
	GATE_MTK(_id, _name, _parent, &audio1_cg_regs, _shift, &mtk_clk_gate_ops_no_setclr)

static const struct mtk_gate audio_clks[] = {
	/* AUDIO0 */
	GATE_AUDIO0(CLK_AUDIO_AFE, "aud_afe", "audio_sel", 2),
	GATE_AUDIO0(CLK_AUDIO_22M, "aud_22m", "aud_engen1_sel", 8),
	GATE_AUDIO0(CLK_AUDIO_APLL_TUNER, "aud_apll_tuner",
		    "aud_engen1_sel", 19),
	GATE_AUDIO0(CLK_AUDIO_ADC, "aud_adc", "audio_sel", 24),
	GATE_AUDIO0(CLK_AUDIO_DAC, "aud_dac", "audio_sel", 25),
	GATE_AUDIO0(CLK_AUDIO_DAC_PREDIS, "aud_dac_predis",
		    "audio_sel", 26),
	GATE_AUDIO0(CLK_AUDIO_TML, "aud_tml", "audio_sel", 27),
	/* AUDIO1 */
	GATE_AUDIO1(CLK_AUDIO_I2S1_BCLK, "aud_i2s1_bclk",
		    "audio_sel", 4),
	GATE_AUDIO1(CLK_AUDIO_I2S2_BCLK, "aud_i2s2_bclk",
		    "audio_sel", 5),
	GATE_AUDIO1(CLK_AUDIO_I2S3_BCLK, "aud_i2s3_bclk",
		    "audio_sel", 6),
	GATE_AUDIO1(CLK_AUDIO_I2S4_BCLK, "aud_i2s4_bclk",
		    "audio_sel", 7),
};

static const struct mtk_clk_desc audio_desc = {
	.clks = audio_clks,
	.num_clks = ARRAY_SIZE(audio_clks),
};

static const struct of_device_id of_match_clk_mt6739_audio[] = {
	{
		.compatible = "mediatek,mt6739-audsys",
		.data = &audio_desc,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6739_audio);

static struct platform_driver clk_mt6739_audio_drv = {
	.probe = mtk_clk_simple_probe,
	.remove_new = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6739-audio",
		.of_match_table = of_match_clk_mt6739_audio,
	},
};
module_platform_driver(clk_mt6739_audio_drv);
MODULE_LICENSE("GPL");
