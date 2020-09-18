/***********************************************************************
 *  zhoujinjian
 **********************************************************************/
#include "zhoujinjian_mipi_rockchip_display.h"

#define RK3399_REG_CFG_DONE			0x0000
#define RK3399_VERSION_INFO			0x0004
#define RK3399_SYS_CTRL				0x0008
#define RK3399_SYS_CTRL1			0x000c
#define RK3399_DSP_CTRL0			0x0010
#define RK3399_DSP_CTRL1			0x0014
#define RK3399_DSP_BG				0x0018
#define RK3399_MCU_CTRL				0x001c
#define RK3399_WB_CTRL0				0x0020
#define RK3399_WB_CTRL1				0x0024
#define RK3399_WB_YRGB_MST			0x0028
#define RK3399_WB_CBR_MST			0x002c
#define RK3399_WIN0_CTRL0			0x0030
#define RK3399_WIN0_CTRL1			0x0034
#define RK3399_WIN0_COLOR_KEY			0x0038
#define RK3399_WIN0_VIR				0x003c
#define RK3399_WIN0_YRGB_MST			0x0040
#define RK3399_WIN0_CBR_MST			0x0044
#define RK3399_WIN0_ACT_INFO			0x0048
#define RK3399_WIN0_DSP_INFO			0x004c
#define RK3399_WIN0_DSP_ST			0x0050
#define RK3399_WIN0_SCL_FACTOR_YRGB		0x0054
#define RK3399_WIN0_SCL_FACTOR_CBR		0x0058
#define RK3399_WIN0_SCL_OFFSET			0x005c
#define RK3399_WIN0_SRC_ALPHA_CTRL		0x0060
#define RK3399_WIN0_DST_ALPHA_CTRL		0x0064
#define RK3399_WIN0_FADING_CTRL			0x0068
#define RK3399_WIN0_CTRL2			0x006c
#define RK3399_WIN1_CTRL0			0x0070
#define RK3399_WIN1_CTRL1			0x0074
#define RK3399_WIN1_COLOR_KEY			0x0078
#define RK3399_WIN1_VIR				0x007c
#define RK3399_WIN1_YRGB_MST			0x0080
#define RK3399_WIN1_CBR_MST			0x0084
#define RK3399_WIN1_ACT_INFO			0x0088
#define RK3399_WIN1_DSP_INFO			0x008c
#define RK3399_WIN1_DSP_ST			0x0090
#define RK3399_WIN1_SCL_FACTOR_YRGB		0x0094
#define RK3399_WIN1_SCL_FACTOR_CBR		0x0098
#define RK3399_WIN1_SCL_OFFSET			0x009c
#define RK3399_WIN1_SRC_ALPHA_CTRL		0x00a0
#define RK3399_WIN1_DST_ALPHA_CTRL		0x00a4
#define RK3399_WIN1_FADING_CTRL			0x00a8
#define RK3399_WIN1_CTRL2			0x00ac
#define RK3399_WIN2_CTRL0			0x00b0
#define RK3399_WIN2_CTRL1			0x00b4
#define RK3399_WIN2_VIR0_1			0x00b8
#define RK3399_WIN2_VIR2_3			0x00bc
#define RK3399_WIN2_MST0			0x00c0
#define RK3399_WIN2_DSP_INFO0			0x00c4
#define RK3399_WIN2_DSP_ST0			0x00c8
#define RK3399_WIN2_COLOR_KEY			0x00cc
#define RK3399_WIN2_MST1			0x00d0
#define RK3399_WIN2_DSP_INFO1			0x00d4
#define RK3399_WIN2_DSP_ST1			0x00d8
#define RK3399_WIN2_SRC_ALPHA_CTRL		0x00dc
#define RK3399_WIN2_MST2			0x00e0
#define RK3399_WIN2_DSP_INFO2			0x00e4
#define RK3399_WIN2_DSP_ST2			0x00e8
#define RK3399_WIN2_DST_ALPHA_CTRL		0x00ec
#define RK3399_WIN2_MST3			0x00f0
#define RK3399_WIN2_DSP_INFO3			0x00f4
#define RK3399_WIN2_DSP_ST3			0x00f8
#define RK3399_WIN2_FADING_CTRL			0x00fc
#define RK3399_WIN3_CTRL0			0x0100
#define RK3399_WIN3_CTRL1			0x0104
#define RK3399_WIN3_VIR0_1			0x0108
#define RK3399_WIN3_VIR2_3			0x010c
#define RK3399_WIN3_MST0			0x0110
#define RK3399_WIN3_DSP_INFO0			0x0114
#define RK3399_WIN3_DSP_ST0			0x0118
#define RK3399_WIN3_COLOR_KEY			0x011c
#define RK3399_WIN3_MST1			0x0120
#define RK3399_WIN3_DSP_INFO1			0x0124
#define RK3399_WIN3_DSP_ST1			0x0128
#define RK3399_WIN3_SRC_ALPHA_CTRL		0x012c
#define RK3399_WIN3_MST2			0x0130
#define RK3399_WIN3_DSP_INFO2			0x0134
#define RK3399_WIN3_DSP_ST2			0x0138
#define RK3399_WIN3_DST_ALPHA_CTRL		0x013c
#define RK3399_WIN3_MST3			0x0140
#define RK3399_WIN3_DSP_INFO3			0x0144
#define RK3399_WIN3_DSP_ST3			0x0148
#define RK3399_WIN3_FADING_CTRL			0x014c
#define RK3399_HWC_CTRL0			0x0150
#define RK3399_HWC_CTRL1			0x0154
#define RK3399_HWC_MST				0x0158
#define RK3399_HWC_DSP_ST			0x015c
#define RK3399_HWC_SRC_ALPHA_CTRL		0x0160
#define RK3399_HWC_DST_ALPHA_CTRL		0x0164
#define RK3399_HWC_FADING_CTRL			0x0168
#define RK3399_HWC_RESERVED1			0x016c
#define RK3399_POST_DSP_HACT_INFO		0x0170
#define RK3399_POST_DSP_VACT_INFO		0x0174
#define RK3399_POST_SCL_FACTOR_YRGB		0x0178
#define RK3399_POST_RESERVED			0x017c
#define RK3399_POST_SCL_CTRL			0x0180
#define RK3399_POST_DSP_VACT_INFO_F1		0x0184
#define RK3399_DSP_HTOTAL_HS_END		0x0188
#define RK3399_DSP_HACT_ST_END			0x018c
#define RK3399_DSP_VTOTAL_VS_END		0x0190
#define RK3399_DSP_VACT_ST_END			0x0194
#define RK3399_DSP_VS_ST_END_F1			0x0198
#define RK3399_DSP_VACT_ST_END_F1		0x019c
#define RK3399_PWM_CTRL				0x01a0
#define RK3399_PWM_PERIOD_HPR			0x01a4
#define RK3399_PWM_DUTY_LPR			0x01a8
#define RK3399_PWM_CNT				0x01ac
#define RK3399_BCSH_COLOR_BAR			0x01b0
#define RK3399_BCSH_BCS				0x01b4
#define RK3399_BCSH_H				0x01b8
#define RK3399_BCSH_CTRL			0x01bc
#define RK3399_CABC_CTRL0			0x01c0
#define RK3399_CABC_CTRL1			0x01c4
#define RK3399_CABC_CTRL2			0x01c8
#define RK3399_CABC_CTRL3			0x01cc
#define RK3399_CABC_GAUSS_LINE0_0		0x01d0
#define RK3399_CABC_GAUSS_LINE0_1		0x01d4
#define RK3399_CABC_GAUSS_LINE1_0		0x01d8
#define RK3399_CABC_GAUSS_LINE1_1		0x01dc
#define RK3399_CABC_GAUSS_LINE2_0		0x01e0
#define RK3399_CABC_GAUSS_LINE2_1		0x01e4
#define RK3399_FRC_LOWER01_0			0x01e8
#define RK3399_FRC_LOWER01_1			0x01ec
#define RK3399_FRC_LOWER10_0			0x01f0
#define RK3399_FRC_LOWER10_1			0x01f4
#define RK3399_FRC_LOWER11_0			0x01f8
#define RK3399_FRC_LOWER11_1			0x01fc
#define RK3399_AFBCD0_CTRL			0x0200
#define RK3399_AFBCD0_HDR_PTR			0x0204
#define RK3399_AFBCD0_PIC_SIZE			0x0208
#define RK3399_AFBCD0_STATUS			0x020c
#define RK3399_AFBCD1_CTRL			0x0220
#define RK3399_AFBCD1_HDR_PTR			0x0224
#define RK3399_AFBCD1_PIC_SIZE			0x0228
#define RK3399_AFBCD1_STATUS			0x022c
#define RK3399_AFBCD2_CTRL			0x0240
#define RK3399_AFBCD2_HDR_PTR			0x0244
#define RK3399_AFBCD2_PIC_SIZE			0x0248
#define RK3399_AFBCD2_STATUS			0x024c
#define RK3399_AFBCD3_CTRL			0x0260
#define RK3399_AFBCD3_HDR_PTR			0x0264
#define RK3399_AFBCD3_PIC_SIZE			0x0268
#define RK3399_AFBCD3_STATUS			0x026c
#define RK3399_INTR_EN0				0x0280
#define RK3399_INTR_CLEAR0			0x0284
#define RK3399_INTR_STATUS0			0x0288
#define RK3399_INTR_RAW_STATUS0			0x028c
#define RK3399_INTR_EN1				0x0290
#define RK3399_INTR_CLEAR1			0x0294
#define RK3399_INTR_STATUS1			0x0298
#define RK3399_INTR_RAW_STATUS1			0x029c
#define RK3399_LINE_FLAG			0x02a0
#define RK3399_VOP_STATUS			0x02a4
#define RK3399_BLANKING_VALUE			0x02a8
#define RK3399_MCU_BYPASS_PORT			0x02ac
#define RK3399_WIN0_DSP_BG			0x02b0
#define RK3399_WIN1_DSP_BG			0x02b4
#define RK3399_WIN2_DSP_BG			0x02b8
#define RK3399_WIN3_DSP_BG			0x02bc
#define RK3399_YUV2YUV_WIN			0x02c0
#define RK3399_YUV2YUV_POST			0x02c4
#define RK3399_AUTO_GATING_EN			0x02cc
#define RK3399_WIN0_CSC_COE			0x03a0
#define RK3399_WIN1_CSC_COE			0x03c0
#define RK3399_WIN2_CSC_COE			0x03e0
#define RK3399_WIN3_CSC_COE			0x0400
#define RK3399_HWC_CSC_COE			0x0420
#define RK3399_BCSH_R2Y_CSC_COE			0x0440
#define RK3399_BCSH_Y2R_CSC_COE			0x0460
#define RK3399_POST_YUV2YUV_Y2R_COE		0x0480
#define RK3399_POST_YUV2YUV_3X3_COE		0x04a0
#define RK3399_POST_YUV2YUV_R2Y_COE		0x04c0
#define RK3399_WIN0_YUV2YUV_Y2R			0x04e0
#define RK3399_WIN0_YUV2YUV_3X3			0x0500
#define RK3399_WIN0_YUV2YUV_R2Y			0x0520
#define RK3399_WIN1_YUV2YUV_Y2R			0x0540
#define RK3399_WIN1_YUV2YUV_3X3			0x0560
#define RK3399_WIN1_YUV2YUV_R2Y			0x0580
#define RK3399_WIN2_YUV2YUV_Y2R			0x05a0
#define RK3399_WIN2_YUV2YUV_3X3			0x05c0
#define RK3399_WIN2_YUV2YUV_R2Y			0x05e0
#define RK3399_WIN3_YUV2YUV_Y2R			0x0600
#define RK3399_WIN3_YUV2YUV_3X3			0x0620
#define RK3399_WIN3_YUV2YUV_R2Y			0x0640
#define RK3399_WIN2_LUT_ADDR			0x1000
#define RK3399_WIN3_LUT_ADDR			0x1400
#define RK3399_HWC_LUT_ADDR			0x1800
#define RK3399_CABC_GAMMA_LUT_ADDR		0x1c00
#define RK3399_GAMMA_LUT_ADDR			0x2000
/* rk3399 register definition end */

