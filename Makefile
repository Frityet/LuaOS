# Copyright (C) 2021-2024 Amrit Bhogal
#
# This file is part of LuaOS.
#
# LuaOS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# LuaOS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with LuaOS.  If not, see <http://www.gnu.org/licenses/>.



CC = clang
LD = ld.lld
LUAJIT = luajit

GDB := x86_64-elf-gdb

CFLAGS = -g -O0 -pipe -Wall -Wextra -Werror -Wno-unused -fms-extensions -Wno-microsoft
NASMFLAGS = -F dwarf -g -f elf64

CFLAGS +=       						\
    -std=gnu2x           				\
    -ffreestanding       				\
    -fno-stack-protector 				\
    -fno-stack-check     				\
    -fno-lto             				\
    -fno-pie             				\
    -fno-pic             				\
    -m64                 				\
    -march=x86-64        				\
    -mabi=sysv           				\
    -mno-80387           				\
    -mno-mmx             				\
    -mno-sse             				\
    -mno-sse2            				\
    -mno-red-zone        				\
    -mcmodel=kernel      				\
    -MMD                 				\
	-target x86_64-elf	 				\
	-nostdinc							\
	-isystem extern/					\
	-isystem extern/LuaJIT/src			\
	-Iinc               				\
	-Wno-unused-command-line-argument	\
	-Wanon-enum-enum-conversion			\
	-Wassign-enum						\
	-Wenum-conversion					\
	-Wenum-enum-conversion				\
	-Wno-unused-function    			\
	-Wno-unused-parameter   			\
	-Wnull-dereference      			\
	-Wnull-conversion       			\
	-Wnullability-completeness			\
	-Wnullable-to-nonnull-conversion	\
	-Wno-missing-field-initializers		\
	-fno-omit-frame-pointer 			\
	-Wno-deprecated-attributes			\
	-fms-extensions\
	-fblocks

LDFLAGS =         			\
    -nostdlib               \
    -static                 \
    -m elf_x86_64           \
    -z max-page-size=0x1000 \
    -T res/linker.ld		\
	-no-pie

ASFLAGS = -f elf64

CFILES := $(shell find ./src -type f -name '*.c') extern/terminal/term.c extern/terminal/backends/framebuffer.c
ASFILES := $(shell find ./src -type f -name '*.asm')

USERLAND_FILES := $(shell find ./Userland -type f -name '*.lua')

COBJS := $(addprefix build/obj/,$(CFILES:.c=.c.o))
ASOBJS := $(addprefix build/obj/,$(ASFILES:.asm=.asm.o))

QEMUFLAGS := -smp 2 -m 2G -monitor stdio -serial file:luaos.log -vga std

QDF ?= -s

.PHONY: all
all: build/bin/luaos.iso extern/ovmf-x64

.PHONY: uefi
uefi: extern/ovmf-x64 build/bin/luaos.iso
	qemu-system-x86_64 -M q35 $(QEMUFLAGS) -bios extern/ovmf-x64/OVMF.fd -cdrom build/bin/luaos.iso -boot d $(QDF)

.PHONY: bios
bios: build/bin/luaos.iso
	qemu-system-x86_64 -M q35 $(QEMUFLAGS) -cdrom build/bin/luaos.iso -boot d $(QDF)

extern/LuaJIT/libluajit_luck.o:
	@/usr/bin/printf "[\033[1;35mKernel - extern\033[0m] \033[32mBuilding LuaJIT\n\033[0m"
	@$(MAKE) -C extern/LuaJIT CC="$(CC) -Wno-implicit-function-declaration"

extern/ovmf-x64:
	@/usr/bin/printf "[\033[1;35mKernel\033[0m] \033[32mDownloading OVMF\n\033[0m"
	@mkdir -p $@
	cd $@ && curl -o OVMF-X64.zip https://efi.akeo.ie/OVMF/OVMF-X64.zip && 7z x OVMF-X64.zip

extern/limine/limine:
	@/usr/bin/printf "[\033[1;35mKernel - extern\033[0m] \033[32mBuilding Limine\n\033[0m"
	@cd extern/limine && git reset
# compiling limine requires that our LDFLAGS are not inherited
	@$(MAKE) -C extern/limine LDFLAGS=""

extern/LuaJIT/src/lua.h: extern/LuaJIT

user-land: build-userland.lua
	@/usr/bin/printf "[\033[1;35mUserland\033[0m] \033[32mBuilding userland\n\033[0m"
	@$(LUAJIT) build-userland.lua

res/limine.cfg: user-land

build/bin/luaos.iso: extern/limine extern/limine/limine build/bin/luck.elf res/limine.cfg user-land
	@/usr/bin/printf "[\033[1;35mKernel\033[0m] \033[32mBuilding ISO\n\033[0m"
	@mkdir -p $(dir $@)/iso

# All files in Userland/lua_modules/share/lua/5.1/ will be copied to the root of the ISO
	cp -r Userland/lua_modules/share/lua/5.1/* $(dir $@)/iso

	# cp \
	# 	build/bin/luck.elf res/powered-by-lua.bmp res/limine.cfg \
	# 	res/font.bin extern/limine/limine-cd.bin extern/limine/limine.sys \
	# 	extern/limine/limine-cd-efi.bin\
	# 	$(dir $@)/iso
	cp build/bin/luck.elf $(dir $@)/iso/luck.elf
	cp res/powered-by-lua.bmp res/limine.cfg res/font.bin $(dir $@)/iso
	cp extern/limine/limine-uefi-cd.bin extern/limine/limine-bios.sys extern/limine/BOOTX64.EFI $(dir $@)/iso
	xorriso -as mkisofs\
			-b limine-uefi-cd.bin\
			-no-emul-boot\
			-boot-load-size 4\
			-boot-info-table\
			--efi-boot BOOTX64.EFI\
			-efi-boot-part\
			--efi-boot-image\
			--protective-msdos-label\
			$(dir $@)/iso -o $@
	rm -rf $(dir $@)/iso

	extern/limine/limine bios-install $@
	@/usr/bin/printf "[\033[1;35mKernel\033[0m] \033[32mISO built at \033[33m$@\n\033[0m"

build/bin/luck.elf: $(COBJS) $(ASOBJS) extern/LuaJIT/libluajit_luck.o
	@/usr/bin/printf "[\033[1;35mKernel\033[0m] \033[32mLinking \033[33m$@\n\033[0m"
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $^

build/obj/extern/%.c.o: extern/limine extern/terminal extern/LuaJIT
	@/usr/bin/printf "[\033[1;35mKernel - extern\033[0m] \033[32mCompiling \033[33m$<\n\033[0m"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(shell echo "$@" | sed 's/build\/obj\///g' | sed 's/\.o//g') -o $@

build/obj/./src/%.c.o: src/%.c extern/limine extern/terminal extern/LuaJIT
	@/usr/bin/printf "[\033[1;35mKernel\033[0m] \033[32mCompiling \033[33m$<\n\033[0m"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/obj/%.asm.o: %.asm
	@/usr/bin/printf "[\033[1;35mKernel\033[0m] \033[32mAssembling \033[33m$^\n\033[0m"
	@mkdir -p $(dir $@)
	nasm $(NASMFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -rf build
	find . -type f -name '*.o' -delete
	rm -rf Userland/.luarocks
	rm -rf Userland/lua_modules
	rm -rf Userland/lua
	rm -rf Userland/luarocks
	$(MAKE) -C extern/limine clean

-include $(CFILES:%.c=build/obj/%.c.d)
