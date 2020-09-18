/*
 * (C) Copyright 2008-2017 Fuzhou Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _ROCKCHIP_DISPLAY_H
#define _ROCKCHIP_DISPLAY_H

#include <bmp_layout.h>
#include <drm_modes.h>
#include <edid.h>
#include <dm/ofnode.h>

#define ROCKCHIP_OUTPUT_DSI_DUAL_CHANNEL	BIT(0)
#define ROCKCHIP_OUTPUT_DSI_DUAL_LINK		BIT(1)

enum data_format {
	ROCKCHIP_FMT_ARGB8888 = 0,
	ROCKCHIP_FMT_RGB888,
	ROCKCHIP_FMT_RGB565,
	ROCKCHIP_FMT_YUV420SP = 4,
	ROCKCHIP_FMT_YUV422SP,
	ROCKCHIP_FMT_YUV444SP,
};

enum display_mode {
	ROCKCHIP_DISPLAY_FULLSCREEN,
	ROCKCHIP_DISPLAY_CENTER,
};

enum rockchip_cmd_type {
	CMD_TYPE_DEFAULT,
	CMD_TYPE_SPI,
	CMD_TYPE_MCU
};

enum rockchip_mcu_cmd {
	MCU_WRCMD = 0,
	MCU_WRDATA,
	MCU_SETBYPASS,
};

/*
 * display output interface supported by rockchip lcdc
 */
#define ROCKCHIP_OUT_MODE_P888	0
#define ROCKCHIP_OUT_MODE_P666	1
#define ROCKCHIP_OUT_MODE_P565	2
#define ROCKCHIP_OUT_MODE_S888		8
#define ROCKCHIP_OUT_MODE_S888_DUMMY	12
#define ROCKCHIP_OUT_MODE_YUV420	14
/* for use special outface */
#define ROCKCHIP_OUT_MODE_AAAA	15

struct rockchip_mcu_timing {
	int mcu_pix_total;
	int mcu_cs_pst;
	int mcu_cs_pend;
	int mcu_rw_pst;
	int mcu_rw_pend;
	int mcu_hold_mode;
};

struct vop_rect {
	int width;
	int height;
};

struct crtc_state {
	struct udevice *dev;
	struct rockchip_crtc *crtc;
	void *private;
	ofnode node;
	int crtc_id;

	int format;
	u32 dma_addr;
	int ymirror;
	int rb_swap;
	int xvir;
	int src_x;
	int src_y;
	int src_w;
	int src_h;
	int crtc_x;
	int crtc_y;
	int crtc_w;
	int crtc_h;
	bool yuv_overlay;
	struct rockchip_mcu_timing mcu_timing;
	u32 dual_channel_swap;
	struct vop_rect max_output;
};

struct panel_state {
	struct rockchip_panel *panel;

	ofnode dsp_lut_node;
};

struct overscan {
	int left_margin;
	int right_margin;
	int top_margin;
	int bottom_margin;
};

struct connector_state {
	struct udevice *dev;
	const struct rockchip_connector *connector;
	struct rockchip_bridge *bridge;
	struct rockchip_phy *phy;
	ofnode node;

	void *private;

	struct drm_display_mode mode;
	struct overscan overscan;
	u8 edid[EDID_SIZE * 4];
	int bus_format;
	int output_mode;
	int type;
	int output_type;
	int color_space;

	struct {
		u32 *lut;
		int size;
	} gamma;
};

struct logo_info {
	int mode;
	char *mem;
	bool ymirror;
	u32 offset;
	u32 width;
	int height;
	u32 bpp;
};

struct rockchip_logo_cache {
	struct list_head head;
	char name[20];
	struct logo_info logo;
};

struct display_state {
	struct list_head head;

	const void *blob;
	ofnode node;

	struct crtc_state crtc_state;
	struct connector_state conn_state;
	struct panel_state panel_state;

	char ulogo_name[30];
	char klogo_name[30];

	struct logo_info logo;
	int logo_mode;
	int charge_logo_mode;
	void *mem_base;
	int mem_size;

	int enable;
	int is_init;
	int is_enable;
};


#define UPDATE(v, h, l)		(((v) << (l)) & GENMASK((h), (l)))

#define DSI_VERSION			0x00
#define DSI_PWR_UP			0x04
#define RESET				0
#define POWERUP				BIT(0)

#define DSI_CLKMGR_CFG			0x08
#define TO_CLK_DIVIDSION(div)		(((div) & 0xff) << 8)
#define TX_ESC_CLK_DIVIDSION(div)	(((div) & 0xff) << 0)

#define DSI_DPI_VCID			0x0c
#define DPI_VID(vid)			(((vid) & 0x3) << 0)

#define DSI_DPI_COLOR_CODING		0x10
#define EN18_LOOSELY			BIT(8)
#define DPI_COLOR_CODING_16BIT_1	0x0
#define DPI_COLOR_CODING_16BIT_2	0x1
#define DPI_COLOR_CODING_16BIT_3	0x2
#define DPI_COLOR_CODING_18BIT_1	0x3
#define DPI_COLOR_CODING_18BIT_2	0x4
#define DPI_COLOR_CODING_24BIT		0x5

