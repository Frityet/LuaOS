-- Copyright (C) 2021-2024 Amrit Bhogal
--
-- This file is part of LuaOS.
--
-- LuaOS is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- LuaOS is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with LuaOS.  If not, see <http://www.gnu.org/licenses/>.

--- Auto generation of limine.cfg

--[[
TIMEOUT=0

TERM_PALETTE=161A21;F8485D;4FD93D;FF9B28;0CB2FF;CA87FF;65E0B8;BBBBBB
TERM_PALETTE_BRIGHT=565555;FF4D62;8ED71C;ECA02D;18B7FF;CA87FF;6AE6BE;FEFFFF
TERM_BACKGROUND=0B0E13
TERM_FOREGROUND=DBDBDB

TERM_FONT=boot:///font.bin
TERM_FONT_SIZE=8x20

TERM_WALLPAPER=boot:///powered-by-lua.bmp

:LuaOS
    PROTOCOL=limine
    KERNEL_PATH=boot:///luck.elf

    MODULE_PATH=boot:///init.lua
    MODULE_CMDLINE=init.lua
]]


print("[\x1b[1;35mUserland\x1b[0m] \x1b[32mBuilding \x1b[33mUserland/lua_modules/share/lua/5.1/\n\x1b[0m")
local suc, _, _ = os.execute "cd Userland && luarocks --lua-version=5.1 init && luarocks make && cd .."
if not suc then
    local suc2, _, ec = os.execute "cd Userland && luarocks-5.1 --lua-version=5.1 init && luarocks-5.1 make && cd .."
    if not suc2 then
        os.exit(ec)
    end
end

---@param proc string
---@param ... string
---@return fun(): string
local function exec(proc, ...)
    local cmd = table.concat({proc, ...}, " ")
    print("\x1b[34m$ \x1b[0m"..cmd.."\x1b[0m")

    local proc = assert(io.popen(cmd, "r"))
    return coroutine.wrap(function()
        for line in proc:lines() do coroutine.yield(line) end
    end)
end

---@generic T
---@param self fun(): T
---@return T[]
local function collect(it)
    local vals = {}
    for val in it do vals[#vals+1] = val end
    return vals
end

---@generic K, V
---@param array V[]
---@param key K | fun(v: V): K
---@return { [K]: V }
local function map(array, key)
    local map = {}

    for _, v in ipairs(array) do
        local k = type(key) == "function" and key(v) or v[key]
        map[k] = v
    end

    return map
end

---Gets all lua files in Userland/lua_modules/share/lua/5.1/
---@return string[]
local function get_userland()
    return collect(exec("/usr/bin/find", "Userland/lua_modules/share/lua/5.1", "-type", "f", "-name", "'*.lua'"))
end

local config = {
    timeout = 0,

    term = {
        pallette = {
            normal = {
                0x161A21,
                0xF8485D,
                0x4FD93D,
                0xFF9B28,
                0x0CB2FF,
                0xCA87FF,
                0x65E0B8,
                0xBBBBBB
            },
            bright = {
                0x565555,
                0xFF4D62,
                0x8ED71C,
                0xECA02D,
                0x18B7FF,
                0xCA87FF,
                0x6AE6BE,
                0xFEFFFF
            }
        },
        background = 0x0B0E13,
        foreground = 0xDBDBDB,

        font = {
            path = "font.bin",
            size = "8x20"
        },

        wallpaper = "powered-by-lua.bmp"
    },

    targets = {
        ["LuaOS"] = {
            protocol = "limine",
            kernel_path = "luck.elf",

            modules = map(get_userland(), function(file)
                return file:match("Userland/lua_modules/share/lua/5.1/(.*)")
            end)
        }
    }
}

print("[\x1b[1;35mUserland\x1b[0m] \x1b[32mBuilding \x1b[33mres/limine.cfg\n\x1b[0m")

-- Dump the config to res/limine.cfg
local file = assert(io.open("res/limine.cfg", "w"))

---Write a line to the limine config
---@nodiscard
---@param ... any
---@return fun(...: any)
local function wl(...)
    file:write(table.concat({...}, ' '))
    return function(...) file:write(table.concat({...}, ' ')..'\n') end
end

wl "TIMEOUT=" (config.timeout)

wl "TERM_PALETTE="(table.concat(config.term.pallette.normal, ";"))
wl "TERM_PALETTE_BRIGHT="(table.concat(config.term.pallette.bright, ";"))
wl "TERM_BACKGROUND="(config.term.background)
wl "TERM_FOREGROUND="(config.term.foreground)

wl "TERM_FONT=boot:///"(config.term.font.path)

wl "TERM_FONT_SIZE="(config.term.font.size)

wl "TERM_WALLPAPER=boot:///"(config.term.wallpaper)

for name, target in pairs(config.targets) do
    wl ":"(name)
    wl "    PROTOCOL="(target.protocol)
    wl "    KERNEL_PATH=boot:///"(target.kernel_path)

    for path, module in pairs(target.modules) do
        wl "    MODULE_PATH=boot:///"(path)
        wl "    MODULE_CMDLINE="(module)
    end
end

file:close()


-- Copy over the modules into the `build/iso/
print("[\x1b[1;35mUserland\x1b[0m] \x1b[32mCopying modules to \x1b[33mbuild/iso/boot/\x1b[0m")
for name, file in pairs(config.targets["LuaOS"].modules) do
    --[[@cast name string]]
    local dir = name:match("(.*)/")
    if dir ~= nil then
        collect(exec("mkdir", "-p", "build/iso/boot/"..dir))
    end
    exec("cp", file, "build/iso/boot/"..name)
    print("[\x1b[1;35mUserland\x1b[0m] \x1b[32mCopied \x1b[33m"..file.."\x1b[32m to \x1b[33mbuild/iso/boot/"..name.."\x1b[0m")
end