#define VOP_REG_VER_MASK(off, _mask, s, _write_mask, _major, \
		         _begin_minor, _end_minor) \
		{.offset = off, \
		 .mask = _mask, \
		 .shift = s, \
		 .write_mask = _write_mask, \
		 .major = _major, \
		 .begin_minor = _begin_minor, \
		 .end_minor = _end_minor,}

#define VOP_REG(off, _mask, s) \
		VOP_REG_VER_MASK(off, _mask, s, false, 0, 0, -1)

#define VOP_REG_MASK(off, _mask, s) \
		VOP_REG_VER_MASK(off, _mask, s, true, 0, 0, -1)

#define VOP_REG_VER(off, _mask, s, _major, _begin_minor, _end_minor) \
		VOP_REG_VER_MASK(off, _mask, s, false, \
				 _major, _begin_minor, _end_minor)

struct zjj_vop_reg {
	uint32_t mask;
	uint32_t offset:12;
	uint32_t shift:5;
	uint32_t begin_minor:4;
	uint32_t end_minor:4;
	uint32_t major:3;
	uint32_t write_mask:1;
};
/* zjj_rockchip_vop_init */
struct zjj_vop_reg global_regdone_en = VOP_REG_VER(RK3399_SYS_CTRL, 0x1, 11, 3, 2, -1);
struct zjj_vop_reg axi_outstanding_max_num = VOP_REG(RK3399_SYS_CTRL1, 0x1f, 13);
struct zjj_vop_reg axi_max_outstanding_en = VOP_REG(RK3399_SYS_CTRL1, 0x1, 12);
struct zjj_vop_reg win_gate0 = VOP_REG(RK3399_WIN2_CTRL0, 0x1, 0);
struct zjj_vop_reg win_gate1 = VOP_REG(RK3399_WIN3_CTRL0, 0x1, 0);
struct zjj_vop_reg dsp_blank = VOP_REG(RK3399_DSP_CTRL0, 0x3, 18);
struct zjj_vop_reg mipi_en = VOP_REG(RK3399_SYS_CTRL, 0x1, 15);
struct zjj_vop_reg mipi_pin_pol = VOP_REG_VER(RK3399_DSP_CTRL1, 0x7, 28, 3, 2, -1);
struct zjj_vop_reg mipi_dclk_pol = VOP_REG_VER(RK3399_DSP_CTRL1, 0x1, 31, 3, 2, -1);
struct zjj_vop_reg mipi_dual_channel_en = VOP_REG(RK3399_SYS_CTRL, 0x1, 3);
struct zjj_vop_reg data01_swap = VOP_REG_VER(RK3399_SYS_CTRL, 0x1, 17, 3, 5, -1);
struct zjj_vop_reg dither_down = VOP_REG(RK3399_DSP_CTRL1, 0xf, 1);
struct zjj_vop_reg dclk_ddr = VOP_REG_VER(RK3399_DSP_CTRL0, 0x1, 8, 3, 4, -1);
struct zjj_vop_reg dsp_data_swap = VOP_REG(RK3399_DSP_CTRL0, 0x1f, 12);
struct zjj_vop_reg out_mode = VOP_REG(RK3399_DSP_CTRL0, 0xf, 0);
struct zjj_vop_reg overlay_mode = VOP_REG_VER(RK3399_SYS_CTRL, 0x1, 16, 3, 2, -1);
struct zjj_vop_reg dsp_out_yuv = VOP_REG_VER(RK3399_POST_SCL_CTRL, 0x1, 2, 3, 5, -1);
struct zjj_vop_reg bcsh_r2y_en = VOP_REG_VER(RK3399_BCSH_CTRL, 0x1, 4, 3, 1, -1);
struct zjj_vop_reg bcsh_y2r_en = VOP_REG_VER(RK3399_BCSH_CTRL, 0x1, 0, 3, 1, -1);
struct zjj_vop_reg bcsh_r2y_csc_mode = VOP_REG_VER(RK3399_BCSH_CTRL, 0x1, 6, 3, 1, -1);
struct zjj_vop_reg bcsh_y2r_csc_mode = VOP_REG_VER(RK3399_BCSH_CTRL, 0x3, 2, 3, 1, -1);
struct zjj_vop_reg dsp_background = VOP_REG(RK3399_DSP_BG, 0xffffffff, 0);
struct zjj_vop_reg htotal_pw = VOP_REG(RK3399_DSP_HTOTAL_HS_END, 0x1fff1fff, 0);
struct zjj_vop_reg hact_st_end = VOP_REG(RK3399_DSP_HACT_ST_END, 0x1fff1fff, 0);
struct zjj_vop_reg vact_st_end = VOP_REG(RK3399_DSP_VACT_ST_END, 0x1fff1fff, 0);
struct zjj_vop_reg dsp_interlace = VOP_REG(RK3399_DSP_CTRL0, 0x1, 10);
struct zjj_vop_reg p2i_en = VOP_REG_VER(RK3399_DSP_CTRL0, 0x1, 5, 3, 4, -1);
struct zjj_vop_reg vtotal_pw = VOP_REG(RK3399_DSP_VTOTAL_VS_END, 0x1fff1fff, 0);
struct zjj_vop_reg hpost_st_end = VOP_REG(RK3399_POST_DSP_HACT_INFO, 0x1fff1fff, 0);
struct zjj_vop_reg vpost_st_end = VOP_REG(RK3399_POST_DSP_VACT_INFO, 0x1fff1fff, 0);
struct zjj_vop_reg post_scl_factor = VOP_REG(RK3399_POST_SCL_FACTOR_YRGB, 0xffffffff, 0);
struct zjj_vop_reg post_scl_ctrl = VOP_REG(RK3399_POST_SCL_CTRL, 0x3, 0);
struct zjj_vop_reg core_dclk_div = VOP_REG_VER(RK3399_DSP_CTRL0, 0x1, 4, 3, 4, -1);
struct zjj_vop_reg line_flag_num0 = VOP_REG(RK3399_LINE_FLAG, 0xffff, 0);
struct zjj_vop_reg line_flag_num1 = VOP_REG(RK3399_LINE_FLAG, 0xffff, 16);

