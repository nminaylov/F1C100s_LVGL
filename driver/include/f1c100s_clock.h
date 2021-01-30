#ifndef __F1C100S_CLOCK_H__
#define __F1C100S_CLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define F1C100S_CCU_BASE        (0x01c20000)

#define CCU_PLL_CPU_CTRL        (0x000)
#define CCU_PLL_AUDIO_CTRL      (0x008)
#define CCU_PLL_VIDEO_CTRL      (0x010)
#define CCU_PLL_VE_CTRL         (0x018)
#define CCU_PLL_DDR_CTRL        (0x020)
#define CCU_PLL_PERIPH_CTRL     (0x028)
#define CCU_CPU_CFG             (0x050)
#define CCU_AHB_APB_CFG         (0x054)

#define CCU_BUS_CLK_GATE0       (0x060)
#define CCU_BUS_CLK_GATE1       (0x064)
#define CCU_BUS_CLK_GATE2       (0x068)

#define CCU_SDMMC0_CLK          (0x088)
#define CCU_SDMMC1_CLK          (0x08c)
#define CCU_DAUDIO_CLK          (0x0b0)
#define CCU_SPDIF_CLK           (0x0b4)
#define CCU_I2S_CLK             (0x0b8)
#define CCU_USBPHY_CFG          (0x0cc)
#define CCU_DRAM_CLK_GATE       (0x100)
#define CCU_DEBE_CLK            (0x104)
#define CCU_DEFE_CLK            (0x10c)
#define CCU_TCON_CLK            (0x118)
#define CCU_DEINTERLACE_CLK     (0x11c)
#define CCU_TVE_CLK             (0x120)
#define CCU_TVD_CLK             (0x124)
#define CCU_CSI_CLK             (0x134)
#define CCU_VE_CLK              (0x13c)
#define CCU_ADDA_CLK            (0x140)
#define CCU_AVS_CLK             (0x144)

#define CCU_PLL_STABLE_TIME0    (0x200)
#define CCU_PLL_STABLE_TIME1    (0x204)
#define CCU_PLL_CPU_BIAS        (0x220)
#define CCU_PLL_AUDIO_BIAS      (0x224)
#define CCU_PLL_VIDEO_BIAS      (0x228)
#define CCU_PLL_VE_BIAS         (0x22c)
#define CCU_PLL_DDR0_BIAS       (0x230)
#define CCU_PLL_PERIPH_BIAS     (0x234)
#define CCU_PLL_CPU_TUN         (0x250)
#define CCU_PLL_DDR_TUN         (0x260)
#define CCU_PLL_AUDIO_PAT       (0x284)
#define CCU_PLL_VIDEO_PAT       (0x288)
#define CCU_PLL_DDR0_PAT        (0x290)

#define CCU_BUS_SOFT_RST0       (0x2c0)
#define CCU_BUS_SOFT_RST1       (0x2c4)
#define CCU_BUS_SOFT_RST3       (0x2d0)

#define F1C100S_RESET_DMA           (6)
#define F1C100S_RESET_SD0           (8)
#define F1C100S_RESET_SD1           (9)
#define F1C100S_RESET_SDRAM         (14)
#define F1C100S_RESET_SPI0          (20)
#define F1C100S_RESET_SPI1          (21)
#define F1C100S_RESET_USB_OTG       (24)
#define F1C100S_RESET_VE            (32)
#define F1C100S_RESET_LCD           (36)
#define F1C100S_RESET_DEINTERLACE   (37)
#define F1C100S_RESET_CSI           (40)
#define F1C100S_RESET_TVD           (41)
#define F1C100S_RESET_TVE           (42)
#define F1C100S_RESET_DEBE          (44)
#define F1C100S_RESET_DEFE          (46)
#define F1C100S_RESET_ADDA          (64)
#define F1C100S_RESET_SPDIF         (65)
#define F1C100S_RESET_CIR           (66)
#define F1C100S_RESET_RSB           (67)
#define F1C100S_RESET_DAUDIO        (76)
#define F1C100S_RESET_I2C0          (80)
#define F1C100S_RESET_I2C1          (81)
#define F1C100S_RESET_I2C2          (82)
#define F1C100S_RESET_UART0         (84)
#define F1C100S_RESET_UART1         (85)
#define F1C100S_RESET_UART2         (86)

typedef enum
{
    PLL_CPU = CCU_PLL_CPU_CTRL,
    PLL_AUDIO = CCU_PLL_AUDIO_CTRL,
    PLL_VIDEO = CCU_PLL_VIDEO_CTRL,
    PLL_VE = CCU_PLL_VE_CTRL,
    PLL_DDR = CCU_PLL_DDR_CTRL,
    PLL_PERIPH = CCU_PLL_PERIPH_CTRL,
} pll_ch_e;

typedef enum
{
    CLK_DE_SRC_PLL_VIDEO = 0, CLK_DE_SRC_PLL_PERIPH = 2,
} clk_source_de_e;

typedef enum
{
    CLK_VID_SRC_PLL_VIDEO_1X = 0, CLK_VID_SRC_OSC24 = 1, // TVD only
    CLK_VID_SRC_PLL_VIDEO_2X = 2,
} clk_source_vid_e;

void clk_pll_enable(pll_ch_e pll);
void clk_pll_disable(pll_ch_e pll);

void clk_pll_enable(pll_ch_e pll);
void clk_pll_disable(pll_ch_e pll);
uint8_t clk_pll_get_state(pll_ch_e pll);
void clk_pll_init(pll_ch_e pll, uint8_t mul, uint8_t div);

void clk_enable(uint32_t reg, uint8_t bit);
void clk_disable(uint32_t reg, uint8_t bit);

void clk_de_config(uint32_t reg, clk_source_de_e source, uint8_t div);
void clk_tcon_config(clk_source_vid_e source);

void clk_reset_set(uint32_t reg, uint8_t bit);
void clk_reset_clear(uint32_t reg, uint8_t bit);

#ifdef __cplusplus
}
#endif

#endif /* __F1C100S_CLOCK_H__ */

