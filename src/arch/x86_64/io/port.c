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

#include "luck/arch/x86_64/io/port.h"

byte port_in_byte(word port)
{
    byte data;
    $asm("INB %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

void port_out_byte(word port, byte data)
{
    $asm("OUTB %0, %1" : : "a"(data), "Nd"(port));
}

word port_in_word(word port)
{
    word data;
    $asm("INW %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

void port_out_word(word port, word data)
{
    $asm("OUTW %0, %1" : : "a"(data), "Nd"(port));
}