/* zjj_rockchip_vop_set_plane*/
struct zjj_vop_reg ymirror = VOP_REG(RK3399_DSP_CTRL0, 0x1, 23);
struct zjj_vop_reg xmirror = VOP_REG(RK3399_DSP_CTRL0, 0x1, 22);
struct zjj_vop_reg format = VOP_REG(RK3399_WIN2_CTRL0, 0x3, 5);
struct zjj_vop_reg yrgb_vir = VOP_REG(RK3399_WIN2_VIR0_1, 0x1fff, 0);
struct zjj_vop_reg yrgb_mst = VOP_REG(RK3399_WIN2_MST0, 0xffffffff, 0);
struct zjj_vop_reg act_info = VOP_REG(RK3399_WIN0_ACT_INFO, 0x1fff1fff, 0);
struct zjj_vop_reg dsp_info = VOP_REG(RK3399_WIN2_DSP_INFO0, 0x0fff0fff, 0);
struct zjj_vop_reg dsp_st = VOP_REG(RK3399_WIN2_DSP_ST0, 0x1fff1fff, 0);
struct zjj_vop_reg rb_swap = VOP_REG(RK3399_WIN2_CTRL0, 0x1, 20);
struct zjj_vop_reg src_alpha_ctl = VOP_REG(RK3399_WIN2_SRC_ALPHA_CTRL, 0xffff, 0);
struct zjj_vop_reg enable = VOP_REG(RK3399_WIN2_CTRL0, 0x1, 4);
/* enable */
struct zjj_vop_reg standby = VOP_REG(RK3399_SYS_CTRL, 0x1, 22);