#define DSI_DPI_CFG_POL			0x14
#define COLORM_ACTIVE_LOW		BIT(4)
#define SHUTD_ACTIVE_LOW		BIT(3)
#define HSYNC_ACTIVE_LOW		BIT(2)
#define VSYNC_ACTIVE_LOW		BIT(1)
#define DATAEN_ACTIVE_LOW		BIT(0)

#define DSI_DPI_LP_CMD_TIM		0x18
#define OUTVACT_LPCMD_TIME(p)		(((p) & 0xff) << 16)
#define INVACT_LPCMD_TIME(p)		((p) & 0xff)

#define DSI_DBI_VCID			0x1c
#define DBI_VCID(x)			UPDATE(x, 1, 0)
#define DSI_DBI_CFG			0x20
#define DSI_DBI_CMDSIZE			0x28

#define DSI_PCKHDL_CFG			0x2c
#define CRC_RX_EN			BIT(4)
#define ECC_RX_EN			BIT(3)
#define BTA_EN				BIT(2)
#define EOTP_RX_EN			BIT(1)
#define EOTP_TX_EN			BIT(0)
#define DSI_MODE_CFG			0x34
#define CMD_VIDEO_MODE			BIT(0)
#define COMMAND_MODE			BIT(0)
#define VIDEO_MODE			0
#define DSI_VID_MODE_CFG		0x38
#define VPG_EN				BIT(16)
#define LP_CMD_EN			BIT(15)
#define FRAME_BTA_ACK			BIT(14)
#define LP_HFP_EN			BIT(13)
#define LP_HBP_EN			BIT(12)
#define LP_VACT_EN			BIT(11)
#define LP_VFP_EN			BIT(10)
#define LP_VBP_EN			BIT(9)
#define LP_VSA_EN			BIT(8)
#define VID_MODE_TYPE_BURST_SYNC_PULSES	0x0
#define VID_MODE_TYPE_BURST_SYNC_EVENTS	0x1
#define VID_MODE_TYPE_BURST		0x2

#define DSI_VID_PKT_SIZE		0x3c
#define VID_PKT_SIZE(p)			(((p) & 0x3fff) << 0)
#define VID_PKT_MAX_SIZE		0x3fff

#define DSI_VID_NUM_CHUMKS		0x40
#define DSI_VID_NULL_PKT_SIZE		0x44
#define DSI_VID_HSA_TIME		0x48
#define DSI_VID_HBP_TIME		0x4c
#define DSI_VID_HLINE_TIME		0x50
#define DSI_VID_VSA_LINES		0x54
#define DSI_VID_VBP_LINES		0x58
#define DSI_VID_VFP_LINES		0x5c
#define DSI_VID_VACTIVE_LINES		0x60
#define DSI_EDPI_CMD_SIZE		0x64
#define DSI_CMD_MODE_CFG		0x68
#define MAX_RD_PKT_SIZE			BIT(24)
#define DCS_LW_TX			BIT(19)
#define DCS_SR_0P_TX			BIT(18)
#define DCS_SW_1P_TX			BIT(17)
#define DCS_SW_0P_TX			BIT(16)
#define GEN_LW_TX			BIT(14)
#define GEN_SR_2P_TX			BIT(13)
#define GEN_SR_1P_TX			BIT(12)
#define GEN_SR_0P_TX			BIT(11)
#define GEN_SW_2P_TX			BIT(10)
#define GEN_SW_1P_TX			BIT(9)
#define GEN_SW_0P_TX			BIT(8)
#define ACK_RQST_EN			BIT(1)
#define TEAR_FX_EN			BIT(0)

#define DSI_GEN_HDR			0x6c
#define GEN_HDATA(data)			(((data) & 0xffff) << 8)
#define GEN_HDATA_MASK			(0xffff << 8)
#define GEN_HTYPE(type)			(((type) & 0xff) << 0)
#define GEN_HTYPE_MASK			0xff

#define DSI_GEN_PLD_DATA		0x70

#define DSI_CMD_PKT_STATUS		0x74
#define GEN_CMD_EMPTY			BIT(0)
#define GEN_CMD_FULL			BIT(1)
#define GEN_PLD_W_EMPTY			BIT(2)
#define GEN_PLD_W_FULL			BIT(3)
#define GEN_PLD_R_EMPTY			BIT(4)
#define GEN_PLD_R_FULL			BIT(5)
#define GEN_RD_CMD_BUSY			BIT(6)

#define DSI_TO_CNT_CFG			0x78
#define HSTX_TO_CNT(p)			(((p) & 0xffff) << 16)
#define LPRX_TO_CNT(p)			((p) & 0xffff)

