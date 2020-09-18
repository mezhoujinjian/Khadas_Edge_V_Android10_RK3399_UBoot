/*
 * (C) Copyright 2008-2017 Fuzhou Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <common.h>
#include <errno.h>
#include <malloc.h>
#include <fdtdec.h>
#include <fdt_support.h>
#include <asm/unaligned.h>
#include <asm/io.h>
#include <linux/list.h>
#include <linux/media-bus-format.h>
#include <clk.h>
#include <asm/arch/clock.h>
#include <linux/err.h>
#include <dm/device.h>
#include <dm/read.h>
#include <syscon.h>

#include "rockchip_display.h"
#include "rockchip_crtc.h"
#include "rockchip_connector.h"
#include "rockchip_vop.h"
#include "zhoujinjian_mipi_rockchip_display.h"

static inline void vop_writel(struct vop *vop, uint32_t offset, uint32_t v)
{
	writel(v, vop->regs + offset);
	vop->regsbak[offset >> 2] = v;
}

static inline uint32_t vop_readl(struct vop *vop, uint32_t offset)
{
	return readl(vop->regs + offset);
}

static inline uint32_t vop_read_reg(struct vop *vop, uint32_t base,
				    const struct vop_reg *reg)
{
	return (vop_readl(vop, base + reg->offset) >> reg->shift) & reg->mask;
}

static inline void vop_mask_write(struct vop *vop, uint32_t offset,
				  uint32_t mask, uint32_t shift, uint32_t v,
				  bool write_mask)
{
	if (!mask)
		return;

	if (write_mask) {
	    //printf("zjj.rk3399.uboot vop_mask_write if (v:0x%x & mask:0x%x) << shift:0x%x) | (mask:0x%x << (shift:0x%x + 16)\n", v, mask, shift, mask, shift);

		v = ((v & mask) << shift) | (mask << (shift + 16));
	} else {
		uint32_t cached_val = vop->regsbak[offset >> 2];
	    //printf("zjj.rk3399.uboot vop_mask_write else (cached_val:0x%x & ~(mask:0x%x << shift:0x%x)) | ((v:0x%x & mask:0x%x) << shift:0x%x)\n", cached_val, mask, shift, v, mask, shift);

		v = (cached_val & ~(mask << shift)) | ((v & mask) << shift);
		vop->regsbak[offset >> 2] = v;
	}

	//printf("zjj.rk3399.uboot vop_mask_write vop->regs:%p offset:0x%x v:0x%x\n", vop->regs ,offset, v);

	writel(v, vop->regs + offset);
}

static inline void vop_cfg_done(struct vop *vop)
{
	VOP_CTRL_SET(vop, cfg_done, 1);
}

static inline void vop_grf_writel(struct vop *vop, struct vop_reg reg, u32 v)
{
	u32 val = 0;

	if (VOP_REG_SUPPORT(vop, reg)) {
		val = (v << reg.shift) | (reg.mask << (reg.shift + 16));
		writel(val, vop->grf + reg.offset);
	}
}

static int rockchip_vop_preinit(struct display_state *state)
{
	const struct vop_data *vop_data = state->crtc_state.crtc->data;
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	state->crtc_state.max_output = vop_data->max_output;

	return 0;
}

static int rockchip_vop_init(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	struct connector_state *conn_state = &state->conn_state;
	struct drm_display_mode *mode = &conn_state->mode;
	const struct rockchip_crtc *crtc = crtc_state->crtc;
	const struct vop_data *vop_data = crtc->data;
	struct vop *vop;
/*	u16 hsync_len = mode->crtc_hsync_end - mode->crtc_hsync_start;
	u16 hdisplay = mode->crtc_hdisplay;
	u16 htotal = mode->crtc_htotal;
	u16 hact_st = mode->crtc_htotal - mode->crtc_hsync_start;
	u16 hact_end = hact_st + hdisplay;
	u16 vdisplay = mode->crtc_vdisplay;
	u16 vtotal = mode->crtc_vtotal;
	u16 vsync_len = mode->crtc_vsync_end - mode->crtc_vsync_start;
	u16 vact_st = mode->crtc_vtotal - mode->crtc_vsync_start;
	u16 vact_end = vact_st + vdisplay;*/
	struct clk dclk;
	//u32 val, act_end;
	int ret;
	//bool yuv_overlay = false, post_r2y_en = false, post_y2r_en = false;
	//u16 post_csc_mode;
	//bool dclk_inv;
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	vop = malloc(sizeof(*vop));
	if (!vop)
		return -ENOMEM;
	memset(vop, 0, sizeof(*vop));

	crtc_state->private = vop;

	vop->regs = dev_read_addr_ptr(crtc_state->dev);
	printf("vop->regs ret=%p\n", vop->regs);

	vop->regsbak = malloc(vop_data->reg_len);
	vop->win = vop_data->win;
	vop->win_offset = vop_data->win_offset;
	vop->ctrl = vop_data->ctrl;
	vop->grf = syscon_get_first_range(ROCKCHIP_SYSCON_GRF);
	printf("vop->grf ret=%p\n",vop->grf);

	if (vop->grf <= 0)
		printf("%s: Get syscon grf failed (ret=%p)\n",
		      __func__, vop->grf);

	vop->grf_ctrl = vop_data->grf_ctrl;
	vop->line_flag = vop_data->line_flag;
	vop->csc_table = vop_data->csc_table;
	vop->win_csc = vop_data->win_csc;
	vop->version = vop_data->version;

	/* Process 'assigned-{clocks/clock-parents/clock-rates}' properties */
	ret = clk_set_defaults(crtc_state->dev);
	if (ret)
		debug("%s clk_set_defaults failed %d\n", __func__, ret);

	ret = clk_get_by_name(crtc_state->dev, "dclk_vop", &dclk);
	printf("mode->clock ret=%d\n",mode->clock);
	if (!ret)
		ret = clk_set_rate(&dclk, mode->clock * 1000);
	if (IS_ERR_VALUE(ret)) {
		printf("%s: Failed to set dclk: ret=%d\n", __func__, ret);
		return ret;
	}

	memcpy(vop->regsbak, vop->regs, vop_data->reg_len);

	//rockchip_vop_init_gamma(vop, state);

	zjj_rockchip_vop_init(vop->regs, vop->regsbak, vop_data->reg_len);

	return 0;
}

