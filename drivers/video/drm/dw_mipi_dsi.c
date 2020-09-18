/*
 * (C) Copyright 2008-2017 Fuzhou Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <drm/drm_mipi_dsi.h>

#include <config.h>
#include <common.h>
#include <errno.h>
#include <asm/unaligned.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <dm/device.h>
#include <dm/read.h>
#include <dm/of_access.h>
#include <syscon.h>
#include <asm/arch-rockchip/clock.h>
#include <linux/iopoll.h>

#include "rockchip_display.h"
#include "rockchip_crtc.h"
#include "rockchip_connector.h"
#include "rockchip_panel.h"
#include "rockchip_phy.h"

static const char *grf_reg_fields_string[] = {
    "DPIUPDATECFG", "DPISHUTDN", "DPICOLORM", "VOPSEL",
    "TURNREQUEST", "TURNDISABLE", "FORCETXSTOPMODE", "FORCERXMODE",
    "ENABLE_N", "MASTERSLAVEZ", "ENABLECLK", "BASEDIR",
	"MAX_FIELDS",
};

static const u32 rk3399_dsi0_grf_reg_fields[MAX_FIELDS] = {
	[DPIUPDATECFG]		= GRF_REG_FIELD(0x6224, 15, 15),
	[DPISHUTDN]		= GRF_REG_FIELD(0x6224, 14, 14),
	[DPICOLORM]		= GRF_REG_FIELD(0x6224, 13, 13),
	[VOPSEL]		= GRF_REG_FIELD(0x6250,  0,  0),
	[TURNREQUEST]		= GRF_REG_FIELD(0x6258, 12, 15),
	[TURNDISABLE]		= GRF_REG_FIELD(0x6258,  8, 11),
	[FORCETXSTOPMODE]	= GRF_REG_FIELD(0x6258,  4,  7),
	[FORCERXMODE]		= GRF_REG_FIELD(0x6258,  0,  3),
};

static inline void dsi_write(struct dw_mipi_dsi *dsi, u32 reg, u32 val)
{
	//printf("zjj.rk3399.uboot dsi_write dsi-> base:%p + reg:0x%x val:0x%x\n", (void*)0xff960000 ,reg, val);

	writel(val, (void*)0xff960000 + reg);
}

static inline u32 dsi_read(struct dw_mipi_dsi *dsi, u32 reg)
{
	return readl((void*)0xff960000 + reg);
}

static inline void dsi_update_bits(struct dw_mipi_dsi *dsi,
				   u32 reg, u32 mask, u32 val)
{
	u32 orig, tmp;
	//printf("zjj.rk3399.uboot dsi_update_bits dsi-> grf:%p + reg:0x%x  mask:0x%x val:%x\n", (void*)0xff770000 ,reg, mask, val);

	orig = dsi_read(dsi, reg);
	tmp = orig & ~mask;
	tmp |= val & mask;
	dsi_write(dsi, reg, tmp);
}

static void grf_field_write(struct dw_mipi_dsi *dsi, enum grf_reg_fields index,
			    unsigned int val)
{
	const u32 field = rk3399_dsi0_grf_reg_fields[index];
	u16 reg;
	u8 msb, lsb;

	if (!field)
		return;

	printf("zjj.rk3399.uboot grf_field_write name %s\n", grf_reg_fields_string[index]);

	reg = (field >> 16) & 0xffff;
	lsb = (field >>  8) & 0xff;
	msb = (field >>  0) & 0xff;
	printf("zjj.rk3399.uboot grf_field_write vop->grf:%p + reg:0x%x  GENMASK(msb:0x%x, lsb:0x%x) val:0x%x << lsb:0x%x\n", (void*)0xff770000 ,reg, msb, lsb, val, lsb);

	rk_clrsetreg((void*)0xff770000 + reg, GENMASK(msb, lsb), val << lsb);
}

static inline void dpishutdn_assert(struct dw_mipi_dsi *dsi)
{
	grf_field_write(dsi, DPISHUTDN, 1);
}

static inline void dpishutdn_deassert(struct dw_mipi_dsi *dsi)
{
	grf_field_write(dsi, DPISHUTDN, 0);
}

static int genif_wait_cmd_fifo_not_full(struct dw_mipi_dsi *dsi)
{
	u32 sts;
	int ret;

	ret = readl_poll_timeout((void*)0xff960000 + DSI_CMD_PKT_STATUS,
				 sts, !(sts & GEN_CMD_FULL),
				 CMD_PKT_STATUS_TIMEOUT_US);
	if (ret < 0) {
		printf("generic write cmd fifo is full\n");
		return ret;
	}

	return 0;
}

static int genif_wait_write_fifo_empty(struct dw_mipi_dsi *dsi)
{
	u32 sts;
	u32 mask;
	int ret;

	mask = GEN_CMD_EMPTY | GEN_PLD_W_EMPTY;
	ret = readl_poll_timeout((void*)0xff960000 + DSI_CMD_PKT_STATUS,
				 sts, (sts & mask) == mask,
				 CMD_PKT_STATUS_TIMEOUT_US);
	if (ret < 0) {
		printf("generic write fifo is full\n");
		return ret;
	}

	return 0;
}

static inline u8 testif_get_data(struct dw_mipi_dsi *dsi)
{
	return dsi_read(dsi, DSI_PHY_TST_CTRL1) >> PHY_TESTDOUT_SHIFT;
}



static void testif_write(struct dw_mipi_dsi *dsi, u8 test_code, u8 test_data)
{

	//testif_test_code_write(dsi, test_code);
	//testif_testclk_assert(dsi);
	dsi_update_bits(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLK, PHY_TESTCLK);
	udelay(1);

	//testif_set_data(dsi, test_code);
	dsi_update_bits(dsi, DSI_PHY_TST_CTRL1,
			PHY_TESTDIN_MASK, PHY_TESTDIN(test_code));
	udelay(1);

	//testif_testen_assert(dsi);
	dsi_update_bits(dsi, DSI_PHY_TST_CTRL1, PHY_TESTEN, PHY_TESTEN);
	udelay(1);

	//testif_testclk_deassert(dsi);
	dsi_update_bits(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLK, 0);
	udelay(1);

	//testif_testen_deassert(dsi);
	dsi_update_bits(dsi, DSI_PHY_TST_CTRL1, PHY_TESTEN, 0);
	udelay(1);

	//testif_test_data_write(dsi, test_data);
	//testif_testclk_deassert(dsi);
	dsi_update_bits(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLK, 0);
	udelay(1);

	//testif_set_data(dsi, test_data);
	dsi_update_bits(dsi, DSI_PHY_TST_CTRL1,
			PHY_TESTDIN_MASK, PHY_TESTDIN(test_data));
	udelay(1);

	//testif_testclk_assert(dsi);
	dsi_update_bits(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLK, PHY_TESTCLK);
	udelay(1);

	printf("test_code=0x%02x, test_data=0x%02x, monitor_data=0x%02x\n",
		test_code, test_data, testif_get_data(dsi));
}

static int mipi_dphy_power_on(struct dw_mipi_dsi *dsi)
{
	u32 mask, val;
	int ret;
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	//mipi_dphy_shutdownz_deassert(dsi);
	dsi_update_bits(dsi, DSI_PHY_RSTZ, PHY_SHUTDOWNZ, PHY_SHUTDOWNZ);
	udelay(1);

	//mipi_dphy_rstz_deassert(dsi);
	dsi_update_bits(dsi, DSI_PHY_RSTZ, PHY_RSTZ, PHY_RSTZ);
	udelay(1);
	mdelay(2);

	//if (dsi->dphy.phy) {
	//	rockchip_phy_set_mode(dsi->dphy.phy, PHY_MODE_VIDEO_MIPI);
	//	rockchip_phy_power_on(dsi->dphy.phy);
	//}

	ret = readl_poll_timeout((void*)0xff960000 + DSI_PHY_STATUS,
				 val, val & PHY_LOCK, PHY_STATUS_TIMEOUT_US);
	if (ret < 0) {
		printf("PHY is not locked\n");
		return ret;
	}

	udelay(200);

	mask = PHY_STOPSTATELANE;
	ret = readl_poll_timeout((void*)0xff960000 + DSI_PHY_STATUS,
				 val, (val & mask) == mask,
				 PHY_STATUS_TIMEOUT_US);
	if (ret < 0) {
		printf("lane module is not in stop state\n");
		return ret;
	}

	udelay(10);

	return 0;
}


static int dw_mipi_dsi_read_from_fifo(struct dw_mipi_dsi *dsi,
				      const struct mipi_dsi_msg *msg)
{
	u8 *payload = msg->rx_buf;
	u16 length;
	u32 val;
	int ret;

	ret = readl_poll_timeout((void*)0xff960000 + DSI_CMD_PKT_STATUS,
				 val, !(val & GEN_RD_CMD_BUSY), 5000);
	if (ret) {
		printf("entire response isn't stored in the FIFO\n");
		return ret;
	}

	/* Receive payload */
	for (length = msg->rx_len; length; length -= 4) {
		ret = readl_poll_timeout((void*)0xff960000 + DSI_CMD_PKT_STATUS,
					 val, !(val & GEN_PLD_R_EMPTY), 5000);
		if (ret) {
			printf("Read payload FIFO is empty\n");
			return ret;
		}

		val = dsi_read(dsi, DSI_GEN_PLD_DATA);

		switch (length) {
		case 3:
			payload[2] = (val >> 16) & 0xff;
			/* Fall through */
		case 2:
			payload[1] = (val >> 8) & 0xff;
			/* Fall through */
		case 1:
			payload[0] = val & 0xff;
			return 0;
		}

		payload[0] = (val >>  0) & 0xff;
		payload[1] = (val >>  8) & 0xff;
		payload[2] = (val >> 16) & 0xff;
		payload[3] = (val >> 24) & 0xff;
		payload += 4;
	}

	return 0;
}