/* cfg_done */
struct zjj_vop_reg cfg_done = VOP_REG(RK3399_REG_CFG_DONE, 0x1, 0);

/* CRU */
 
#define CRU           0xFF760000   
#define CRU_CPLL_CON0 (*(volatile unsigned long *)(CRU + 0x0060))
#define CRU_CPLL_CON1 (*(volatile unsigned long *)(CRU + 0x0064))
#define CRU_CPLL_CON2 (*(volatile unsigned long *)(CRU + 0x0068))
#define CRU_CPLL_CON3 (*(volatile unsigned long *)(CRU + 0x006c))

//Rockchip+RK3399TRM+V1.3+Part1.pdf P136
#define aclkreg_addr (*(volatile unsigned long *)(CRU + 0x01c0)) //CRU_CLKSEL_CON48  
#define dclkreg_addr (*(volatile unsigned long *)(CRU + 0x01c8))  //CRU_CLKSEL_CON50 


/*X:\u-boot\drivers\clk\rockchip\clk_rk3399.c start*/
enum {
	/* PLL_CON0 */
	PLL_FBDIV_MASK			= 0xfff,
	PLL_FBDIV_SHIFT			= 0,

	/* PLL_CON1 */
	PLL_POSTDIV2_SHIFT		= 12,
	PLL_POSTDIV2_MASK		= 0x7 << PLL_POSTDIV2_SHIFT,
	PLL_POSTDIV1_SHIFT		= 8,
	PLL_POSTDIV1_MASK		= 0x7 << PLL_POSTDIV1_SHIFT,
	PLL_REFDIV_MASK			= 0x3f,
	PLL_REFDIV_SHIFT		= 0,

	/* PLL_CON2 */
	PLL_LOCK_STATUS_SHIFT		= 31,
	PLL_LOCK_STATUS_MASK		= 1 << PLL_LOCK_STATUS_SHIFT,
	PLL_FRACDIV_MASK		= 0xffffff,
	PLL_FRACDIV_SHIFT		= 0,

	/* PLL_CON3 */
	PLL_MODE_SHIFT			= 8,
	PLL_MODE_MASK			= 3 << PLL_MODE_SHIFT,
	PLL_MODE_SLOW			= 0,
	PLL_MODE_NORM,
	PLL_MODE_DEEP,
	PLL_DSMPD_SHIFT			= 3,
	PLL_DSMPD_MASK			= 1 << PLL_DSMPD_SHIFT,
	PLL_INTEGER_MODE		= 1,

	/* PMUCRU_CLKSEL_CON0 */
	PMU_PCLK_DIV_CON_MASK		= 0x1f,
	PMU_PCLK_DIV_CON_SHIFT		= 0,

	/* PMUCRU_CLKSEL_CON1 */
	SPI3_PLL_SEL_SHIFT		= 7,
	SPI3_PLL_SEL_MASK		= 1 << SPI3_PLL_SEL_SHIFT,
	SPI3_PLL_SEL_24M		= 0,
	SPI3_PLL_SEL_PPLL		= 1,
	SPI3_DIV_CON_SHIFT		= 0x0,
	SPI3_DIV_CON_MASK		= 0x7f,

	/* PMUCRU_CLKSEL_CON2 */
	I2C_DIV_CON_MASK		= 0x7f,
	CLK_I2C8_DIV_CON_SHIFT		= 8,
	CLK_I2C0_DIV_CON_SHIFT		= 0,

	/* PMUCRU_CLKSEL_CON3 */
	CLK_I2C4_DIV_CON_SHIFT		= 0,

