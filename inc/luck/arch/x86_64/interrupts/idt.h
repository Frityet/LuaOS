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

#pragma once

#include "common.h"

$nonnull_begin

struct IDTEntry {
    word offset_low, selector;
    byte interrupt_stack_table, flags;
    word offset_middle;
    dword offset_high, reserved;
};


struct [[gnu::packed]] IDTRegister {
    word limit;
    qword base;
};

void idt_init(void);
void idt_register_int(byte int_no, [[gnu::interrupt]] void(*routine)(void*));

$nonnull_end
