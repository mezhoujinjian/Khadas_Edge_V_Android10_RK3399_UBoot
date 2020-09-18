/*
 * (C) Copyright 2008-2017 Fuzhou Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _ZHOUJINJIAN_MIPI_ROCKCHIP_DISPLAY_H
#define _ZHOUJINJIAN_MIPI_ROCKCHIP_DISPLAY_H
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

void zjj_rockchip_vop_init(void * regs,u32 * regsbak,int len);
void zjj_rockchip_vop_set_plane(void);
void zjj_rockchip_vop_enable(void);
//void rkclk_set_pll(void);

int zjj_lcd_test(void);

#endif