#define DSI_BTA_TO_CNT			0x8c
#define DSI_LPCLK_CTRL			0x94
#define AUTO_CLKLANE_CTRL		BIT(1)
#define PHY_TXREQUESTCLKHS		BIT(0)

#define DSI_PHY_TMR_LPCLK_CFG		0x98
#define PHY_CLKHS2LP_TIME(lbcc)		(((lbcc) & 0x3ff) << 16)
#define PHY_CLKLP2HS_TIME(lbcc)		((lbcc) & 0x3ff)

#define DSI_PHY_TMR_CFG			0x9c
#define PHY_HS2LP_TIME(lbcc)		(((lbcc) & 0xff) << 24)
#define PHY_LP2HS_TIME(lbcc)		(((lbcc) & 0xff) << 16)
#define MAX_RD_TIME(lbcc)		((lbcc) & 0x7fff)

#define DSI_PHY_RSTZ			0xa0
#define PHY_ENFORCEPLL			BIT(3)
#define PHY_ENABLECLK			BIT(2)
#define PHY_RSTZ			BIT(1)
#define PHY_SHUTDOWNZ			BIT(0)

#define DSI_PHY_IF_CFG			0xa4
#define N_LANES(n)			((((n) - 1) & 0x3) << 0)
#define PHY_STOP_WAIT_TIME(cycle)	(((cycle) & 0xff) << 8)

#define DSI_PHY_STATUS			0xb0
#define PHY_STOPSTATE0LANE		BIT(4)
#define PHY_STOPSTATECLKLANE		BIT(2)
#define PHY_LOCK			BIT(0)
#define PHY_STOPSTATELANE		(PHY_STOPSTATE0LANE | \
					 PHY_STOPSTATECLKLANE)

#define DSI_PHY_TST_CTRL0		0xb4
#define PHY_TESTCLK			BIT(1)
#define PHY_TESTCLR			BIT(0)

#define DSI_PHY_TST_CTRL1		0xb8
#define PHY_TESTEN			BIT(16)
#define PHY_TESTDOUT_SHIFT		8
#define PHY_TESTDIN_MASK		GENMASK(7, 0)
#define PHY_TESTDIN(x)			UPDATE(x, 7, 0)

#define DSI_INT_ST0			0xbc
#define DSI_INT_ST1			0xc0
#define DSI_INT_MSK0			0xc4
#define DSI_INT_MSK1			0xc8

#define PHY_STATUS_TIMEOUT_US		10000
#define CMD_PKT_STATUS_TIMEOUT_US	20000

/* Test Code: 0x44 (HS RX Control of Lane 0) */
#define HSFREQRANGE(x)			UPDATE(x, 6, 1)
/* Test Code: 0x17 (PLL Input Divider Ratio) */
#define INPUT_DIV(x)			UPDATE(x, 6, 0)
/* Test Code: 0x18 (PLL Loop Divider Ratio) */
#define FEEDBACK_DIV_LO(x)		UPDATE(x, 4, 0)
#define FEEDBACK_DIV_HI(x)		(BIT(7) | UPDATE(x, 3, 0))

#define GRF_REG_FIELD(reg, lsb, msb)	((reg << 16) | (lsb << 8) | (msb))

enum grf_reg_fields {
	DPIUPDATECFG,
	DPISHUTDN,
	DPICOLORM,
	VOPSEL,
	TURNREQUEST,
	TURNDISABLE,
	FORCETXSTOPMODE,
	FORCERXMODE,
	ENABLE_N,
	MASTERSLAVEZ,
	ENABLECLK,
	BASEDIR,
	MAX_FIELDS,
};

struct dw_mipi_dsi_plat_data {
	const u32 *dsi0_grf_reg_fields;
	const u32 *dsi1_grf_reg_fields;
	unsigned long max_bit_rate_per_lane;
};

struct mipi_dphy {
	/* Non-SNPS PHY */
	struct rockchip_phy *phy;

	u16 input_div;
	u16 feedback_div;
};

struct dw_mipi_dsi {
	struct udevice *dev;
	void *base;
	void *grf;
	int id;
	struct dw_mipi_dsi *master;
	struct dw_mipi_dsi *slave;
	bool prepared;

	unsigned int lane_mbps; /* per lane */
	u32 channel;
	u32 lanes;
	u32 format;
	u32 mode_flags;
	struct mipi_dphy dphy;
	struct drm_display_mode mode;

	const struct dw_mipi_dsi_plat_data *pdata;
};


static inline struct rockchip_panel *state_get_panel(struct display_state *s)
{
	struct panel_state *panel_state = &s->panel_state;

	return panel_state->panel;
}

int drm_mode_vrefresh(const struct drm_display_mode *mode);
int display_send_mcu_cmd(struct display_state *state, u32 type, u32 val);
bool drm_mode_is_420(const struct drm_display_info *display,
		     struct drm_display_mode *mode);

void drm_mode_max_resolution_filter(struct hdmi_edid_data *edid_data,
				    struct vop_rect *max_output);

#endif
