/**
 * Copyright (C) 2021-2024 Amrit Bhogal, pitust
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

#include "luck/arch/x86_64/gdt.h"

#include "luck/io/log.h"

static struct GlobalDescriptorTable gdt;
static struct GDTRegister gdtr = {.base = (qword)&gdt, .limit = sizeof(gdt) - 1};

void gdt_init(void)
{
    gdt.descriptors[0] = 0x0000000000000000; // null
    gdt.descriptors[1] = 0x00009a000000ffff; // 16-bit code
    gdt.descriptors[2] = 0x000093000000ffff; // 16-bit data
    gdt.descriptors[3] = 0x00cf9a000000ffff; // 32-bit code
    gdt.descriptors[4] = 0x00cf93000000ffff; // 32-bit data
    gdt.descriptors[5] = 0x00af9b000000ffff; // 64-bit code
    gdt.descriptors[6] = 0x00af93000000ffff; // 64-bit data
    gdt.descriptors[7] = 0x00affb000000ffff; // usermode 64-bit code
    gdt.descriptors[8] = 0x00aff3000000ffff; // usermode 64-bit data

    gdt.task_state_segment.tss[0x66] = 0x13;

    $asm("LGDT %0" :: "m"(gdtr));
    $success("Done");
}
