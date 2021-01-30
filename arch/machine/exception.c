/*
 * exception.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <stdio.h>
#include <arm32.h>
#include <printf.h>

extern void IRQ_Handler_global(void);

struct pt_regs {
	long uregs[18];
};

#define ARM_cpsr	uregs[16]
#define ARM_pc		uregs[15]
#define ARM_lr		uregs[14]
#define ARM_sp		uregs[13]
#define ARM_ip		uregs[12]
#define ARM_fp		uregs[11]
#define ARM_r10		uregs[10]
#define ARM_r9		uregs[9]
#define ARM_r8		uregs[8]
#define ARM_r7		uregs[7]
#define ARM_r6		uregs[6]
#define ARM_r5		uregs[5]
#define ARM_r4		uregs[4]
#define ARM_r3		uregs[3]
#define ARM_r2		uregs[2]
#define ARM_r1		uregs[1]
#define ARM_r0		uregs[0]
#define ARM_ORIG_r0	uregs[17]

#define PCMASK		0
#define instruction_pointer(regs) \
	(pc_pointer((regs)->ARM_pc))
#define pc_pointer(v) \
	((v) & ~PCMASK)

struct arm_regs_t {
	uint32_t r[13];
	uint32_t sp;
	uint32_t lr;
	uint32_t pc;
	uint32_t cpsr;
};

static void show_regs(struct arm_regs_t * regs)
{
	int i;

	printf("pc : [<%08lx>] lr : [<%08lx>] cpsr: %08lx\r\n", regs->pc, regs->lr, regs->cpsr);
	printf("sp : %08lx\r\n", regs->sp);
	for(i = 12; i >= 0; i--)
	{
		printf("r%-2d: %08lx ", i, regs->r[i]);
		if(i % 2 == 0)
			printf("\r\n");
	}
	printf("\r\n");
}

void arm32_do_undefined_instruction(struct arm_regs_t * regs)
{
	printf("\r\n!! PREFETCH_ABORT !!\r\n");
	show_regs(regs);
	while(1);
}

void arm32_do_software_interrupt(struct arm_regs_t * regs)
{
	printf("\r\n!! SOFT_INTERRUPT !!\r\n");
	show_regs(regs);
	while(1);
}

void arm32_do_prefetch_abort(struct arm_regs_t * regs)
{
	printf("\r\n!! PREFETCH_ABORT !!\r\n");
	show_regs(regs);
	while(1);
}

void arm32_do_data_abort(struct arm_regs_t * regs)
{
	printf("\r\n!! DATA_ABORT !!\r\n");
	show_regs(regs);
	while(1);
}

void arm32_do_irq(struct pt_regs *pt_regs)
{
	IRQ_Handler_global();
}



void arm32_do_fiq(struct arm_regs_t * regs)
{
	IRQ_Handler_global();
	//interrupt_handle_exception(regs);
}
