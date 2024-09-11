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

#include <stdint.h>

#include "common.h"

//#define APIC_BASE_MSR 0x1B
$closed_enum ModelSpecificRegister: dword {
    ModelSpecificRegister_APIC_BASE = 0x1B
};

static inline qword read_msr(register enum ModelSpecificRegister msr)
{
    register dword low, high;
    $asm("RDMSR" : "=a"(low), "=d"(high) : "c"(msr));
    return ((qword)high << 32) | low;
}

static inline void write_msr(register enum ModelSpecificRegister msr, qword value)
{
    register dword low = value & 0xFFFFFFFF;
    register dword high = value >> 32;
    $asm("WRMSR" : : "a"(low), "d"(high), "c"(msr));
}