static ssize_t dw_mipi_dsi_transfer(struct dw_mipi_dsi *dsi,
				    const struct mipi_dsi_msg *msg)
{
	struct mipi_dsi_packet packet;
	int ret;
	int val;
	//printf("zjj.rk3399.uboot  msg->type:0x%x mode_flags:0x%x\n", msg->type ,dsi->mode_flags);

	dsi_update_bits(dsi, DSI_VID_MODE_CFG, LP_CMD_EN, LP_CMD_EN);
	dsi_update_bits(dsi, DSI_LPCLK_CTRL, PHY_TXREQUESTCLKHS, 0);


	switch (msg->type) {
	case MIPI_DSI_DCS_SHORT_WRITE:

		dsi_update_bits(dsi, DSI_CMD_MODE_CFG, DCS_SW_0P_TX,
				dsi->mode_flags & MIPI_DSI_MODE_LPM ?
				DCS_SW_0P_TX : 0);
		break;
	case MIPI_DSI_DCS_SHORT_WRITE_PARAM:
		dsi_update_bits(dsi, DSI_CMD_MODE_CFG, DCS_SW_1P_TX,
				dsi->mode_flags & MIPI_DSI_MODE_LPM ?
				DCS_SW_1P_TX : 0);
		break;
	default:
		return -EINVAL;
	}

	dsi_update_bits(dsi, DSI_CMD_MODE_CFG,
				ACK_RQST_EN, ACK_RQST_EN);

	/* create a packet to the DSI protocol */
	ret = mipi_dsi_create_packet(&packet, msg);


	ret = genif_wait_cmd_fifo_not_full(dsi);


	/* Send packet header */
	val = get_unaligned_le32(packet.header);
	dsi_write(dsi, DSI_GEN_HDR, val);

	ret = genif_wait_write_fifo_empty(dsi);
	if (ret)
		return ret;

	if (msg->rx_len) {
		ret = dw_mipi_dsi_read_from_fifo(dsi, msg);
		if (ret < 0)
			return ret;
	}

	if (dsi->slave) {
		ret = dw_mipi_dsi_transfer(dsi->slave, msg);
		if (ret < 0)
			return ret;
	}

	return msg->rx_len ? msg->rx_len : msg->tx_len;
}

