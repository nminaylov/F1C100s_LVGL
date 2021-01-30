#include "touch.h"
#include "printf.h"
#include "gpio-f1c100s.h"
#include "lcd.h"
#include "lvgl.h"
#include "main.h"
#include <stdio.h>

uint16_t t_x = 0;
uint16_t t_y = 0;
uint8_t t_pressed = 0;
uint16_t t_cnt = 0;

volatile int32_t 	cali_A = -4590, cali_B = 2783, cali_C = 8517532,
					cali_D = 453, cali_E = -6556, cali_F = 5014778;

static bool touch_read_lvgl(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void touch_init_lvgl(void)
{
    lv_indev_drv_t indev_drv;

    touch_init();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read_lvgl;
    indev_touchpad = lv_indev_drv_register(&indev_drv);
}

/* Will be called by the library to read the touchpad */
static bool touch_read_lvgl(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    /*Save the pressed coordinates and the state*/
    if (t_pressed != 0)
    {
        last_x = (cali_A * t_x + cali_B * t_y + cali_C) / RESCALE_FACTOR;
        last_y = (cali_D * t_x + cali_E * t_y + cali_F) / RESCALE_FACTOR;
        data->state = LV_INDEV_STATE_PR;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Set the last pressed coordinates*/
    data->point.x = last_x;
    data->point.y = last_y;

    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}


void touch_init(void)
{
	gpio_f1c100s_set_cfg(&GPIO_PA, 0, 0x02);
	gpio_f1c100s_set_cfg(&GPIO_PA, 1, 0x02);
	gpio_f1c100s_set_cfg(&GPIO_PA, 2, 0x02);
	gpio_f1c100s_set_cfg(&GPIO_PA, 3, 0x02);

	TOUCH_REG0 |= (7 << 16);// FS_DIV
	TOUCH_REG0 |= (63 << 0);// TACQ
	TOUCH_REG1 = 0;
	TOUCH_REG1 |= (5 << 12); // debounce
	TOUCH_REG1 |= (1 << 9); // debounce_en
	TOUCH_REG1 |= (1 << 5); // TP_EN
	//TOUCH_REG2 = //оставляем как есть
	TOUCH_REG3 |= (1 << 2); // filter_en
	TOUCH_FIFO_STAT = 0;
	TOUCH_FIFO_CTRL = 0;
	TOUCH_FIFO_CTRL |= (1 << 0) | (1 << 1) | (1 << 16); // down_irq | up_irq | data_irq
	TOUCH_FIFO_CTRL |= (1 << 4) | (2 << 8); // fifo flush | fifo = 2

	*((uint32_t*)(0x01C20400+0x20)) |= (1 << 20);	// IRQ
	*((uint32_t*)(0x01C20400+0x30)) &= ~(1 << 20);
}

void Touch_IRQ_Handler(void)
{
	//static uint8_t dn_cnt = 0;
	uint32_t temp_stat = TOUCH_FIFO_STAT;
	if (temp_stat & (1 << 16))
	{
		if (t_cnt > 5)
			t_pressed = 1;
		else
			t_cnt++;
		t_x = TOUCH_DATA;
		t_y = TOUCH_DATA;

	}
	if (temp_stat & (1 << 1)) // UP
	{
		t_pressed = 0;
		t_cnt = 0;
	}
	else if (temp_stat & (1 << 0)) // DN
	{
		//t_pressed = 1;
		//if (t_cnt == 0)
			//t_cnt = 1;
	}
	TOUCH_FIFO_STAT = temp_stat;
}

uint8_t touch_get_raw(uint16_t * x, uint16_t * y)
{
	if (t_pressed == 0)
	{
		return 0;
	}
	else
	{
		//t_cnt = 0;
		*x = t_x;
		*y = t_y;
		return 1;
	}
}

uint8_t Touch_get(uint16_t * X, uint16_t * Y)
{
	uint16_t x_temp = 0, y_temp = 0;
	if(!touch_get_raw(&x_temp,&y_temp))
		return 0;
	*X = (cali_A * x_temp + cali_B * y_temp + cali_C) / RESCALE_FACTOR;
	*Y = (cali_D * x_temp + cali_E * y_temp + cali_F) / RESCALE_FACTOR;
	if (*X >= TOUCH_X_MAX || *Y >= TOUCH_Y_MAX)
		return 0;
	return 1;
}

void Touch_calibrate(void)
{
	uint16_t t1,t2;
	uint32_t xd1 = (50 * TOUCH_X_MAX) / 100, xd2 = (90 * TOUCH_X_MAX) / 100, xd3 = (10 * TOUCH_X_MAX) / 100;
	uint32_t yd1 = (10 * TOUCH_Y_MAX) / 100, yd2 = (50 * TOUCH_Y_MAX) / 100, yd3 = (90 * TOUCH_Y_MAX) / 100;
	uint16_t xt1, xt2, xt3;
	uint16_t yt1, yt2, yt3;
	float temp1, temp2;
	float cal_A = 0.0, cal_B = 0.0, cal_C = 0.0, cal_D = 0.0, cal_E = 0.0, cal_F = 0.0;

	while (touch_get_raw(&t1,&t2));

	//POINT 1
	LCD_Fill(xd1-4, yd1-4, 8, 8, RED);
	while (!touch_get_raw(&xt1,&yt1));
	delay(100);
	while (touch_get_raw(&t1,&t2));

	//POINT 2
	LCD_Fill(xd2-4, yd2-4, 8, 8, RED);
	while (!touch_get_raw(&xt2,&yt2));
	delay(100);
	while (touch_get_raw(&t1,&t2));

	//POINT 3
	LCD_Fill(xd3-4, yd3-4, 8, 8, RED);
	while (!touch_get_raw(&xt3,&yt3));
	delay(100);
	while (touch_get_raw(&t1,&t2));

	printf("%d, %d, %d\r\n",xt1,xt2,xt3);
	printf("%d, %d, %d\r\n",yt1,yt2,yt3);

	//A
	temp1 = ((float) xd1 * ((float) yt2 - (float) yt3)) + ((float) xd2 * ((float) yt3 - (float) yt1)) + ((float) xd3 * ((float) yt1 - (float) yt2));
	temp2 = ((float) xt1 * ((float) yt2 - (float) yt3)) + ((float) xt2 * ((float) yt3 - (float) yt1)) + ((float) xt3 * ((float) yt1 - (float) yt2));
	cal_A = temp1 / temp2;
	cali_A = (int32_t) ((float)cal_A * RESCALE_FACTOR);
	printf("A:%lu\r\n",cali_A);

	//B
	temp1 = (cal_A * ((float) xt3 - (float) xt2)) + (float) xd2 - (float) xd3;
	temp2 = (float) yt2 - (float) yt3;
	cal_B = temp1 / temp2;
	cali_B = (int32_t) ((float)cal_B * RESCALE_FACTOR);
	printf("B:%lu\r\n",cali_B);

	//C
	cal_C = (float) xd3 - (cal_A * (float) xt3) - (cal_B * (float) yt3);
	cali_C = (int32_t) (cal_C * RESCALE_FACTOR);
	printf("C:%lu\r\n",cali_C);

	//D
	temp1 = ((float) yd1 * ((float) yt2 - (float) yt3)) + ((float) yd2 * ((float) yt3 - (float) yt1)) + ((float) yd3 * ((float) yt1 - (float) yt2));
	temp2 = ((float) xt1 * ((float) yt2 - (float) yt3)) + ((float) xt2 * ((float) yt3 - (float) yt1)) + ((float) xt3 * ((float) yt1 - (float) yt2));
	cal_D = (float)temp1 / (float)temp2;
	cali_D = (int32_t) (cal_D * RESCALE_FACTOR);
	printf("D:%lu\r\n",cali_D);

	//E
	temp1 = (cal_D * ((float) xt3 - (float) xt2)) + (float) yd2 - (float) yd3;
	temp2 = (float) yt2 - (float) yt3;
	cal_E = (float)temp1 / (float)temp2;
	cali_E = (int32_t) (cal_E * RESCALE_FACTOR);
	printf("E:%lu\r\n",cali_E);

	//F
	cal_F = (float) yd3 - cal_D * (float) xt3 - cal_E * (float) yt3;
	cali_F = (int32_t) (cal_F * RESCALE_FACTOR);
	printf("F:%lu\r\n",cali_F);
}