	/* CLKSEL_CON0 / CLKSEL_CON2 */
	ACLKM_CORE_DIV_CON_MASK	= 0x1f,
	ACLKM_CORE_DIV_CON_SHIFT	= 8,
	CLK_CORE_PLL_SEL_MASK		= 3,
	CLK_CORE_PLL_SEL_SHIFT		= 6,
	CLK_CORE_PLL_SEL_ALPLL		= 0x0,
	CLK_CORE_PLL_SEL_ABPLL		= 0x1,
	CLK_CORE_PLL_SEL_DPLL		= 0x10,
	CLK_CORE_PLL_SEL_GPLL		= 0x11,
	CLK_CORE_DIV_MASK		= 0x1f,
	CLK_CORE_DIV_SHIFT		= 0,

	/* CLKSEL_CON1 / CLKSEL_CON3 */
	PCLK_DBG_DIV_MASK		= 0x1f,
	PCLK_DBG_DIV_SHIFT		= 0x8,
	ATCLK_CORE_DIV_MASK		= 0x1f,
	ATCLK_CORE_DIV_SHIFT		= 0,

	/* CLKSEL_CON14 */
	PCLK_PERIHP_DIV_CON_SHIFT	= 12,
	PCLK_PERIHP_DIV_CON_MASK	= 0x7 << PCLK_PERIHP_DIV_CON_SHIFT,
	HCLK_PERIHP_DIV_CON_SHIFT	= 8,
	HCLK_PERIHP_DIV_CON_MASK	= 3 << HCLK_PERIHP_DIV_CON_SHIFT,
	ACLK_PERIHP_PLL_SEL_SHIFT	= 7,
	ACLK_PERIHP_PLL_SEL_MASK	= 1 << ACLK_PERIHP_PLL_SEL_SHIFT,
	ACLK_PERIHP_PLL_SEL_CPLL	= 0,
	ACLK_PERIHP_PLL_SEL_GPLL	= 1,
	ACLK_PERIHP_DIV_CON_SHIFT	= 0,
	ACLK_PERIHP_DIV_CON_MASK	= 0x1f,

	/* CLKSEL_CON21 */
	ACLK_EMMC_PLL_SEL_SHIFT         = 7,
	ACLK_EMMC_PLL_SEL_MASK          = 0x1 << ACLK_EMMC_PLL_SEL_SHIFT,
	ACLK_EMMC_PLL_SEL_GPLL          = 0x1,
	ACLK_EMMC_DIV_CON_SHIFT         = 0,
	ACLK_EMMC_DIV_CON_MASK          = 0x1f,

	/* CLKSEL_CON22 */
	CLK_EMMC_PLL_SHIFT              = 8,
	CLK_EMMC_PLL_MASK               = 0x7 << CLK_EMMC_PLL_SHIFT,
	CLK_EMMC_PLL_SEL_GPLL           = 0x1,
	CLK_EMMC_PLL_SEL_24M            = 0x5,
	CLK_EMMC_DIV_CON_SHIFT          = 0,
	CLK_EMMC_DIV_CON_MASK           = 0x7f << CLK_EMMC_DIV_CON_SHIFT,

	/* CLKSEL_CON23 */
	PCLK_PERILP0_DIV_CON_SHIFT	= 12,
	PCLK_PERILP0_DIV_CON_MASK	= 0x7 << PCLK_PERILP0_DIV_CON_SHIFT,
	HCLK_PERILP0_DIV_CON_SHIFT	= 8,
	HCLK_PERILP0_DIV_CON_MASK	= 3 << HCLK_PERILP0_DIV_CON_SHIFT,
	ACLK_PERILP0_PLL_SEL_SHIFT	= 7,
	ACLK_PERILP0_PLL_SEL_MASK	= 1 << ACLK_PERILP0_PLL_SEL_SHIFT,
	ACLK_PERILP0_PLL_SEL_CPLL	= 0,
	ACLK_PERILP0_PLL_SEL_GPLL	= 1,
	ACLK_PERILP0_DIV_CON_SHIFT	= 0,
	ACLK_PERILP0_DIV_CON_MASK	= 0x1f,

	/* CRU_CLK_SEL24_CON */
	CRYPTO0_PLL_SEL_SHIFT		= 6,
	CRYPTO0_PLL_SEL_MASK		= 3 << CRYPTO0_PLL_SEL_SHIFT,
	CRYPTO_PLL_SEL_CPLL		= 0,
	CRYPTO_PLL_SEL_GPLL,
	CRYPTO_PLL_SEL_PPLL		= 0,
	CRYPTO0_DIV_SHIFT		= 0,
	CRYPTO0_DIV_MASK		= 0x1f << CRYPTO0_DIV_SHIFT,

	/* CLKSEL_CON25 */
	PCLK_PERILP1_DIV_CON_SHIFT	= 8,
	PCLK_PERILP1_DIV_CON_MASK	= 0x7 << PCLK_PERILP1_DIV_CON_SHIFT,
	HCLK_PERILP1_PLL_SEL_SHIFT	= 7,
	HCLK_PERILP1_PLL_SEL_MASK	= 1 << HCLK_PERILP1_PLL_SEL_SHIFT,
	HCLK_PERILP1_PLL_SEL_CPLL	= 0,
	HCLK_PERILP1_PLL_SEL_GPLL	= 1,
	HCLK_PERILP1_DIV_CON_SHIFT	= 0,
	HCLK_PERILP1_DIV_CON_MASK	= 0x1f,

	/* CLKSEL_CON26 */
	CLK_SARADC_DIV_CON_SHIFT	= 8,
	CLK_SARADC_DIV_CON_MASK		= GENMASK(15, 8),
	CLK_SARADC_DIV_CON_WIDTH	= 8,
	CRYPTO1_PLL_SEL_SHIFT		= 6,
	CRYPTO1_PLL_SEL_MASK		= 3 << CRYPTO1_PLL_SEL_SHIFT,
	CRYPTO1_DIV_SHIFT		= 0,
	CRYPTO1_DIV_MASK		= 0x1f << CRYPTO1_DIV_SHIFT,