static void dw_mipi_dsi_enable(struct dw_mipi_dsi *dsi)
{
	//const struct drm_display_mode *mode = &dsi->mode;
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	dsi_update_bits(dsi, DSI_LPCLK_CTRL,
			PHY_TXREQUESTCLKHS, PHY_TXREQUESTCLKHS);
	dsi_write(dsi, DSI_PWR_UP, RESET);
	dsi_update_bits(dsi, DSI_MODE_CFG, CMD_VIDEO_MODE, VIDEO_MODE);
	dsi_write(dsi, DSI_PWR_UP, POWERUP);

}

static void dw_mipi_dsi_disable(struct dw_mipi_dsi *dsi)
{
	dsi_write(dsi, DSI_PWR_UP, RESET);
	dsi_write(dsi, DSI_LPCLK_CTRL, 0);
	dsi_write(dsi, DSI_EDPI_CMD_SIZE, 0);
	dsi_update_bits(dsi, DSI_MODE_CFG, CMD_VIDEO_MODE, COMMAND_MODE);
	dsi_write(dsi, DSI_PWR_UP, POWERUP);
}

static void dw_mipi_dsi_post_disable(struct dw_mipi_dsi *dsi)
{
	if (!dsi->prepared)
		return;

	if (dsi->master)
		dw_mipi_dsi_post_disable(dsi->master);

	dsi_write(dsi, DSI_PWR_UP, RESET);
	dsi_write(dsi, DSI_PHY_RSTZ, 0);

	dsi->prepared = false;

	if (dsi->slave)
		dw_mipi_dsi_post_disable(dsi->slave);
}

