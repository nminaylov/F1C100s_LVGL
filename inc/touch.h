#ifndef DRIVER_INCLUDE_TOUCH_H_
#define DRIVER_INCLUDE_TOUCH_H_

#include <stdint.h>

#define TOUCH_BASE 0x01C24800

#define TOUCH_REG0      (*(uint32_t *)(TOUCH_BASE + 0x00))
#define TOUCH_REG1      (*(uint32_t *)(TOUCH_BASE + 0x04))
#define TOUCH_REG2      (*(uint32_t *)(TOUCH_BASE + 0x08))
#define TOUCH_REG3      (*(uint32_t *)(TOUCH_BASE + 0x0C))
#define TOUCH_FIFO_CTRL (*(uint32_t *)(TOUCH_BASE + 0x10))
#define TOUCH_FIFO_STAT (*(uint32_t *)(TOUCH_BASE + 0x14))
#define TOUCH_COM_DATA  (*(uint32_t *)(TOUCH_BASE + 0x1C))
#define TOUCH_DATA      (*(uint32_t *)(TOUCH_BASE + 0x24))

#define TOUCH_X_MAX 800
#define TOUCH_Y_MAX 480
#define TOUCH_X_MIN 0
#define TOUCH_Y_MIN 0

#define RESCALE_FACTOR 10000

void touch_init(void);
void touch_init_lvgl(void);
uint8_t touch_get_raw(uint16_t * x, uint16_t * y);
void Touch_IRQ_Handler(void);
void Touch_calibrate(void);
uint8_t Touch_get(uint16_t * X, uint16_t * Y);

#endif /* DRIVER_INCLUDE_TOUCH_H_ */