	/* CLKSEL_CON27 */
	CLK_TSADC_SEL_X24M		= 0x0,
	CLK_TSADC_SEL_SHIFT		= 15,
	CLK_TSADC_SEL_MASK		= 1 << CLK_TSADC_SEL_SHIFT,
	CLK_TSADC_DIV_CON_SHIFT		= 0,
	CLK_TSADC_DIV_CON_MASK		= 0x3ff,

	/* CLKSEL_CON47 & CLKSEL_CON48 */
	ACLK_VOP_PLL_SEL_SHIFT		= 6,
	ACLK_VOP_PLL_SEL_MASK		= 0x3 << ACLK_VOP_PLL_SEL_SHIFT,
	ACLK_VOP_PLL_SEL_CPLL		= 0x1,
	ACLK_VOP_PLL_SEL_GPLL		= 0x2,
	ACLK_VOP_DIV_CON_SHIFT		= 0,
	ACLK_VOP_DIV_CON_MASK		= 0x1f << ACLK_VOP_DIV_CON_SHIFT,

	/* CLKSEL_CON49 & CLKSEL_CON50 */
	DCLK_VOP_DCLK_SEL_SHIFT         = 11,
	DCLK_VOP_DCLK_SEL_MASK          = 1 << DCLK_VOP_DCLK_SEL_SHIFT,
	DCLK_VOP_DCLK_SEL_DIVOUT        = 0,
	DCLK_VOP_PLL_SEL_SHIFT          = 8,
	DCLK_VOP_PLL_SEL_MASK           = 3 << DCLK_VOP_PLL_SEL_SHIFT,
	DCLK_VOP_PLL_SEL_VPLL           = 0,
	DCLK_VOP_PLL_SEL_CPLL           = 1,
	DCLK_VOP_DIV_CON_MASK           = 0xff,
	DCLK_VOP_DIV_CON_SHIFT          = 0,

	/* CLKSEL_CON57 */
	PCLK_ALIVE_DIV_CON_SHIFT	= 0,
	PCLK_ALIVE_DIV_CON_MASK		= 0x1f << PCLK_ALIVE_DIV_CON_SHIFT,

	/* CLKSEL_CON58 */
	CLK_SPI_PLL_SEL_WIDTH = 1,
	CLK_SPI_PLL_SEL_MASK = ((1 < CLK_SPI_PLL_SEL_WIDTH) - 1),
	CLK_SPI_PLL_SEL_CPLL = 0,
	CLK_SPI_PLL_SEL_GPLL = 1,
	CLK_SPI_PLL_DIV_CON_WIDTH = 7,
	CLK_SPI_PLL_DIV_CON_MASK = ((1 << CLK_SPI_PLL_DIV_CON_WIDTH) - 1),

	CLK_SPI5_PLL_DIV_CON_SHIFT      = 8,
	CLK_SPI5_PLL_SEL_SHIFT	        = 15,

	/* CLKSEL_CON59 */
	CLK_SPI1_PLL_SEL_SHIFT		= 15,
	CLK_SPI1_PLL_DIV_CON_SHIFT	= 8,
	CLK_SPI0_PLL_SEL_SHIFT		= 7,
	CLK_SPI0_PLL_DIV_CON_SHIFT	= 0,

	/* CLKSEL_CON60 */
	CLK_SPI4_PLL_SEL_SHIFT		= 15,
	CLK_SPI4_PLL_DIV_CON_SHIFT	= 8,
	CLK_SPI2_PLL_SEL_SHIFT		= 7,
	CLK_SPI2_PLL_DIV_CON_SHIFT	= 0,

	/* CLKSEL_CON61 */
	CLK_I2C_PLL_SEL_MASK		= 1,
	CLK_I2C_PLL_SEL_CPLL		= 0,
	CLK_I2C_PLL_SEL_GPLL		= 1,
	CLK_I2C5_PLL_SEL_SHIFT		= 15,
	CLK_I2C5_DIV_CON_SHIFT		= 8,
	CLK_I2C1_PLL_SEL_SHIFT		= 7,
	CLK_I2C1_DIV_CON_SHIFT		= 0,

	/* CLKSEL_CON62 */
	CLK_I2C6_PLL_SEL_SHIFT		= 15,
	CLK_I2C6_DIV_CON_SHIFT		= 8,
	CLK_I2C2_PLL_SEL_SHIFT		= 7,
	CLK_I2C2_DIV_CON_SHIFT		= 0,

	/* CLKSEL_CON63 */
	CLK_I2C7_PLL_SEL_SHIFT		= 15,
	CLK_I2C7_DIV_CON_SHIFT		= 8,
	CLK_I2C3_PLL_SEL_SHIFT		= 7,
	CLK_I2C3_DIV_CON_SHIFT		= 0,

	/* CRU_SOFTRST_CON4 */
	RESETN_DDR0_REQ_SHIFT		= 8,
	RESETN_DDR0_REQ_MASK		= 1 << RESETN_DDR0_REQ_SHIFT,
	RESETN_DDRPHY0_REQ_SHIFT	= 9,
	RESETN_DDRPHY0_REQ_MASK		= 1 << RESETN_DDRPHY0_REQ_SHIFT,
	RESETN_DDR1_REQ_SHIFT		= 12,
	RESETN_DDR1_REQ_MASK		= 1 << RESETN_DDR1_REQ_SHIFT,
	RESETN_DDRPHY1_REQ_SHIFT	= 13,
	RESETN_DDRPHY1_REQ_MASK		= 1 << RESETN_DDRPHY1_REQ_SHIFT,
};

#define VCO_MAX_KHZ	(3200 * (MHz / KHz))
#define VCO_MIN_KHZ	(800 * (MHz / KHz))
#define OUTPUT_MAX_KHZ	(3200 * (MHz / KHz))
#define OUTPUT_MIN_KHZ	(16 * (MHz / KHz))

