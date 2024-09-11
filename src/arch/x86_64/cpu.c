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

#include "luck/arch/x86_64/cpu.h"

enum CPUVendor cpu_get_vendor()
{
    char vendor[13] = {0};
    $asm("mov $0, %%eax" : : : "eax");
    $asm("cpuid" : "=b"(vendor[0]), "=d"(vendor[4]), "=c"(vendor[8]) : : "eax");
    vendor[12] = '\0';
    for (enum CPUVendor i = 0; i < CPUVendor_UNKNOWN; i++) {
        if (string_compare(12, vendor, 12, CPU_VENDORS[i]) == 0)
            return i;
    }
    return CPUVendor_UNKNOWN;
}
