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

#include "luck/arch/x86_64/acpi/acpi.h"
#include "luck/arch/x86_64/acpi/madt.h"

struct MADT *madt_init(const struct RSDP *rsdp)
{
    struct MADT *madt = (struct MADT *nullable)sdt_find(rsdp, "APIC", 0);
    if (madt == nullptr) return nullptr;

    /*TODO*/

    return madt;
}
