/**
 * Copyright (C) 2021-2024 Louis Dupraz
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

// These are macros defined in math.h.
// They are not used yet but will be used in the future for error handling.
// They may also be used in other files.

#include "../../inc/common.h"
#define NAN 0.0 / 0.0
#define HUGE_VAL 1e10000
#define HUGE_VALF 1e10000f
#define DBL_MAX 179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000
#define INFINITY INF

typedef union {
    float f;
    dword d;
} float_bytes;

typedef union {
    double d;
    qword q;
} double_bytes;

int isnan(double x)
{
    double_bytes bytes = {x};
    bytes.q &= 0x7FFFFFFFFFFFFFFF;
    return ((bytes.q & 0x7FF0000000000000) == 0x7FF0000000000000 && !(bytes.q & 0x000FFFFFFFFFFFFF));
}

int isinf(double x)
{
    double_bytes bytes = {x};
    bytes.q &= 0x7FFFFFFFFFFFFFFF;
    return (bytes.q == 0x7FF0000000000000);
}

double fabs(double x)
{
    double_bytes bytes = {x};
    bytes.q &= 0x7FFFFFFFFFFFFFFF;
    return bytes.d;
}

float fabsf(float x)
{
    float_bytes bytes = {x};
    bytes.d &= 0x7FFFFFFF;
    return bytes.f;
}
