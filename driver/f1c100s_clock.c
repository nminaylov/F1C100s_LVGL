/***************************************************************************
 * @file f1c100s_clock.c
 * @brief F1C100s Clock contoroller driver (CCU)
 * @author n.minaylov
 **************************************************************************/

#include "f1c100s_clock.h"
#include "io.h"

static void pll_cpu_init(uint8_t mul, uint8_t div);
static void pll_audio_init(uint16_t mul, uint8_t div);
static void pll_video_init(pll_ch_e pll, uint8_t mul, uint8_t div);


/************** PLLs ***************/
// Enable PLL
inline void clk_pll_enable(pll_ch_e pll)
{
	write32(F1C100S_CCU_BASE+pll, (read32(F1C100S_CCU_BASE+pll)|(1 << 31)));
}

// Disable PLL
inline void clk_pll_disable(pll_ch_e pll)
{
	write32(F1C100S_CCU_BASE+pll, (read32(F1C100S_CCU_BASE+pll)&~(1 << 31)));
}

// Get PLL lock state
uint8_t clk_pll_get_state(pll_ch_e pll)
{
	uint32_t val = read32(F1C100S_CCU_BASE + pll);
	return ((val >> 28) & 0x1);
}

// Configure PLL channel
// out = 24MHz * mul / div
void clk_pll_init(pll_ch_e pll, uint8_t mul, uint8_t div)
{
	switch (pll)
	{
	case PLL_CPU:
		pll_cpu_init(mul, div);
		break;
	case PLL_AUDIO:
		pll_audio_init(mul, div);
		break;
	case PLL_VIDEO:
	case PLL_VE:
		pll_video_init(pll, mul, div);
		break;
	case PLL_DDR:
	case PLL_PERIPH: // todo
	default: break;
	}
}

static void pll_cpu_init(uint8_t mul, uint8_t div) // out = (24MHz*N*K) / (M*P)
{
	if ((mul == 0) || (div == 0))
		return;
	if ((mul > 128) || (div > 16))
		return;

	uint8_t n, k, m, p;
	for (k = 1; k <= 4; k++) // mul = n*k
	{
		n = mul / k;
		if ((n < 32) && (n*k == mul))
			break;
	}
	if (n*k != mul)
		return;

	for (m = 1; m <= 4; m++) // div = m*p
	{
		p = div / m;
		if (((p == 1) || (p == 2) || (p == 4)) && (m*p == div))
			break;
	}
	if (m*p != div)
		return;

	p--;
	if (p == 3)
		p = 2;


	uint32_t val = read32(F1C100S_CCU_BASE + CCU_PLL_CPU_CTRL);
	val &= (1 << 31) | (1 << 28);
	val |= ((n-1) << 8) | ((k-1) << 4) | (m-1) | (p << 16);
	write32(F1C100S_CCU_BASE + CCU_PLL_CPU_CTRL, val);
}

static void pll_audio_init(uint16_t mul, uint8_t div) // out = (24MHz*N*2) / M
{
	if ((mul == 0) || (div == 0))
		return;
	if ((mul > 256) || (div > 32))
		return;

	uint8_t n = (uint8_t)(mul / 2); // mul = n*2
	// div = m

	uint32_t val = read32(F1C100S_CCU_BASE + CCU_PLL_AUDIO_CTRL);
	val &= (1 << 31) | (1 << 28);
	val |= ((n-1) << 8) | (div-1);
	write32(F1C100S_CCU_BASE + CCU_PLL_AUDIO_CTRL, val);
}

static void pll_video_init(pll_ch_e pll, uint8_t mul, uint8_t div) // out = (24MHz*N) / M
{
	if ((mul == 0) || (div == 0))
		return;
	if ((mul > 128) || (div > 16))
		return;

	// mul = n
	// div = m

	uint32_t val = read32(F1C100S_CCU_BASE + pll);
	val &= (1 << 31) | (1 << 28);
	val |= ((mul-1) << 8) | (div-1) | (1 << 24);
	write32(F1C100S_CCU_BASE + pll, val);
}

/************** Clock gating ***************/

inline void clk_enable(uint32_t reg, uint8_t bit)
{
	write32(F1C100S_CCU_BASE+reg, (read32(F1C100S_CCU_BASE+reg) | (1 << bit)));
}

inline void clk_disable(uint32_t reg, uint8_t bit)
{
	write32(F1C100S_CCU_BASE+reg, (read32(F1C100S_CCU_BASE+reg) & ~(1 << bit)));
}

/************** Specific clocks configuration ***************/

void clk_de_config(uint32_t reg, clk_source_de_e source, uint8_t div)
{
	if ((div == 0) || (div > 16))
		return;

	uint32_t val = read32(F1C100S_CCU_BASE+reg);

	val &= ~((0x7 << 24) | (0xF));
	val |= (source << 24) | (div - 1);

	write32(F1C100S_CCU_BASE+reg, val);
}

void clk_tcon_config(clk_source_vid_e source)
{
	uint32_t reg = read32(F1C100S_CCU_BASE+CCU_TCON_CLK) & ~(0x7 << 24);
	write32(F1C100S_CCU_BASE+CCU_TCON_CLK, reg | (source << 24));
}


/************** Resets ***************/

inline void clk_reset_set(uint32_t reg, uint8_t bit)
{
	write32(F1C100S_CCU_BASE+reg, (read32(F1C100S_CCU_BASE+reg) & ~(1 << bit)));
}

inline void clk_reset_clear(uint32_t reg, uint8_t bit)
{
	write32(F1C100S_CCU_BASE+reg, (read32(F1C100S_CCU_BASE+reg) | (1 << bit)));
}