static int dw_mipi_dsi_connector_init(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	//struct dw_mipi_dsi *dsi = dev_get_priv(conn_state->dev);
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	//dsi->dphy.phy = conn_state->phy;

	conn_state->output_mode = ROCKCHIP_OUT_MODE_P888;
	conn_state->color_space = V4L2_COLORSPACE_DEFAULT;
	conn_state->type = DRM_MODE_CONNECTOR_DSI;
    //printf("zjj.rk3399.uboot dsi:%p , dsi->id %d \n",&dsi , dsi->id);

	return 0;
}

//static void dw_mipi_dsi_set_hs_clk(struct dw_mipi_dsi *dsi, unsigned long rate)
//{
//	rate = rockchip_phy_set_pll(dsi->dphy.phy, rate);
//	dsi->lane_mbps = rate / 1000 / 1000;
//}

static void dw_mipi_dsi_host_init(struct dw_mipi_dsi *dsi)
{
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	//dw_mipi_dsi_init(dsi);
	dsi_write(dsi, DSI_PWR_UP, RESET);
	dsi_write(dsi, DSI_CLKMGR_CFG, 0xa07);

	//dw_mipi_dsi_dpi_config(dsi, &dsi->mode);
	dsi_write(dsi, DSI_DPI_VCID, 0x0);
	dsi_write(dsi, DSI_DPI_COLOR_CODING, 0x05);//MIPI_DSI_FMT_RGB888:DPI_COLOR_CODING_24BIT
	dsi_write(dsi, DSI_DPI_CFG_POL, 0x06);
	dsi_write(dsi, DSI_DPI_LP_CMD_TIM, OUTVACT_LPCMD_TIME(4)| INVACT_LPCMD_TIME(4));
	//dw_mipi_dsi_packet_handler_config(dsi);
	dsi_write(dsi, DSI_PCKHDL_CFG, 0x1c);

	//dw_mipi_dsi_video_mode_config(dsi);
	dsi_write(dsi, DSI_VID_MODE_CFG, 0x3f02);
	//dw_mipi_dsi_video_packet_config(dsi, &dsi->mode);
	dsi_write(dsi, DSI_VID_PKT_SIZE, VID_PKT_SIZE(1088));

	//dw_mipi_dsi_command_mode_config(dsi);
	dsi_write(dsi, DSI_TO_CNT_CFG, HSTX_TO_CNT(1000) | LPRX_TO_CNT(1000));
	dsi_write(dsi, DSI_BTA_TO_CNT, 0xd00);

	dsi_update_bits(dsi, DSI_MODE_CFG, CMD_VIDEO_MODE, COMMAND_MODE);

	//dw_mipi_dsi_line_timer_config(dsi);
	dsi_write(dsi, DSI_VID_HLINE_TIME, 0x55d);
	dsi_write(dsi, DSI_VID_HSA_TIME, 0x04);
	dsi_write(dsi, DSI_VID_HBP_TIME, 0x84);

	//dw_mipi_dsi_vertical_timing_config(dsi);
	dsi_write(dsi, DSI_VID_VACTIVE_LINES, 1920);
	dsi_write(dsi, DSI_VID_VSA_LINES, 0x02);
	dsi_write(dsi, DSI_VID_VFP_LINES, 0x04);
	dsi_write(dsi, DSI_VID_VBP_LINES, 0x03);

	//dw_mipi_dsi_dphy_timing_config(dsi);
	dsi_write(dsi, DSI_PHY_TMR_CFG, PHY_HS2LP_TIME(0x14)
		  | PHY_LP2HS_TIME(0x10) | MAX_RD_TIME(10000));

	dsi_write(dsi, DSI_PHY_TMR_LPCLK_CFG, PHY_CLKHS2LP_TIME(0x40)
		  | PHY_CLKLP2HS_TIME(0x40));

	//dw_mipi_dsi_dphy_interface_config(dsi);
	dsi_write(dsi, DSI_PHY_IF_CFG, PHY_STOP_WAIT_TIME(0x20) |
		  N_LANES(4));

	//dw_mipi_dsi_clear_err(dsi);
	dsi_read(dsi, DSI_INT_ST0);
	dsi_read(dsi, DSI_INT_ST1);
	dsi_write(dsi, DSI_INT_MSK0, 0);
	dsi_write(dsi, DSI_INT_MSK1, 0);

}

