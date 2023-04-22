 /**
 * Copyright (C) 2023 Amrit Bhogal
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

#include <limine/limine.h>
#include <string.h>
#include <terminal/backends/framebuffer.h>
#include <terminal/term.h>

#include "luck/io/console.h"
#include "luck/memory/manager.h"


static const volatile struct limine_framebuffer_request fb_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST
};

static struct term_context *term_ctx;
static uint32_t term_pal[8] = {0x161A21, 0xF8485D, 0x4FD93D, 0xFF9B28, 0x0CB2FF, 0xCA87FF, 0x65E0B8, 0xBBBBBB};
static uint32_t term_pal_bright[8] = {0x565555, 0xFF4D62, 0x8ED71C, 0xECA02D, 0x18B7FF, 0xCA87FF, 0x6AE6BE, 0xFEFFFF};
static uint32_t term_bg = 0x0B0E13, term_fg = 0xDBDBDB;

void initalise_terminal()
{
    if (!fb_request.response) return;
    if (fb_request.response->framebuffer_count == 0) return;
    struct limine_framebuffer *fb = fb_request.response->framebuffers[0];


    term_ctx = fbterm_init(kalloc, fb->address, fb->width, fb->height, fb->pitch, nullptr, term_pal, term_pal_bright, &term_bg, &term_fg, &term_bg, &term_fg, nullptr, 0, 0, 0, 1, 1, 0);
}

void console_write(const char *str)
{
    if (term_ctx) term_write(term_ctx, str, string_length(str));
}
void console_write_char(char c)
{
    if (term_ctx) term_write(term_ctx, &c, 1);
}
