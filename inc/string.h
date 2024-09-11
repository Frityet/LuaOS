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
#include "memory.h" //Must keep this header!

$nonnull_begin

[[gnu::always_inline]]
static inline size_t string_length(const char *str)
{
    size_t len = 0;
    while (str[len++] != '\0');
    return len;
}

[[gnu::always_inline]]
static inline int string_compare(size_t len1, const char str1[static len1], size_t len2, const char str2[static len2])
{
    size_t len = len1 < len2 ? len1 : len2;

    for (size_t i = 0; i < len; i++) {
        if (str1[i] < str2[i]) return -1;
        if (str1[i] > str2[i]) return 1;
    }
    if (len1 < len2) return -1;
    if (len1 > len2) return 1;
    return 0;
}

[[gnu::always_inline]]
static inline void string_copy(size_t bufsiz, char dest[static bufsiz], size_t n, const char src[static n])
{
    size_t i;
    for (i = 0; i < n && i < bufsiz - 1 && src[i] != '\0'; i++)
        dest[i] = src[i];

    dest[i] = '\0';
}

[[gnu::always_inline]]
static inline char *string_concatenate(size_t bufsiz, char dest[static bufsiz], size_t n, const char src[static n])
{
    size_t len = string_length(dest);
    string_copy(bufsiz - len, dest + len, n, src);
    return dest;
}

static char *integer_to_string(size_t length, char dest[static length], sqword i)
{
    size_t len = 0;
    bool negative = false;
    if (i < 0) {
        negative = true;
        i = -i;
    }
    do {
        dest[len++] = '0' + i % 10;
        i /= 10;
    } while (i != 0);
    if (negative) dest[len++] = '-';
    dest[len] = '\0';
    for (size_t i = 0; i < len / 2; i++) {
        char temp = dest[i];
        dest[i] = dest[len - i - 1];
        dest[len - i - 1] = temp;
    }
    return dest;
}

//libc compat
size_t strlen(const char *str);
int strcmp(const char *str1, const char *str2);
char *strcpy(char *dest, const char *src),
     *strncpy(char *dest, const char *src, size_t n);

$nonnull_end
