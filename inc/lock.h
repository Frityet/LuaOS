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

#include "stdatomic.h"
#include "common.h"

$nonnull_begin

typedef _Atomic(bool) Lock;

static inline void acquire_lock(Lock *b)
{
    bool spin = false;
    do {
        while (atomic_load_explicit(b, memory_order_relaxed)) { spin = true; $asm("pause"); }
    } while (atomic_exchange(b, true));
}
static inline void release_lock(Lock *b)
{ atomic_store(b, false); }

$nonnull_end
