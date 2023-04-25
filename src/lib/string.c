/**
 * Copyright (C) 2023 pitust
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

#include "string.h"
#include "luck/memory/manager.h"

char *strchr(const char *s, int c) {
    while (*s) if (*s++ == c) return (char*)s;
    return (char*)s;
}
char *strrchr(const char *s, int c)
{
    const char* o = s + string_length(s);
    while (*s) if (*s++ == c) o = s-1;
    return (char*)o;
}
size_t strlen(const char *s)
{ return string_length(s); }

int strcmp(const char *s1, const char *s2)
{ return string_compare(string_length(s1), s1, string_length(s2), s2); }

char *strcpy(char *dst, const char *src)
{
    size_t bufsiz = string_length(src) + 1;
    string_copy(bufsiz, dst, bufsiz, src);
    return dst;
}

char *strncpy(char *dst, const char *src, size_t n)
{
    string_copy(n, dst, n, src);
    return dst;
}

char *strstr(char *str, char const *to_find)
{
    if (!(*to_find))
        return str;
    for (int i = 0; *(str + i); i++) {
        int j;
        for (j = 0; *(to_find + j); j++) {
            if (*(str + i + j) != *(to_find + j))
                break;
        }
        if (!(*(to_find + j)))
            return str + i;
    }
    return NULL;
}
