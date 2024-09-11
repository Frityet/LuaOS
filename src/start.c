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

#include <limine/limine.h>

#include "common.h"
#include "lj-libc/limits.h"
#include "luck/io/log.h"

#include "luck/arch/x86_64/acpi/madt.h"
#include "luck/arch/x86_64/acpi/acpi.h"
#include "luck/io/console.h"
#include "luck/io/framebuffer.h"
#include "luck/arch/x86_64/gdt.h"
#include "luck/arch/x86_64/interrupts/idt.h"
#include "luck/arch/x86_64/interrupts/lapic.h"
#include "luck/arch/x86_64/io/ps2.h"
#include "luck/processes/scheduler.h"
#include "luck/memory/manager.h"
#include "luck/memory/magazines.h"
#include "luck/bootloader/limine.h"

#undef stdin
#undef stdout
#undef stderr

$nonnull_begin

static const char *nullable extension(size_t buflen, const char buf[buflen])
{
    //Find the last dot
    size_t i = buflen;
    while (i > 0 && buf[i] != '.') i--;

    if (i == 0) return nullptr;
    return &buf[i + 1];
}

static void stdout_write(const char *str, int siz)
{
    while (siz) {
        console_write_char(*str++);
        siz--;
    }
}

static void ps2_gets(char *buf)
{
    char *start = buf;
    while (true) {
        char c = ps2_getc();
        if (c == '\n') {
            stdout_write("\n", 1);
            *buf = 0;
            return;
        }
        if (c == '\b') {
            if (buf == start) continue;
            buf--;
            stdout_write("\b \b", 3);
            continue;
        }
        *buf++ = c;
        stdout_write(&c, 1);
    }
}

[[gnu::used]] noreturn void kernel_start()
{
    $asm (
        ".intel_syntax noprefix\n"
        "mov rax, cr0\n"
        "and ax, 0xFFFB\n"
        "or ax, 0x2\n"
        "mov cr0, rax\n"
        "mov rax, cr4\n"
        "or ax, 3 << 9\n"
        "mov cr4, rax\n"
        ".att_syntax\n"
    );

    qword cr3;
    $asm("MOVQ %%CR3, %0" : "=r"(cr3));
    for (qword i = 0; i < 256; i++)
        $virt(cr3, qword)[i] = 0;

    $asm("MOVQ %0, %%CR3" :: "r"(cr3) : "memory");


    $success("Started LuaOS");

    $info("CPU Vendor: {}", cpu_get_vendor());

    $info("Initialising bootloader");
    bootloader_init();
    $success("Done");

    $info("Initialising GDT");
    gdt_init();
    $success("Done");

    $info("Initialising IDT");
    idt_init();
    $success("Done");
    $asm("STI");

    $info("Initialising memory");
    {
        $info("  Magazines...");
        magazine_init();
        $success("  Done");

        $info("  Kernel memory allocator...");
        kalloc_init();
        $success("  Done");
    }
    $success("Done");

    terminal_init();
    framebuffer_init();

    if (bootloader_framebuffer == nullptr || bootloader_framebuffer->framebuffer_count == 0)
        $panic("No framebuffer found!");

    $info("Initialising APIC");
    auto rsdp = $assert_nonnull(rsdp_init(), "No RSDP found");
    auto madt = $assert_nonnull(madt_init(rsdp), "No MADT found");

    size_t core_c = 0;

    for (struct MADTEntryHeader *entry = (struct MADTEntryHeader *)madt->entries;
         (uintptr_t)entry < (uintptr_t)(((uintptr_t)madt->entries) + madt->descriptor.length - sizeof(struct MADT));
         entry = (struct MADTEntryHeader *)((byte *)entry + (entry)->length)) {

        $debug("  Found entry with ID {}", entry->id);
        switch (entry->id) {
            case MADTEntryID_LAPIC: {
                struct MADTEntry_LAPIC *lapic = (struct MADTEntry_LAPIC *) entry;
                $success("  Found LAPIC at core {} (address: {})", core_c++, lapic);
                $debug("    Processor ID: {}", lapic->processor_id);
                $debug("    APIC ID: {}", lapic->apic_id);
                $debug("    Flags: {}", lapic->flags);
                break;
            }
        }
    }
    $success("Done");

    $info("Initalising LAPIC");
    lapic_init();
    $info("LAPIC base: {}", lapic_base);
    $success("Done");

    static FILE stdout = {
        .write = stdout_write,

    };
    _get_pcb()->stdout = &stdout;

    if (bootloader_module == nullptr) $panic("no modules available!");
    if (bootloader_module->module_count == 0) $panic("more than one module available!");
    $info("Initialising scheduler");
    scheduler_init();
    $success("Done");

    $success("Initialisation complete");

    $info("Loading {} programs", bootloader_module->module_count);
    Thread *nonnull active_threads[bootloader_module->module_count];
    for (size_t i = 0; i < bootloader_module->module_count; i++) {
        struct limine_file *m = bootloader_module->modules[i];
        auto ext_ptr = extension(string_length(m->cmdline), m->cmdline);
        if (ext_ptr == nullptr) continue;
        auto ext = (const char *nonnull)ext_ptr;
        if (string_compare(string_length(ext), ext, 3, "lua") != 0) continue;
        $info("  Loading {}...", m->cmdline);
        active_threads[i] = $assert_nonnull(spawn_thread(m->address, m->size, m->cmdline), "Failed to load lua module {}!", m->cmdline);
        $success("  Done");
    }

    halt();
}

$nonnull_end
