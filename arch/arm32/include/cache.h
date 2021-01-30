#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdint.h>

static inline void cache_inv_range(unsigned long start, unsigned long end)
{
#if __ARM32_ARCH__ == 4
	extern void v4_cache_inv_range(unsigned long start, unsigned long end);
	v4_cache_inv_range(start, end);
#endif
#if __ARM32_ARCH__ == 5
	extern void v5_cache_inv_range(unsigned long start, unsigned long end);
	v5_cache_inv_range(start, end);
#endif
#if __ARM32_ARCH__ == 6
	extern void v6_cache_inv_range(unsigned long start, unsigned long end);
	v6_cache_inv_range(start, end);
#endif
#if __ARM32_ARCH__ == 7
	extern void v7_cache_inv_range(unsigned long start, unsigned long end);
	v7_cache_inv_range(start, end);
#endif
}

static inline void cache_clean_range(unsigned long start, unsigned long end)
{
#if __ARM32_ARCH__ == 4
	extern void v4_cache_clean_range(unsigned long start, unsigned long end);
	v4_cache_clean_range(start, end);
#endif
#if __ARM32_ARCH__ == 5
	extern void v5_cache_clean_range(unsigned long start, unsigned long end);
	v5_cache_clean_range(start, end);
#endif
#if __ARM32_ARCH__ == 6
	extern void v6_cache_clean_range(unsigned long start, unsigned long end);
	v6_cache_clean_range(start, end);
#endif
#if __ARM32_ARCH__ == 7
	extern void v7_cache_clean_range(unsigned long start, unsigned long end);
	v7_cache_clean_range(start, end);
#endif
}

static inline void cache_flush_range(unsigned long start, unsigned long end)
{
#if __ARM32_ARCH__ == 4
	extern void v4_cache_flush_range(unsigned long start, unsigned long end);
	v4_cache_flush_range(start, end);
#endif
#if __ARM32_ARCH__ == 5
	extern void v5_cache_flush_range(unsigned long start, unsigned long end);
	v5_cache_flush_range(start, end);
#endif
#if __ARM32_ARCH__ == 6
	extern void v6_cache_flush_range(unsigned long start, unsigned long end);
	v6_cache_flush_range(start, end);
#endif
#if __ARM32_ARCH__ == 7
	extern void v7_cache_flush_range(unsigned long start, unsigned long end);
	v7_cache_flush_range(start, end);
#endif
}

#endif /* __CACHE_H__ */
