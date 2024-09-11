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

#include "memory.h"
#include "luck/io/log.h"
#include "luck/bootloader/limine.h"

struct SDTHeader *sdt_find(const struct RSDP *rsdp, const char id[static 4], int idx)
{
    struct RSDT *rsdt = $virt(rsdp->rsdt_address, struct RSDT);
    size_t nent = (rsdt->length - sizeof(struct SDTHeader)) / sizeof(dword);
    for (size_t i = 0;i < nent;i++) {
        struct SDTHeader *sdt = $virt(rsdt->pointers[i], struct SDTHeader);
        if (!memcmp(sdt->signature, id, 4) && idx-- == 0) return sdt;
    }

    return nullptr;
}


