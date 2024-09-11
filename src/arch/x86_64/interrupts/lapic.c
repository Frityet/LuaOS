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

#define LAPIC_IMPL
#include "luck/arch/x86_64/interrupts/lapic.h"

#include <stdint.h>

#include "luck/processes/scheduler.h"
#include "luck/arch/x86_64/cpu.h"
#include "luck/arch/x86_64/interrupts/idt.h"
#include "luck/arch/x86_64/msr.h"
#include "luck/arch/x86_64/io/port.h"
#include "luck/io/log.h"

#define VECTOR_ID (0xEF)
#define TIMER_DIV (0x10)

static volatile dword counter;
volatile dword *lapic_base;
extern void int_lapic_timer(void *);

dword lapic_read(enum LAPICRegister reg)
{ return lapic_base[reg / 4]; }

void lapic_write(enum LAPICRegister reg, dword value)
{ lapic_base[reg / 4] = value; }

static word pit_count(void)
{
    port_out_byte(0x43, 0x00);
    return port_in_byte(0x40) | (port_in_byte(0x40) << 8);
}

static void set_pit(word count)
{
    port_out_byte(0x43, 0x30);
    port_out_byte(0x40, count & 0xFF);
    port_out_byte(0x40, count >> 8);
}

void pit_set_frequency(dword frequency)
{
    qword div = PIT_DIVIDEND / frequency;
    if (PIT_DIVIDEND % frequency > frequency / 2)
        div++;

    set_pit(div);
}

void handle_lapic_irq(CPUContext *ctx)
{
    if (ctx->interrupt_number == 0xEF) {
        lapic_write(LAPICRegister_END_OF_INTERRUPT, 0x0);
        lapic_write(LAPICRegister_INITAL_COUNT, 1000000); // re-arm timeout
    }

    reschedule(ctx);
}

void lapic_init(void)
{
    qword apic_base_msr = read_msr(ModelSpecificRegister_APIC_BASE);
    apic_base_msr |= (1 << 11);
    write_msr(ModelSpecificRegister_APIC_BASE, apic_base_msr);

    lapic_base = (volatile dword *)((apic_base_msr & 0xFFFFF000) + 0xFFFF800000000000);

    // Set task priority to the lowest (allow all interrupts)
    lapic_write(LAPICRegister_PRIORITY, 0);

    dword timer = lapic_read(LAPICRegister_LVT_TIMER);
    timer &= ~(1 << 16); //clear mask to enable interrupt
    timer |= VECTOR_ID;
    lapic_write(LAPICRegister_LVT_TIMER, timer);

    //setup for div config
    // no, i do *not* know what 0x03 does. it worked for xtrix so it must work here
    // https://git.sr.ht/~pitust/xtrix/tree/trunk/item/src/xtrm/interrupt/lapic.d#L29
    lapic_write(LAPICRegister_DIVIDE_CONFIG, 0x03);

    //Set the init count to smth big
    lapic_write(LAPICRegister_INITAL_COUNT, 1000000);

    // Initalise PIT and sync with it
    pit_set_frequency(1000);


}