static void mipi_dphy_init(struct dw_mipi_dsi *dsi)
{
	u32 map[] = {0x0, 0x1, 0x3, 0x7, 0xf};
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	//mipi_dphy_enableclk_deassert(dsi);
	dsi_update_bits(dsi, DSI_PHY_RSTZ, PHY_ENABLECLK, 0);
	udelay(1);

	//mipi_dphy_shutdownz_assert(dsi);
	dsi_update_bits(dsi, DSI_PHY_RSTZ, PHY_SHUTDOWNZ, 0);
	udelay(1);

	//mipi_dphy_rstz_assert(dsi);
	dsi_update_bits(dsi, DSI_PHY_RSTZ, PHY_RSTZ, 0);
	udelay(1);

	//testif_testclr_assert(dsi);
	dsi_update_bits(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLR, PHY_TESTCLR);
	udelay(1);

	/* Configures DPHY to work as a Master */
	grf_field_write(dsi, MASTERSLAVEZ, 1);

	/* Configures lane as TX */
	grf_field_write(dsi, BASEDIR, 0);

	/* Set all REQUEST inputs to zero */
	grf_field_write(dsi, TURNREQUEST, 0);
	grf_field_write(dsi, TURNDISABLE, 0);
	grf_field_write(dsi, FORCETXSTOPMODE, 0);
	grf_field_write(dsi, FORCERXMODE, 0);
	udelay(1);

	//testif_testclr_deassert(dsi);
	dsi_update_bits(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLR, 0);
	udelay(1);

	//if (!dsi->dphy.phy)
	//	dw_mipi_dsi_phy_init(dsi);
	testif_write(dsi, 0x44, HSFREQRANGE(26));
	testif_write(dsi, 0x60, 0x80 | (996 >> 3) * 60 / 1000);
	testif_write(dsi, 0x70, 0x80 | (996 >> 3) * 60 / 1000);
	testif_write(dsi, 0x19, 0x30);
	testif_write(dsi, 0x17, INPUT_DIV(3));
	testif_write(dsi, 0x18, FEEDBACK_DIV_LO(165));
	testif_write(dsi, 0x18, FEEDBACK_DIV_HI(165 >> 5));

	/* Enable Data Lane Module */
	grf_field_write(dsi, ENABLE_N, map[dsi->lanes]);

	/* Enable Clock Lane Module */
	grf_field_write(dsi, ENABLECLK, 1);

	//mipi_dphy_enableclk_assert(dsi);
	dsi_update_bits(dsi, DSI_PHY_RSTZ, PHY_ENABLECLK, PHY_ENABLECLK);
	udelay(1);

}

static void dw_mipi_dsi_pre_enable(struct dw_mipi_dsi *dsi)
{
	if (dsi->prepared)
		return;
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	if (dsi->master)
		dw_mipi_dsi_pre_enable(dsi->master);

	dw_mipi_dsi_host_init(dsi);
	mipi_dphy_init(dsi);
	mipi_dphy_power_on(dsi);
	dsi_write(dsi, DSI_PWR_UP, POWERUP);

	dsi->prepared = true;

	//if (dsi->slave)
	//	dw_mipi_dsi_pre_enable(dsi->slave);
}