static int rockchip_vop_set_plane(struct display_state *state)
{
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);
	zjj_rockchip_vop_set_plane();

	return 0;
}

static int rockchip_vop_prepare(struct display_state *state)
{
	return 0;
}

static int rockchip_vop_enable(struct display_state *state)
{
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);
	zjj_rockchip_vop_enable();
	return 0;
}

static int rockchip_vop_disable(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	struct vop *vop = crtc_state->private;
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	VOP_CTRL_SET(vop, standby, 1);
	vop_cfg_done(vop);
	return 0;
}

static int rockchip_vop_fixup_dts(struct display_state *state, void *blob)
{
#if 0
	struct crtc_state *crtc_state = &state->crtc_state;
	struct panel_state *pstate = &state->panel_state;
	uint32_t phandle;
	char path[100];
	int ret, dsp_lut_node;

	if (!ofnode_valid(pstate->dsp_lut_node))
		return 0;
	ret = fdt_get_path(state->blob, pstate->dsp_lut_node, path, sizeof(path));
	if (ret < 0) {
		printf("failed to get dsp_lut path[%s], ret=%d\n",
			path, ret);
		return ret;
	}

	dsp_lut_node = fdt_path_offset(blob, path);
	phandle = fdt_get_phandle(blob, dsp_lut_node);
	if (!phandle) {
		phandle = fdt_alloc_phandle(blob);
		if (!phandle) {
			printf("failed to alloc phandle\n");
			return -ENOMEM;
		}

		fdt_set_phandle(blob, dsp_lut_node, phandle);
	}

	ret = fdt_get_path(state->blob, crtc_state->node, path, sizeof(path));
	if (ret < 0) {
		printf("failed to get route path[%s], ret=%d\n",
			path, ret);
		return ret;
	}

	do_fixup_by_path_u32(blob, path, "dsp-lut", phandle, 1);
#endif
	return 0;
}

static int rockchip_vop_send_mcu_cmd(struct display_state *state,
				     u32 type, u32 value)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	struct vop *vop = crtc_state->private;

	if (vop) {
		switch (type) {
		case MCU_WRCMD:
			VOP_CTRL_SET(vop, mcu_rs, 0);
			VOP_CTRL_SET(vop, mcu_rw_bypass_port, value);
			VOP_CTRL_SET(vop, mcu_rs, 1);
			break;
		case MCU_WRDATA:
			VOP_CTRL_SET(vop, mcu_rs, 1);
			VOP_CTRL_SET(vop, mcu_rw_bypass_port, value);
			break;
		case MCU_SETBYPASS:
			VOP_CTRL_SET(vop, mcu_bypass, value ? 1 : 0);
			break;
		default:
			break;
		}
	}

	return 0;
}

const struct rockchip_crtc_funcs rockchip_vop_funcs = {
	.preinit = rockchip_vop_preinit,
	.init = rockchip_vop_init,
	.set_plane = rockchip_vop_set_plane,
	.prepare = rockchip_vop_prepare,
	.enable = rockchip_vop_enable,
	.disable = rockchip_vop_disable,
	.fixup_dts = rockchip_vop_fixup_dts,
	.send_mcu_cmd = rockchip_vop_send_mcu_cmd,
};