/*
 *  the div restructions of pll in integer mode, these are defined in
 *  * CRU_*PLL_CON0 or PMUCRU_*PLL_CON0
 */
#define PLL_DIV_MIN	16
#define PLL_DIV_MAX	3200

#define rk_clrsetreg(addr, clr, set)	\
				writel(((clr) | (set)) << 16 | (set), addr)
#define rk_clrreg(addr, clr)		writel((clr) << 16, addr)
#define rk_setreg(addr, set)		writel((set) << 16 | (set), addr)

/*X:\u-boot\drivers\clk\rockchip\clk_rk3399.c end*/

u32 *regsbak;
void *regs;//vopl
/*
printf("Detailed mode clock %u kHz, flags[%x]\n"
	   "	H: %04d %04d %04d %04d\n"
	   "	V: %04d %04d %04d %04d\n"
	   "bus_format: %x\n",
	   mode->clock, mode->flags,
	   mode->hdisplay, mode->hsync_start,
	   mode->hsync_end, mode->htotal,
	   mode->vdisplay, mode->vsync_start,
	   mode->vsync_end, mode->vtotal,
	   conn_state->bus_format);
[2020/9/4 9:23:42] Detailed mode clock 120000 kHz, flags[a]  //#define DRM_MODE_FLAG_BCAST (1 << 10)
[2020/9/4 9:23:42]     H: 1088 1192 1196 1323
[2020/9/4 9:23:42]     V: 1920 1924 1926 1929
*/
static inline void vop_mask_write(void *vop, uint32_t offset,
				  uint32_t mask, uint32_t shift, uint32_t v,
				  bool write_mask)
{
	if (!mask)
		return;

	if (write_mask) {
		v = ((v & mask) << shift) | (mask << (shift + 16));
	} else {
		uint32_t cached_val = regsbak[offset >> 2];
		v = (cached_val & ~(mask << shift)) | ((v & mask) << shift);
		regsbak[offset >> 2] = v;
	}
	writel(v, regs + offset);
}
				  
//X:\u-boot\drivers\clk\rockchip\clk_rk3399.c
#define RK3399_LIMIT_PLL_ACLK_VOP	(400 * 1000000)
//X:\u-boot\arch\arm\include\asm\arch-rockchip\cru_rk3399.h
#define MHz		1000000
#define KHz		1000
#define OSC_HZ		(24*MHz)
#define APLL_HZ		(600*MHz)
#define GPLL_HZ		(800 * MHz)
#define CPLL_HZ		(384*MHz)
#define NPLL_HZ		(600 * MHz)
#define PPLL_HZ		(676*MHz)

#define PMU_PCLK_HZ	(48*MHz)

#define ACLKM_CORE_HZ	(300*MHz)
#define ATCLK_CORE_HZ	(300*MHz)
#define PCLK_DBG_HZ	(100*MHz)

#define PERIHP_ACLK_HZ	(150 * MHz)
#define PERIHP_HCLK_HZ	(75 * MHz)
#define PERIHP_PCLK_HZ	(37500 * KHz)

#define PERILP0_ACLK_HZ	(300 * MHz)
#define PERILP0_HCLK_HZ	(100 * MHz)
#define PERILP0_PCLK_HZ	(50 * MHz)

#define PERILP1_HCLK_HZ	(100 * MHz)
#define PERILP1_PCLK_HZ	(50 * MHz)

#define PWM_CLOCK_HZ    PMU_PCLK_HZ


