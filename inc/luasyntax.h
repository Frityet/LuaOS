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

#define if if (
#define then )
#define else } else
#define elseif else if (
#define do do {
#define while while (
#define repeat do
#define until(...) while __VA_ARGS__)
#define for(init, cond, inc) for (__auto_type init; cond; inc)
#define end }
#define _FUNC_END(...) (__VA_ARGS__) {
#define function(name) name _FUNC_END
