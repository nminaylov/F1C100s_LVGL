#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "gpio-f1c100s.h"
#include "pwm-f1c100s.h"
#include "display.h"
#include "f1c100s_de.h"
#include "lvgl/lvgl.h"
#include "printf.h"
#include "cache.h"

pwm_t led_pwm_bl =
{
    .virt = 0x01c21000,
    .duty = 0,//100,
    .period = 100000,
    .channel = 1,
    .polarity = true,
    .pwm_port = &GPIO_PE,
    .pwm_pin = 6,
    .pwm_pin_cfg = 0x03,
};

#define LCD_H_RES LV_HOR_RES_MAX
#define LCD_V_RES LV_VER_RES_MAX

static void display_gpio_init(void);

static void display_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
#if LV_USE_GPU
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width, const lv_area_t * fill_area, lv_color_t color);
#endif

static lv_disp_buf_t draw_buf_dsc;
static lv_color_t draw_buf_1[LCD_H_RES * LCD_V_RES];// __attribute__ ((section (".dma")));            /*A screen sized buffer*/
static lv_color_t draw_buf_2[LCD_H_RES * LCD_V_RES];// __attribute__ ((section (".dma")));            /*An other screen sized buffer*/

void display_init_lvgl(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    display_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/
    lv_disp_buf_init(&draw_buf_dsc, draw_buf_1, draw_buf_2, LV_HOR_RES_MAX * LV_VER_RES_MAX);   /*Initialize the display buffer*/

    debe_layer_init(1);
    debe_layer_set_addr(1,draw_buf_1);
	debe_layer_enable(1);

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = LCD_H_RES;
    disp_drv.ver_res = LCD_V_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = display_flush;

    /*Set a display buffer*/
    disp_drv.buffer = &draw_buf_dsc;

#if LV_USE_GPU
    /*Optionally add functions to access the GPU. (Only in buffered mode, LV_VDB_SIZE != 0)*/

    /*Blend two color array using opacity*/
    disp_drv.gpu_blend_cb = gpu_blend;

    /*Fill a memory array with a color*/
    disp_drv.gpu_fill_cb = gpu_fill;
#endif

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */


static void display_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

	//dma_cache_sync(color_p, LCD_H_RES*LCD_V_RES*4, DMA_TO_DEVICE);
	cache_flush_range((uint32_t)color_p, (uint32_t)color_p+LCD_H_RES*LCD_V_RES*4);
	//cache_clean_range((uint32_t)color_p, (uint32_t)color_p+LCD_H_RES*LCD_V_RES*4);
	debe_layer_set_addr(1, color_p);

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}


/*OPTIONAL: GPU INTERFACE*/
#if LV_USE_GPU

/* If your MCU has hardware accelerator (GPU) then you can use it to blend to memories using opacity
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa)
{
    /*It's an example code which should be done by your GPU*/
    uint32_t i;
    for(i = 0; i < length; i++) {
        dest[i] = lv_color_mix(dest[i], src[i], opa);
    }
}

/* If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
                    const lv_area_t * fill_area, lv_color_t color)
{
    /*It's an example code which should be done by your GPU*/
    int32_t x, y;
    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/

    for(y = fill_area->y1; y <= fill_area->y2; y++) {
        for(x = fill_area->x1; x <= fill_area->x2; x++) {
            dest_buf[x] = color;
        }
        dest_buf+=dest_width;    /*Go to the next line*/
    }
}

#endif  /*LV_USE_GPU*/

void display_init(void)
{
    de_config_t config;

	config.width = 800;
	config.height = 480;
    config.bus_width = DE_LCD_R_6BITS | DE_LCD_G_6BITS | DE_LCD_G_6BITS;

	config.pixel_clock_hz = 33000000;
	config.h_front_porch = 40;
	config.h_back_porch = 87;
	config.h_sync_len = 1;
	config.v_front_porch = 13;
	config.v_back_porch = 31;
	config.v_sync_len = 3;
	config.h_sync_invert = 1;
	config.v_sync_invert = 1;
	
    display_gpio_init();
    de_init(&config);
    pwm_f1c100s_init(&led_pwm_bl);
    debe_set_bg_color(0x0000FF00);
	debe_load(DEBE_UPDATE_AUTO);
}

void display_set_bl(int32_t brightness)
{
	pwm_f1c100s_set_duty(&led_pwm_bl, brightness);
}

static void display_gpio_init(void)
{
	for (uint8_t i = 0; i <= 12; i++)
	{
		gpio_f1c100s_set_cfg(&GPIO_PD, i, 0x02);
		gpio_f1c100s_set_pull(&GPIO_PD, i, GPIO_PULL_NONE);
		gpio_f1c100s_set_drv(&GPIO_PD, i, GPIO_DRV_STRONG);
	}
	for (uint8_t i = 13; i <= 21; i++)
	{
		gpio_f1c100s_set_cfg(&GPIO_PD, i, 0x02);
		gpio_f1c100s_set_pull(&GPIO_PD, i, GPIO_PULL_NONE);
		gpio_f1c100s_set_drv(&GPIO_PD, i, GPIO_DRV_STRONG);
	}
}