void zjj_rockchip_vop_init(void * reg, u32 * rbak, int len)
{
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);

	regsbak = rbak;
    regs = reg;

	vop_mask_write(regs, global_regdone_en.offset, global_regdone_en.mask, global_regdone_en.shift, 
		1, global_regdone_en.write_mask);
	vop_mask_write(regs, axi_outstanding_max_num.offset, axi_outstanding_max_num.mask, axi_outstanding_max_num.shift, 
		0x1e, axi_outstanding_max_num.write_mask);
	vop_mask_write(regs, axi_max_outstanding_en.offset, axi_max_outstanding_en.mask, axi_max_outstanding_en.shift, 
		1, axi_max_outstanding_en.write_mask);
	vop_mask_write(regs, win_gate0.offset, win_gate0.mask, win_gate0.shift, 
		1, win_gate0.write_mask);
	vop_mask_write(regs, win_gate1.offset, win_gate1.mask, win_gate1.shift, 
		1, win_gate0.write_mask);
	vop_mask_write(regs, dsp_blank.offset, dsp_blank.mask, dsp_blank.shift, 
		0, dsp_blank.write_mask);
	vop_mask_write(regs, mipi_en.offset, mipi_en.mask, mipi_en.shift, 
		1, mipi_en.write_mask);
	vop_mask_write(regs, mipi_pin_pol.offset, mipi_pin_pol.mask, mipi_pin_pol.shift, 
		8, mipi_pin_pol.write_mask);
	vop_mask_write(regs, mipi_dclk_pol.offset, mipi_dclk_pol.mask, mipi_dclk_pol.shift, 
		1, mipi_dclk_pol.write_mask);
	vop_mask_write(regs, mipi_dual_channel_en.offset, mipi_dual_channel_en.mask, mipi_dual_channel_en.shift, 
		0, mipi_dual_channel_en.write_mask);

	vop_mask_write(regs, data01_swap.offset, data01_swap.mask, data01_swap.shift, 
		0, data01_swap.write_mask);

	vop_mask_write(regs, dither_down.offset, dither_down.mask, dither_down.shift, 
		1, dither_down.write_mask);

	vop_mask_write(regs, dclk_ddr.offset, dclk_ddr.mask, dclk_ddr.shift, 
		0, dclk_ddr.write_mask);
	vop_mask_write(regs, dsp_data_swap.offset, dsp_data_swap.mask, dsp_data_swap.shift, 
		0, dsp_data_swap.write_mask);
	vop_mask_write(regs, dsp_data_swap.offset, dsp_data_swap.mask, dsp_data_swap.shift, 
		0, dsp_data_swap.write_mask);

	vop_mask_write(regs, out_mode.offset, out_mode.mask, out_mode.shift, 
		0, out_mode.write_mask);
	vop_mask_write(regs, overlay_mode.offset, overlay_mode.mask, overlay_mode.shift, 
		0, overlay_mode.write_mask);

	vop_mask_write(regs, dsp_out_yuv.offset, dsp_out_yuv.mask, dsp_out_yuv.shift, 
		0, dsp_out_yuv.write_mask);

	vop_mask_write(regs, bcsh_r2y_en.offset, bcsh_r2y_en.mask, bcsh_r2y_en.shift, 
		0, bcsh_r2y_en.write_mask);
	vop_mask_write(regs, bcsh_y2r_en.offset, bcsh_y2r_en.mask, bcsh_y2r_en.shift, 
		0, bcsh_y2r_en.write_mask);
	vop_mask_write(regs, bcsh_r2y_csc_mode.offset, bcsh_r2y_csc_mode.mask, bcsh_r2y_csc_mode.shift, 
		1, bcsh_r2y_csc_mode.write_mask);
	vop_mask_write(regs, bcsh_y2r_csc_mode.offset, bcsh_y2r_csc_mode.mask, bcsh_y2r_csc_mode.shift, 
		1, bcsh_y2r_csc_mode.write_mask);
	vop_mask_write(regs, htotal_pw.offset, htotal_pw.mask, htotal_pw.shift, 
		0x52b0004, htotal_pw.write_mask);
	vop_mask_write(regs, hact_st_end.offset, hact_st_end.mask, hact_st_end.shift, 
		0x8304c3, hact_st_end.write_mask);
	vop_mask_write(regs, vact_st_end.offset, vact_st_end.mask, vact_st_end.shift, 
		0x50785, vact_st_end.write_mask);
	vop_mask_write(regs, dsp_interlace.offset, dsp_interlace.mask, dsp_interlace.shift, 
		0, dsp_interlace.write_mask);
	vop_mask_write(regs, p2i_en.offset, p2i_en.mask, p2i_en.shift, 
		0, p2i_en.write_mask);
	vop_mask_write(regs, vtotal_pw.offset, vtotal_pw.mask, vtotal_pw.shift, 
		 0x7890002, vtotal_pw.write_mask);

	vop_mask_write(regs, hpost_st_end.offset, hpost_st_end.mask, hpost_st_end.shift, 
		 0x8304c3, hpost_st_end.write_mask);

	vop_mask_write(regs, vpost_st_end.offset, vpost_st_end.mask, vpost_st_end.shift, 
		 0x50785, vpost_st_end.write_mask);

	vop_mask_write(regs, post_scl_factor.offset, post_scl_factor.mask, post_scl_factor.shift, 
		 0x10001000 , post_scl_factor.write_mask);
	vop_mask_write(regs, post_scl_ctrl.offset, post_scl_ctrl.mask, post_scl_ctrl.shift, 
		 0 , post_scl_ctrl.write_mask);
	
	vop_mask_write(regs, core_dclk_div.offset, core_dclk_div.mask, core_dclk_div.shift, 
		 0 , core_dclk_div.write_mask);
	
	vop_mask_write(regs, line_flag_num0.offset, line_flag_num0.mask, line_flag_num0.shift, 
		 0x782 , line_flag_num0.write_mask);
	vop_mask_write(regs, line_flag_num1.offset, line_flag_num1.mask, line_flag_num1.shift, 
		 0x72b , line_flag_num1.write_mask);
	vop_mask_write(regs, cfg_done.offset, cfg_done.mask, cfg_done.shift, 
		 1 , cfg_done.write_mask);

}

void zjj_rockchip_vop_set_plane()
{
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);
	vop_mask_write(regs, ymirror.offset, ymirror.mask, ymirror.shift, 
		 0 , ymirror.write_mask);
	vop_mask_write(regs, xmirror.offset, xmirror.mask, xmirror.shift, 
		 0 , xmirror.write_mask);
	vop_mask_write(regs, format.offset, format.mask, format.shift, 
		 2 , format.write_mask);
	vop_mask_write(regs, yrgb_vir.offset, yrgb_vir.mask, yrgb_vir.shift, 
		 0x12c , yrgb_vir.write_mask);
	vop_mask_write(regs, yrgb_mst.offset, yrgb_mst.mask, yrgb_mst.shift, 
		 0x7df06000 , yrgb_mst.write_mask);
	vop_mask_write(regs, act_info.offset, act_info.mask, act_info.shift, 
		 ((1920-1) << 16) |  ((1080-1) & 0xffff), act_info.write_mask);
	vop_mask_write(regs, dsp_info.offset, dsp_info.mask, dsp_info.shift, 
		 ((1920-1) << 16) |  ((1080-1) & 0xffff), dsp_info.write_mask);
	vop_mask_write(regs, dsp_st.offset, dsp_st.mask, dsp_st.shift, 
		 ((1929 - 1924) << 16) | ((1323 - 1192) & 0xffff)  , dsp_st.write_mask);
	vop_mask_write(regs, rb_swap.offset, rb_swap.mask, rb_swap.shift, 
		 1 , rb_swap.write_mask);
	vop_mask_write(regs, src_alpha_ctl.offset, src_alpha_ctl.mask, src_alpha_ctl.shift, 
		 0 , src_alpha_ctl.write_mask);
	vop_mask_write(regs, enable.offset, enable.mask, enable.shift, 
		 1 , enable.write_mask);

}

void zjj_rockchip_vop_enable()
{
    printf("zjj.rk3399.uboot %s %s %d \n",__FILE__,__FUNCTION__,__LINE__);
	vop_mask_write(regs, standby.offset, standby.mask, standby.shift, 
		 0 , standby.write_mask);
	vop_mask_write(regs, cfg_done.offset, cfg_done.mask, cfg_done.shift, 
		 1 , cfg_done.write_mask);

}


