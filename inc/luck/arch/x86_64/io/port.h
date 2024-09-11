/**
 * Copyright (C) 2021-2024 Amrit Bhogal, pitust
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

byte port_in_byte(word port);
void port_out_byte(word port, byte data);

word port_in_word(word port);
void port_out_word(word port, word data);

#define $port_in(T) (_Generic(typeof(T), word: port_in_word, default: port_in_byte))
#define $port_out(T) (_Generic(typeof(T), word: port_out_word, default: port_out_byte))

$nonnull_end
