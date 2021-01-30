#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "gpio-f1c100s.h"
#include "io.h"
#include "display.h"
#include "f1c100s_de.h"
#include "lcd.h"
#include "arm32.h"
#include "touch.h"
#include "lvgl.h"
#include "lv_demo_widgets.h"

#define UART_BASE 0x01C25000
#define TIMER_BASE 0x01C20C00
#define INTC_BASE 0x01C20400

#define UART_RBR (uint32_t*)(UART_BASE + 0x00 )
#define UART_THR (uint32_t*)(UART_BASE + 0x00 )
#define UART_DLL (uint32_t*)(UART_BASE + 0x00 )
#define UART_IER (uint32_t*)(UART_BASE + 0x04 )
#define UART_DLM (uint32_t*)(UART_BASE + 0x04 )
#define UART_FCR (uint32_t*)(UART_BASE + 0x08 )
#define UART_LCR (uint32_t*)(UART_BASE + 0x0C )
#define UART_MCR (uint32_t*)(UART_BASE + 0x10 )
#define UART_LSR (uint32_t*)(UART_BASE + 0x14 )
#define UART_MSR (uint32_t*)(UART_BASE + 0x18 )
#define UART_SCR (uint32_t*)(UART_BASE + 0x1C )

#define UART_USR (uint32_t*)(UART_BASE + 0x7C )

void _putchar(char c);
void dump_regs(uint32_t addr, uint32_t len);
void Reset_CPU(void);

void timer_init(void);
void Timer_IRQ_Handler(void);

void lvgl_print_cb(lv_log_level_t level, const char *file, uint32_t line, const char *func, const char *buf);

int main(void)
{
    sys_mmu_init();

    printf("Hello from firmware!\r\n");
    timer_init();
//  lcd_init();

    arm32_interrupt_enable();

    lv_init();
    lv_log_register_print_cb(lvgl_print_cb);
    display_init_lvgl();
    display_set_bl(100);
    //touch_init_lvgl();

//    *UART_IER |= 1;
//    *((uint32_t*)(INTC_BASE+0x20)) |= (1 << 1);
//    *((uint32_t*)(INTC_BASE+0x30)) &= ~0;

    lv_demo_widgets();

    while (1)
    {
        lv_task_handler();
    }
    return 0;
}

void timer_init(void)
{
    *((uint32_t*) (TIMER_BASE + 0x14)) = 24000000UL / 1000UL; // T0 interval
    *((uint32_t*) (TIMER_BASE + 0x10)) = (0 << 4) | (1 << 2) | (1 << 1); // T0 control
    *((uint32_t*) (TIMER_BASE + 0x00)) |= (1 << 0); // T0 IRQ EN

    *((uint32_t*) (INTC_BASE + 0x20)) |= (1 << 13);    // IRQ
    *((uint32_t*) (INTC_BASE + 0x30)) &= ~(1 << 13);

    *((uint32_t*) (TIMER_BASE + 0x10)) |= (1 << 0); // T0 control
}

volatile uint8_t tick_flag = 0;
inline void Timer_IRQ_Handler(void)
{
    lv_tick_inc(1);
    tick_flag = 1;
    *((uint32_t*) (TIMER_BASE + 0x04)) |= (1 << 0); // T0 IRQ clear
}

void delay(uint32_t t)
{
    while (1)
    {
        if (t == 0)
            return;
        if (tick_flag == 1)
        {
            tick_flag = 0;
            t--;
        }
    }
}

void lvgl_print_cb(lv_log_level_t level, const char *file, uint32_t line, const char *func, const char *buf)
{
    /*Use only the file name not the path*/
    size_t p;
    for (p = strlen(file); p > 0; p--)
    {
        if (file[p] == '/' || file[p] == '\\')
        {
            p++; /*Skip the slash*/
            break;
        }
    }

    static const char *lvl_prefix[] =
        { "Trace", "Info", "Warn", "Error", "User" };
    printf("%s: %s \t(%s #%lu %s())\r\n", lvl_prefix[level], buf, &file[p], line, func);
}

void UART0_IRQ_Handler(void)
{
    uint32_t fcr = *UART_FCR;
    uint8_t data = 0;
    //printf("UART_IRQ %d!\r\n",fcr&0x0F);
    switch (fcr & 0x0F)
    {
    case 1:
        break;
    case 4:
    case 6:
    case 12:
        data = *UART_RBR;
        //printf("Rx: %02X\r\n",data);
        break;
    case 7:
        data = *UART_USR;
        break;
    default:
        break;
    }
}

void dump_regs(uint32_t addr, uint32_t len)
{
    for (uint32_t i = addr; i < addr + len; i += 4)
    {
        printf("%08lX: %08lX\r\n", i, read32(i));
    }
}

void Reset_CPU(void)
{
    *((uint32_t*) (TIMER_BASE + 0xB4)) = 1;
    *((uint32_t*) (TIMER_BASE + 0xB8)) = 1;
    *((uint32_t*) (TIMER_BASE + 0xB0)) = 1 | (0xA57 << 1);
    while (1)
        ;
}

void IRQ_Handler_global(void)
{
    uint8_t irq_src = *((uint32_t*) INTC_BASE) >> 2; // INTC_0

    switch (irq_src)
    {
    case 1:
        UART0_IRQ_Handler();
        break;
    case 13:
        Timer_IRQ_Handler();
        break;
    case 20:
        Touch_IRQ_Handler();
        break;
    default:
        printf("UNKNOWN IRQ %d!\r\n", irq_src);
        break;
    }
}

void _putchar(char c)
{
    while (!(*(volatile uint32_t*) (UART_LSR ) & (1 << 5)))
        ;
    *UART_THR = (uint32_t) c;
}