static int dw_mipi_dsi_connector_prepare(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	//struct crtc_state *crtc_state = &state->crtc_state;
	struct dw_mipi_dsi *dsi = dev_get_priv(conn_state->dev);
	//unsigned long lane_rate;
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	memcpy(&dsi->mode, &conn_state->mode, sizeof(struct drm_display_mode));
	//if (dsi->dphy.phy)
	//	dw_mipi_dsi_set_hs_clk(dsi, lane_rate);
	//else
	//dw_mipi_dsi_set_pll(dsi, 1000000000);

	//printf("final DSI-Link lane_rate: lane_rate %ld crtc_id %d\n",lane_rate,crtc_state->crtc_id);

	printf("final DSI-Link bandwidth: %u Mbps x %d\n",
	       dsi->lane_mbps, dsi->slave ? dsi->lanes * 2 : dsi->lanes);

	//dw_mipi_dsi_vop_routing(dsi, 1);
	grf_field_write(dsi, VOPSEL, 1);

	dw_mipi_dsi_pre_enable(dsi);

	return 0;
}

static void dw_mipi_dsi_connector_unprepare(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct dw_mipi_dsi *dsi = dev_get_priv(conn_state->dev);

	dw_mipi_dsi_post_disable(dsi);
}

static int dw_mipi_dsi_connector_enable(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct dw_mipi_dsi *dsi = dev_get_priv(conn_state->dev);
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	dw_mipi_dsi_enable(dsi);

	return 0;
}

static int dw_mipi_dsi_connector_disable(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct dw_mipi_dsi *dsi = dev_get_priv(conn_state->dev);

	dw_mipi_dsi_disable(dsi);

	return 0;
}

static const struct rockchip_connector_funcs dw_mipi_dsi_connector_funcs = {
	.init = dw_mipi_dsi_connector_init,
	.prepare = dw_mipi_dsi_connector_prepare,
	.unprepare = dw_mipi_dsi_connector_unprepare,
	.enable = dw_mipi_dsi_connector_enable,
	.disable = dw_mipi_dsi_connector_disable,
};

static int dw_mipi_dsi_probe(struct udevice *dev)
{
	struct dw_mipi_dsi *dsi = dev_get_priv(dev);
	//const struct rockchip_connector *connector =
	//	(const struct rockchip_connector *)dev_get_driver_data(dev);
	//const struct dw_mipi_dsi_plat_data *pdata = connector->data;
	int id;
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	dsi->base = (void*)0xff960000;
	dsi->grf = (void*)0xff770000;

	id = of_alias_get_id(ofnode_to_np(dev->node), "dsi");
	if (id < 0)
		id = 0;

	//dsi->dev = dev;
	//dsi->pdata = pdata;
	dsi->id = id;

	printf("zjj.rk3399.uboot dw_mipi_dsi_probe dsi->id 0x%d \n", dsi->id);

	return 0;
}



static const u32 rk3399_dsi1_grf_reg_fields[MAX_FIELDS] = {
	[VOPSEL]		= GRF_REG_FIELD(0x6250,  4,  4),
	[DPIUPDATECFG]		= GRF_REG_FIELD(0x6250,  3,  3),
	[DPISHUTDN]		= GRF_REG_FIELD(0x6250,  2,  2),
	[DPICOLORM]		= GRF_REG_FIELD(0x6250,  1,  1),
	[TURNDISABLE]		= GRF_REG_FIELD(0x625c, 12, 15),
	[FORCETXSTOPMODE]	= GRF_REG_FIELD(0x625c,  8, 11),
	[FORCERXMODE]           = GRF_REG_FIELD(0x625c,  4,  7),
	[ENABLE_N]		= GRF_REG_FIELD(0x625c,  0,  3),
	[MASTERSLAVEZ]		= GRF_REG_FIELD(0x6260,  7,  7),
	[ENABLECLK]		= GRF_REG_FIELD(0x6260,  6,  6),
	[BASEDIR]		= GRF_REG_FIELD(0x6260,  5,  5),
	[TURNREQUEST]		= GRF_REG_FIELD(0x6260,  0,  3),
};

