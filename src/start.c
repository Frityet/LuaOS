/**
 * Copyright (C) 2023 Amrit Bhogal
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

#include <limine.h>

#include "luck/mm.h"
#include "luck/magazines.h"
#include "luck/io/log.h"
#include "luck/arch/x86_64/gdt.h"
#include "luck/arch/x86_64/interrupts.h"

void kernel_start()
{
    gdt_init();
    idt_init();
    mag_init();
    kalloc_init();

    info("started the luaOS kernel!");
    info("2 + 2 = {:~^15}", 4);
    // info("kernel_start: {}", (void*)kernel_start);
    info("cool addr: {:#x}", (qword)(void*)kalloc(69));

    asm("ud2");
    halt();
}
