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

struct SDTHeader {
    char signature[4];
    dword length;
    byte revision, checksum;
    char oem_id[6], oem_table_id[8];
    dword oem_revision, creator_id, creator_revision;
};

struct RSDT {
    struct SDTHeader;
    dword pointers[];
};

struct [[gnu::packed]] XSDT {
    struct SDTHeader;
    qword pointers[];
};

struct [[gnu::packed]] RSDP {
    //1.0
    char signature[8];
    byte checksum;
    char oem_id[6];
    byte revision;
    dword rsdt_address;

    //2.0
    dword length;
    qword xsdt_address;
    byte extended_checksum, reserved[3];
};

struct RSDP *nullable rsdp_init(void);
struct SDTHeader *nullable sdt_find(const struct RSDP *table, const char id[static 4], int idx);

$nonnull_end
