#ifndef __IO_H__
#define __IO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//static inline u8_t read8(uint32_t addr)
//{
//	return( *((volatile u8_t *)(addr)) );
//}
//
//static inline u16_t read16(uint32_t addr)
//{
//	return( *((volatile u16_t *)(addr)) );
//}
//
////static inline u32_t read32(uint32_t addr)
////{
////	return( *((volatile u32_t *)(addr)) );
////}
//
//static inline u64_t read64(uint32_t addr)
//{
//	return( *((volatile u64_t *)(addr)) );
//}
//
//static inline void write8(uint32_t addr, u8_t value)
//{
//	*((volatile u8_t *)(addr)) = value;
//}
//
//static inline void write16(uint32_t addr, u16_t value)
//{
//	*((volatile u16_t *)(addr)) = value;
//}

#define read8(x) (*(( uint8_t *)(x)))
#define write8(x,y) (*(( uint8_t *)(x)) = y)

#define read16(x) (*(( uint16_t *)(x)))
#define write16(x,y) (*(( uint16_t *)(x)) = y)

#define read32(x) (*(( uint32_t *)(x)))
#define write32(x,y) (*(( uint32_t *)(x)) = y)

#define read64(x) (*(( uint64_t *)(x)))
#define write64(x,y) (*(( uint64_t *)(x)) = y)

void sys_mmu_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __IO_H__ */
