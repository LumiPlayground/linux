// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2022 Yassine Oudjana <y.oudjana@protonmail.com>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-gate.h"
#include "clk-mtk.h"

#include <dt-bindings/clock/mediatek,mt6735-mmsys.h>

#define MMSYS_CG_CON0			0x100
#define MMSYS_CG_SET0			0x104
#define MMSYS_CG_CLR0			0x108
#define MMSYS_CG_CON1			0x110
#define MMSYS_CG_SET1			0x114
#define MMSYS_CG_CLR1			0x118

static struct mtk_gate_regs mmsys_cg_regs0 = {
	.set_ofs = MMSYS_CG_SET0,
	.clr_ofs = MMSYS_CG_CLR0,
	.sta_ofs = MMSYS_CG_CON0,
};

static struct mtk_gate_regs mmsys_cg_regs1 = {
	.set_ofs = MMSYS_CG_SET1,
	.clr_ofs = MMSYS_CG_CLR1,
	.sta_ofs = MMSYS_CG_CON1,
};

static const struct mtk_gate mmsys_gates[] = {
	GATE_MTK(CLK_SMI_COMMON, "smi_common", "mm_sel", &mmsys_cg_regs0, 0, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_SMI_LARB0, "smi_larb0", "mm_sel", &mmsys_cg_regs0, 1, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_CAM_MDP, "cam_mdp", "mm_sel", &mmsys_cg_regs0, 2, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_MDP_RDMA, "mdp_rdma", "mm_sel", &mmsys_cg_regs0, 3, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_MDP_RSZ0, "mdp_rsz0", "mm_sel", &mmsys_cg_regs0, 4, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_MDP_RSZ1, "mdp_rsz1", "mm_sel", &mmsys_cg_regs0, 5, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_MDP_TDSHP, "mdp_tdshp", "mm_sel", &mmsys_cg_regs0, 6, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_MDP_WDMA, "mdp_wdma", "mm_sel", &mmsys_cg_regs0, 7, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_MDP_WROT, "mdp_wrot", "mm_sel", &mmsys_cg_regs0, 8, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_FAKE_ENG, "fake_eng", "mm_sel", &mmsys_cg_regs0, 9, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DISP_OVL0, "disp_ovl0", "mm_sel", &mmsys_cg_regs0, 10, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DISP_RDMA0, "disp_rdma0", "mm_sel", &mmsys_cg_regs0, 11, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DISP_RDMA1, "disp_rdma1", "mm_sel", &mmsys_cg_regs0, 12, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DISP_WDMA0, "disp_wdma0", "mm_sel", &mmsys_cg_regs0, 13, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DISP_COLOR, "disp_color", "mm_sel", &mmsys_cg_regs0, 14, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DISP_CCORR, "disp_ccorr", "mm_sel", &mmsys_cg_regs0, 15, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DISP_AAL, "disp_aal", "mm_sel", &mmsys_cg_regs0, 16, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DISP_GAMMA, "disp_gamma", "mm_sel", &mmsys_cg_regs0, 17, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DISP_DITHER, "disp_dither", "mm_sel", &mmsys_cg_regs0, 18, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_AXI_ASIF, "axi_asif", "mm_sel", &mmsys_cg_regs0, 20, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DSI_ENGINE, "dsi_engine", "mm_sel", &mmsys_cg_regs1, 2, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DSI_DIGITAL, "dsi_digital", "mm_sel", &mmsys_cg_regs1, 3, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DPI_ENGINE, "dpi_engine", "mm_sel", &mmsys_cg_regs1, 4, &mtk_clk_gate_ops_setclr),
	GATE_MTK(CLK_DPI_PIXEL, "dpi_pixel", "dpi0_sel", &mmsys_cg_regs1, 5, &mtk_clk_gate_ops_setclr),
};

static const struct mtk_clk_desc mmsys_desc = {
	.clks = mmsys_gates,
	.num_clks = ARRAY_SIZE(mmsys_gates),
};

static const struct platform_device_id clk_mt6735_mmsys_id_table[] = {
	{ .name = "clk-mt6735-mm", .driver_data = (kernel_ulong_t)&mmsys_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(platform, clk_mt6735_mmsys_id_table);

static struct platform_driver clk_mt6735_mmsys = {
	.probe = mtk_clk_pdev_probe,
	.remove_new = mtk_clk_pdev_remove,
	.driver = {
		.name = "clk-mt6735-mmsys",
	},
	.id_table = clk_mt6735_mmsys_id_table
};
module_platform_driver(clk_mt6735_mmsys);

MODULE_AUTHOR("Yassine Oudjana <y.oudjana@protonmail.com>");
MODULE_DESCRIPTION("MediaTek MT6735 mmsys clock driver");
MODULE_LICENSE("GPL");
