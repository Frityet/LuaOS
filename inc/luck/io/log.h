/*nonnull *nonnull
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
#include "macro_util.h"
#include "string.h"

#include "luck/arch/x86_64/cpu.h"

$nonnull_begin

void _log_level_success(void);
void _log_level_info(void);
void _log_level_debug(void);
void _log_level_warning(void);
void _log_level_error(void);
void _log_level_panic(void);

void _log_begin(const char* file, const char *line, const char* function);

void _log_level_common_end(const char *nonnull *nonnull fmtref);
noreturn void _log_level_panic_end(const char *nonnull *nonnull fmtref);

#define _log_level_success_end _log_level_common_end
#define _log_level_info_end _log_level_common_end
#define _log_level_debug_end _log_level_common_end
#define _log_level_warning_end _log_level_common_end
#define _log_level_error_end _log_level_common_end

#define _log__formatters(X) \
    X(char, char) \
    X(string, const char *) \
    X(signed8, signed char) \
    X(signed16, short) \
    X(signed32, int) \
    X(signedptr, long) \
    X(signed64, long long) \
    X(unsigned8, unsigned char) \
    X(unsigned16, unsigned short) \
    X(unsigned32, unsigned int) \
    X(unsignedptr, unsigned long) \
    X(unsigned64, unsigned long long) \
    X(cpucontext, CPUContext)\
    X(voidptr, const void *nullable)

#define _log__defines(name, type) void _log_##name(const char *nonnull *nonnull fmtref, type value);
_log__formatters(_log__defines)
#undef _log__defines

#define _log__eachtype_cb(name, type) , type: _log_##name

#define _log__one(_, argument) \
    {\
        __auto_type _argument = (argument);\
        _Generic(_argument _log__formatters(_log__eachtype_cb), char *: _log_string, \
                 default: _log_voidptr)(&_fmt, _argument);\
    }

#define $write_log(level, fmt, ...) ({ \
        _log_level_##level(); \
        _log_begin(__FILE_NAME__, $stringify(__LINE__), __FUNCTION__); \
        const char* _fmt = (fmt); \
        $foreach(_log__one, _ __VA_OPT__(,) __VA_ARGS__) \
        _log_level_##level##_end(&_fmt); \
    })

#define $success(fmt, ...)  $write_log(success, fmt __VA_OPT__(,) __VA_ARGS__)
#define $info(fmt, ...)     $write_log(info, fmt __VA_OPT__(,) __VA_ARGS__)
#define $debug(fmt, ...)    $write_log(debug, fmt __VA_OPT__(,) __VA_ARGS__)
#define $warning(fmt, ...)  $write_log(warning, fmt __VA_OPT__(,) __VA_ARGS__)
#define $error(fmt, ...)    $write_log(error, fmt __VA_OPT__(,) __VA_ARGS__)
#define $panic(fmt, ...)    $write_log(panic, fmt __VA_OPT__(,) __VA_ARGS__)

$nonnull_end
