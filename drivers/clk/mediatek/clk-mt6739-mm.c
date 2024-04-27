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

static const struct mtk_gate_regs mm_cg_regs = {
	.set_ofs = 0x104,
	.clr_ofs = 0x108,
	.sta_ofs = 0x100,
};

#define GATE_MM(_id, _name, _parent, _shift)	\
	GATE_MTK(_id, _name, _parent, &mm_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

static const struct mtk_gate mm_clks[] = {
	GATE_MM(CLK_MM_SMI_COMMON, "mm_smi_common", "mm_sel", 0),
	GATE_MM(CLK_MM_SMI_LARB0, "mm_smi_larb0", "mm_sel", 1),
	GATE_MM(CLK_MM_GALS_COMM0, "mm_gals_comm0", "mm_sel", 2),
	GATE_MM(CLK_MM_GALS_COMM1, "mm_gals_comm1", "mm_sel", 3),
	GATE_MM(CLK_MM_ISP_DL, "mm_isp_dl", "mm_sel", 4),
	GATE_MM(CLK_MM_MDP_RDMA0, "mm_mdp_rdma0", "mm_sel", 5),
	GATE_MM(CLK_MM_MDP_RSZ0, "mm_mdp_rsz0", "mm_sel", 6),
	GATE_MM(CLK_MM_MDP_RSZ1, "mm_mdp_rsz1", "mm_sel", 7),
	GATE_MM(CLK_MM_MDP_TDSHP, "mm_mdp_tdshp", "mm_sel", 8),
	GATE_MM(CLK_MM_MDP_WROT0, "mm_mdp_wrot0", "mm_sel", 9),
	GATE_MM(CLK_MM_MDP_WDMA0, "mm_mdp_wdma0", "mm_sel", 10),
	GATE_MM(CLK_MM_FAKE_ENG, "mm_fake_eng", "mm_sel", 11),
	GATE_MM(CLK_MM_DISP_OVL0, "mm_disp_ovl0", "mm_sel", 12),
	GATE_MM(CLK_MM_DISP_RDMA0, "mm_disp_rdma0", "mm_sel", 13),
	GATE_MM(CLK_MM_DISP_WDMA0, "mm_disp_wdma0", "mm_sel", 14),
	GATE_MM(CLK_MM_DISP_COLOR0, "mm_disp_color0", "mm_sel", 15),
	GATE_MM(CLK_MM_DISP_CCORR0, "mm_disp_ccorr0", "mm_sel", 16),
	GATE_MM(CLK_MM_DISP_AAL0, "mm_disp_aal0", "mm_sel", 17),
	GATE_MM(CLK_MM_DISP_GAMMA0, "mm_disp_gamma0", "mm_sel", 18),
	GATE_MM(CLK_MM_DISP_DITHER0, "mm_disp_dither0", "mm_sel", 19),
	GATE_MM(CLK_MM_DSI_MM_CLOCK, "mm_dsi_mm_clock", "mm_sel", 20),
	GATE_MM(CLK_MM_DSI_INTERF, "mm_dsi_interf", "mm_sel", 21),
	GATE_MM(CLK_MM_DBI_MM_CLOCK, "mm_dbi_mm_clock", "mm_sel", 22),
	GATE_MM(CLK_MM_DBI_INTERF, "mm_dbi_interf", "dbi0_sel", 23),
	GATE_MM(CLK_MM_F26M_HRT, "mm_f26m_hrt", "clk26m", 24),
	GATE_MM(CLK_MM_CG0_B25, "mm_cg0_b25", "mm_sel", 25),
	GATE_MM(CLK_MM_CG0_B26, "mm_cg0_b26", "mm_sel", 26),
	GATE_MM(CLK_MM_CG0_B27, "mm_cg0_b27", "mm_sel", 27),
	GATE_MM(CLK_MM_CG0_B28, "mm_cg0_b28", "mm_sel", 28),
	GATE_MM(CLK_MM_CG0_B29, "mm_cg0_b29", "mm_sel", 29),
	GATE_MM(CLK_MM_CG0_B30, "mm_cg0_b30", "mm_sel", 30),
	GATE_MM(CLK_MM_CG0_B31, "mm_cg0_b31", "mm_sel", 31),
};

static const struct mtk_clk_desc mm_desc = {
	.clks = mm_clks,
	.num_clks = ARRAY_SIZE(mm_clks),
};

static const struct of_device_id of_match_clk_mt6739_mm[] = {
	{
		.compatible = "mediatek,mt6739-mmsys",
		.data = &mm_desc,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6739_mm);

static struct platform_driver clk_mt6739_mm_drv = {
	.probe = mtk_clk_simple_probe,
	.remove_new = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6739-mm",
		.of_match_table = of_match_clk_mt6739_mm,
	},
};
module_platform_driver(clk_mt6739_mm_drv);
MODULE_LICENSE("GPL");
