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
#include "luck/io/log.h"

#include <limine/limine.h>
#include "string.h"

#include "luck/bootloader/limine.h"

struct RSDP *rsdp_init(void)
{
    if (bootloader_rsdp == nullptr || bootloader_rsdp->address == nullptr)
        return nullptr;

    struct RSDP *nonnull rsdp = bootloader_rsdp->address;

    if (rsdp->revision < 2)
        $info("  XSDT not supported on this machine; using RSDT");

    return rsdp;
}


