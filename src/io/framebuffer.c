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

#define OLIVEC_IMPLEMENTATION
#include "luck/io/framebuffer.h"
#include "luck/bootloader/limine.h"

#include "memory.h"
#include "luck/memory/manager.h"
#include "lj-libc/limits.h"
#include <lauxlib.h>

#define CANVAS_T "OliveCanvas"

enum {
    /*Largest framebuffer supported*/
    Framebuffer_SAVE_SIZE = 1280 * 1280
};

Olivec_Canvas framebuffer_init()
{
    struct limine_framebuffer *nonnull fb = bootloader_framebuffer->framebuffers[0];
    return olivec_canvas(fb->address, fb->width, fb->height, fb->width);
}

static int libframebuffer_canvas_fill(lua_State *L)
{
    Olivec_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_T);
    dword colour = luaL_checkinteger(L, 2);

    olivec_fill(*canvas, colour);

    return 0;
}

static int libframebuffer_canvas_set_pixel(lua_State *L)
{
    Olivec_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_T);
    dword x = luaL_checkinteger(L, 2);
    dword y = luaL_checkinteger(L, 3);
    dword colour = luaL_checkinteger(L, 4);

    if (x >= canvas->width || y >= canvas->height) {
        lua_pushnil(L);
        lua_pushstring(L, "pixel out of bounds");
        return 2;
    }

    OLIVEC_PIXEL(*canvas, x, y) = colour;

    return 0;
}

static int libframebuffer_canvas_get_pixel(lua_State *L)
{
    Olivec_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_T);
    dword x = luaL_checkinteger(L, 2);
    dword y = luaL_checkinteger(L, 3);

    if (x >= canvas->width || y >= canvas->height) {
        lua_pushnil(L);
        lua_pushstring(L, "pixel out of bounds");
        return 2;
    }

    lua_pushinteger(L, OLIVEC_PIXEL(*canvas, x, y));

    return 1;
}

static int libframebuffer_canvas_draw_line(lua_State *L)
{
    Olivec_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_T);
    auto x1 = (sdword)luaL_checkinteger(L, 2);
    auto y1 = (sdword)luaL_checkinteger(L, 3);
    auto x2 = (sdword)luaL_checkinteger(L, 4);
    auto y2 = (sdword)luaL_checkinteger(L, 5);
    dword colour = luaL_checkinteger(L, 6);

    olivec_line(*canvas, x1, y1, x2, y2, colour);

    return 0;
}

static int libframebuffer_canvas_draw_rect(lua_State *L)
{
    Olivec_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_T);
    auto x = (sdword)luaL_checkinteger(L, 2);
    auto y = (sdword)luaL_checkinteger(L, 3);
    auto w = (sdword)luaL_checkinteger(L, 4);
    auto h = (sdword)luaL_checkinteger(L, 5);
    dword colour = luaL_checkinteger(L, 6);

    olivec_rect(*canvas, x, y, w, h, colour);

    return 0;
}

static int libframebuffer_canvas_draw_circle(lua_State *L)
{
    Olivec_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_T);
    auto x = (sdword)luaL_checkinteger(L, 2);
    auto y = (sdword)luaL_checkinteger(L, 3);
    auto r = (sdword)luaL_checkinteger(L, 4);
    dword colour = luaL_checkinteger(L, 5);

    olivec_circle(*canvas, x, y, r, colour);

    return 0;
}

static int libframebuffer_canvas_draw_triangle(lua_State *L)
{
    Olivec_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_T);
    auto x1 = (sdword)luaL_checkinteger(L, 2);
    auto y1 = (sdword)luaL_checkinteger(L, 3);
    auto x2 = (sdword)luaL_checkinteger(L, 4);
    auto y2 = (sdword)luaL_checkinteger(L, 5);
    auto x3 = (sdword)luaL_checkinteger(L, 6);
    auto y3 = (sdword)luaL_checkinteger(L, 7);
    dword colour = luaL_checkinteger(L, 8);

    olivec_triangle(*canvas, x1, y1, x2, y2, x3, y3, colour);

    return 0;
}

static int libframebuffer_canvas_draw_text(lua_State *L)
{
    Olivec_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_T);
    auto x = (sdword)luaL_checkinteger(L, 2);
    auto y = (sdword)luaL_checkinteger(L, 3);
    const char *text = luaL_checkstring(L, 4);
    dword colour = luaL_checkinteger(L, 5);
    qword size = luaL_checkinteger(L, 6);


    olivec_text(*canvas, text, x, y, olivec_default_font, size, colour);

    return 0;
}

// function framebuffer:get_dimensions()
static int libframebuffer_dimensions(lua_State *L)
{
    Olivec_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_T);

    lua_pushinteger(L, (sqword)canvas->width);
    lua_pushinteger(L, (sqword)canvas->height);

    return 2;
}

static int libframebuffer_get(lua_State *L)
{
    struct limine_framebuffer *nonnull fb = bootloader_framebuffer->framebuffers[0];

    Olivec_Canvas *canvas = lua_newuserdata(L, sizeof(Olivec_Canvas));
    *canvas = olivec_canvas(fb->address, fb->width, fb->height, fb->width);

    luaL_getmetatable(L, CANVAS_T);
    lua_setmetatable(L, -2);

    return 1;
}

static _Atomic dword save_buffer[Framebuffer_SAVE_SIZE];

static int libframebuffer_canvas_save(lua_State *L)
{
    Olivec_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_T);

    memory_copy(save_buffer, canvas->pixels, canvas->width * canvas->height * sizeof(dword));
    return 0;
}

static int libframebuffer_canvas_load(lua_State *L)
{
    Olivec_Canvas *canvas = luaL_checkudata(L, 1, CANVAS_T);

    memory_copy(canvas->pixels, save_buffer, canvas->width * canvas->height * sizeof(dword));

    return 0;
}

static const luaL_Reg libframebuffer_canvas[] = {
    { "fill", libframebuffer_canvas_fill },
    { "set_pixel", libframebuffer_canvas_set_pixel },
    { "get_pixel", libframebuffer_canvas_get_pixel },
    { "draw_line", libframebuffer_canvas_draw_line },
    { "draw_rect", libframebuffer_canvas_draw_rect },
    { "draw_circle", libframebuffer_canvas_draw_circle },
    { "draw_triangle", libframebuffer_canvas_draw_triangle },
    { "draw_text", libframebuffer_canvas_draw_text },
    { "dimensions", libframebuffer_dimensions },
    { "save", libframebuffer_canvas_save },
    { "load", libframebuffer_canvas_load },
    { nullptr, nullptr }
};

static const luaL_Reg libframebuffer[] = {
    { "get", libframebuffer_get },
    { nullptr, nullptr }
};

int luaopen_framebuffer(lua_State *L)
{
    luaL_newmetatable(L, CANVAS_T);
    // metatable.__index = metatable
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, libframebuffer_canvas, 0);
    lua_pop(L, 1);

    luaL_newlib(L, libframebuffer);
    return 1;
}
