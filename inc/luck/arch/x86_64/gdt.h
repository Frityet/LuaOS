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

#pragma once

#include "common.h"

NONNULL_BEGIN

struct [[gnu::aligned(4096)]] TSSDescriptor {
    byte tss[0x6B];
};

struct GlobalDescriptorTable {
    uint64_t descriptors[11];
    struct TSSDescriptor task_state_segment;
};


struct [[gnu::packed]] GDTRegister {
    word limit;
    qword base;
};

void gdt_init(void);

NONNULL_END
