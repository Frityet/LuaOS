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

#include <lua.h>
#include "luck/io/log.h"

#include "lj-libc/limits.h"
#include <lauxlib.h>

#include "luck/io/framebuffer.h"
#include "luck/processes/scheduler.h"
#include "luck/io/console.h"
#include "luck/arch/x86_64/io/ps2.h"
#include "string.h"


#define $lua_write_log(source, line, func, level, fmt, ...) ({ \
        char buf[32] = {0};\
        _log_level_##level(); \
        _log_begin(source, integer_to_string(32, buf, line), func); \
        const char* _fmt = (fmt); \
        $foreach(_log__one, _ __VA_OPT__(,) __VA_ARGS__) \
        _log_level_##level##_end(&_fmt); \
    })

static void stdout_write(const char *str, size_t siz)
{
    while (siz) {
        console_write_char(*str++);
        siz--;
    }
}

static size_t ps2_gets(char *buf)
{
    size_t n = 0;
    char *start = buf;
    while (true) {
        char c = ps2_getc();
        if (c == '\n') {
            stdout_write("\n", 1);
            *buf = 0;
            return n;
        }
        if (c == '\b') {
            if (buf == start) continue;
            buf--;
            stdout_write("\b \b", 3);
            continue;
        }
        n++;
        *buf++ = c;
        stdout_write(&c, 1);
    }
}


static int kernel_log_debug(lua_State *L)
{
    const char *msg = luaL_checkstring(L, 1);
    struct lua_Debug dbg = {0};
    lua_getstack(L, 1, &dbg);
    lua_getinfo(L, "nSl", &dbg);
    $lua_write_log(dbg.source, dbg.currentline, dbg.name, debug, msg);
    return 0;
}

static int kernel_log_info(lua_State *L)
{
    const char *msg = luaL_checkstring(L, 1);
    struct lua_Debug dbg = {0};
    lua_getstack(L, 1, &dbg);
    lua_getinfo(L, "nSl", &dbg);
    $lua_write_log(dbg.source, dbg.currentline, dbg.name, info, msg);
    return 0;
}

static int kernel_log_warning(lua_State *L)
{
    const char *msg = luaL_checkstring(L, 1);
    struct lua_Debug dbg = {0};
    lua_getstack(L, 1, &dbg);
    lua_getinfo(L, "nSl", &dbg);
    $lua_write_log(dbg.source, dbg.currentline, dbg.name, warning, msg);
    return 0;
}

static int kernel_log_error(lua_State *L)
{
    const char *msg = luaL_checkstring(L, 1);
    struct lua_Debug dbg = {0};
    lua_getstack(L, 1, &dbg);
    lua_getinfo(L, "nSl", &dbg);
    $lua_write_log(dbg.source, dbg.currentline, dbg.name, error, msg);
    return 0;
}

static int kernel_log_panic(lua_State *L)
{
    const char *msg = luaL_checkstring(L, 1);
    struct lua_Debug dbg = {0};
    lua_getstack(L, 1, &dbg);
    lua_getinfo(L, "nSl", &dbg);
    $lua_write_log(dbg.source, dbg.currentline, dbg.name, panic, msg);
    return 0;
}

static const luaL_Reg libkernel_log[] = {
    { "debug", kernel_log_debug },
    { "info", kernel_log_info },
    { "warning", kernel_log_warning },
    { "error", kernel_log_error },
    { "panic", kernel_log_panic },
    { nullptr, nullptr }
};

static int kernel_write(lua_State *L)
{
    size_t n = 0;
    const char *str = luaL_checklstring(L, 1, &n);
    stdout_write(str, n);

    return 0;
}

static int kernel_read(lua_State *L)
{
    static char buf[1024 * 1024] = {0};
    size_t n = ps2_gets(buf);
    lua_pushlstring(L, buf, n);
    return 1;
}

static const luaL_Reg libkernel[] = {
    { "write", kernel_write },
    { "read", kernel_read },
    { nullptr, nullptr }
};

int luaopen_kernel(lua_State *L)
{
    luaL_newlib(L, libkernel);
    lua_setglobal(L, "kernel");

    #define $add_module(named) ({ \
        lua_getglobal(L, "kernel"); \
        luaopen_##named(L); \
        lua_setfield(L, -2, #named); \
    })

    lua_getglobal(L, "kernel");
    luaL_newlib(L, libkernel_log);
    lua_setfield(L, -2, "log");

    $add_module(framebuffer);
    $add_module(scheduler);

    #undef $add_module

    return 0;
}
