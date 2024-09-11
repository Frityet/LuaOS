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

#include <common.h>

$nonnull_begin

static void *memory_copy(void *dst, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
        ((byte *)dst)[i] = ((byte *)src)[i];

    return dst;
}

static void *memory_set(void *dst, int c, size_t n)
{
    for (size_t i = 0; i < n; i++)
        ((byte *)dst)[i] = c;

    return dst;
}

static int memory_compare(const void *b1, const void *b2, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        if (((byte *)b1)[i] < ((byte *)b2)[i]) return -1;
        if (((byte *)b1)[i] > ((byte *)b2)[i]) return 1;
    }

    return 0;
}

static void *memory_move(void *dst, const void *src, size_t n)
{
    if (src < dst) {
        for (size_t i = n; i > 0; i--)
            ((byte *)dst)[i - 1] = ((byte *)src)[i - 1];
    } else {
        for (size_t i = 0; i < n; i++)
            ((byte *)dst)[i] = ((byte *)src)[i];
    }

    return dst;
}

void *memcpy(void *dst, const void *src, size_t n),
     *memmove(void *dst, const void *src, size_t n);

void *memset(void *dst, int c, size_t n);

int memcmp(const void *b1, const void *b2, size_t n);

$nonnull_end