static const struct dw_mipi_dsi_plat_data rk3399_mipi_dsi_plat_data = {
	.dsi0_grf_reg_fields = rk3399_dsi0_grf_reg_fields,
	.dsi1_grf_reg_fields = rk3399_dsi1_grf_reg_fields,
	.max_bit_rate_per_lane = 1500000000UL,
};

static const struct rockchip_connector rk3399_mipi_dsi_driver_data = {
	 .funcs = &dw_mipi_dsi_connector_funcs,
	 .data = &rk3399_mipi_dsi_plat_data,
};

static const struct udevice_id dw_mipi_dsi_ids[] = {
	{
		.compatible = "rockchip,rk3399-mipi-dsi",
		.data = (ulong)&rk3399_mipi_dsi_driver_data,
	},
	{}
};

static ssize_t dw_mipi_dsi_host_transfer(struct mipi_dsi_host *host,
					 const struct mipi_dsi_msg *msg)
{
	struct dw_mipi_dsi *dsi = dev_get_priv(host->dev);

	return dw_mipi_dsi_transfer(dsi, msg);
}

static int dw_mipi_dsi_host_attach(struct mipi_dsi_host *host,
				   struct mipi_dsi_device *device)
{
	struct dw_mipi_dsi *dsi = dev_get_priv(host->dev);

	if (device->lanes < 1 || device->lanes > 8)
		return -EINVAL;
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	dsi->lanes = device->lanes;
	dsi->channel = device->channel;
	dsi->format = device->format;
	dsi->mode_flags = device->mode_flags;

	return 0;
}

static const struct mipi_dsi_host_ops dw_mipi_dsi_host_ops = {
	.attach = dw_mipi_dsi_host_attach,
	.transfer = dw_mipi_dsi_host_transfer,
};

static int dw_mipi_dsi_bind(struct udevice *dev)
{
	struct mipi_dsi_host *host = dev_get_platdata(dev);

	host->dev = dev;
	host->ops = &dw_mipi_dsi_host_ops;

	return dm_scan_fdt_dev(dev);
}

static int dw_mipi_dsi_child_post_bind(struct udevice *dev)
{
	struct mipi_dsi_host *host = dev_get_platdata(dev->parent);
	struct mipi_dsi_device *device = dev_get_parent_platdata(dev);
	char name[20];

	sprintf(name, "%s.%d", host->dev->name, device->channel);
	device_set_name(dev, name);

	device->dev = dev;
	device->host = host;
	device->lanes = dev_read_u32_default(dev, "dsi,lanes", 4);
	device->format = dev_read_u32_default(dev, "dsi,format",
					      MIPI_DSI_FMT_RGB888);
	device->mode_flags = dev_read_u32_default(dev, "dsi,flags",
						  MIPI_DSI_MODE_VIDEO |
						  MIPI_DSI_MODE_VIDEO_BURST |
						  MIPI_DSI_MODE_VIDEO_HBP |
						  MIPI_DSI_MODE_LPM |
						  MIPI_DSI_MODE_EOT_PACKET);
	device->channel = dev_read_u32_default(dev, "reg", 0);

	return 0;
}

static int dw_mipi_dsi_child_pre_probe(struct udevice *dev)
{
	struct mipi_dsi_device *device = dev_get_parent_platdata(dev);
	int ret;
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	ret = mipi_dsi_attach(device);
	if (ret) {
		dev_err(dev, "mipi_dsi_attach() failed: %d\n", ret);
		return ret;
	}

	return 0;
}

U_BOOT_DRIVER(dw_mipi_dsi) = {
	.name = "dw_mipi_dsi",
	.id = UCLASS_DISPLAY,
	.of_match = dw_mipi_dsi_ids,
	.probe = dw_mipi_dsi_probe,
	.bind = dw_mipi_dsi_bind,
	.priv_auto_alloc_size = sizeof(struct dw_mipi_dsi),
	.per_child_platdata_auto_alloc_size = sizeof(struct mipi_dsi_device),
	.platdata_auto_alloc_size = sizeof(struct mipi_dsi_host),
	.child_post_bind = dw_mipi_dsi_child_post_bind,
	.child_pre_probe = dw_mipi_dsi_child_pre_probe,
};
