/**
 * Copyright (C) 2021-2024 Amrit Bhogal
 *
 * This file is part of LuaOS.
 *
 * LuaOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LuaOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LuaOS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "luck/arch/x86_64/interrupts/idt.h"
#include "luck/arch/x86_64/cpu.h"

#include "luck/io/log.h"

extern void int_div_by_zero(void *);
extern void int_breakpoint(void *);
extern void int_double_fault(void *);
extern void int_general_protection(void *);
extern void int_debug(void *);
extern void int_invalid_opcode(void *);


static struct IDTEntry idt[256];
static struct IDTRegister idtr = {
    .limit = sizeof(struct IDTEntry) * 256 - 1,
    .base = (qword)idt,
};

qword* get_idt_targets(void);

void idt_init()
{
    qword* handlers = get_idt_targets();
    for (int i = 0;i < 256;i++) {
        qword handler = handlers[i];
        idt[i] = (struct IDTEntry) {
            .offset_low = (dword)handler,
            .selector = 0x28,
            .interrupt_stack_table = 0,
            .flags = 0x8E,
            .offset_middle = handler >> 16,
            .offset_high = handler >> 32,
            .reserved = 0
        };
    }

    $asm("LIDT %0" :: "m"(idtr));
}

[[gnu::always_inline]]
static inline void print_cpu_info(CPUContext ctx)
{
    $info("CPU Info");
    $info("RAX: {} | RBX: {} | RCX: {} | RDX: {}", (void *)ctx.rax, (void *)ctx.rbx, (void *)ctx.rcx, (void *)ctx.rdx);
    $info("RSI: {} | RDI: {} | RSP: {} | RBP: {}", (void *)ctx.rsi, (void *)ctx.rdi, (void *)ctx.rsp, (void *)ctx.rbp);
    $info("R8:  {} | R9:  {} | R10: {} | R11: {}", (void *)ctx.r8, (void *)ctx.r9, (void *)ctx.r10, (void *)ctx.r11);
    $info("R12: {} | R13: {} | R14: {} | R15: {}", (void *)ctx.r12, (void *)ctx.r13, (void *)ctx.r14, (void *)ctx.r15);
    $info("RIP: {} | RFL: {} | ERR: {} | ISR: {}", (void *)ctx.rip, (void *)ctx.rflags, (void*)ctx.error, (void*)ctx.interrupt_number);
}

void handle_lapic_irq(CPUContext *ctx);
void handle_interrupt(CPUContext *cpu)
{
    // INT3 is used by the threadsweeper (the special pseudo-thread we spawn when a thread is gonna die with a fixed 16k stack)
    if (cpu->interrupt_number == 0xEF || cpu->interrupt_number == 0x03) {
        handle_lapic_irq(cpu);
        return;
    }
    print_cpu_info(*cpu);
    $panic("unexpected interrupt!");
}
