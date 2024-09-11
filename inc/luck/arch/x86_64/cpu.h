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

#include "common.h"

#include <string.h>

typedef struct [[gnu::packed]] {
    qword r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rdx, rcx, rbx, rax, rbp;

    qword interrupt_number, error, rip, cs, rflags, rsp, ss;
} CPUContext;

$closed_enum CPUVendor {
    CPUVendor_INTEL,
    CPUVendor_AMD,
    CPUVendor_QEMU,
    CPUVendor_KVM,
    CPUVendor_VMWARE,
    CPUVendor_VIRTUALBOX,
    CPUVendor_UNKNOWN,
};

static const char CPU_VENDORS[][12] = {
    [CPUVendor_INTEL]       = "Intel",
    [CPUVendor_AMD]         = "AMD",
    [CPUVendor_QEMU]        = "QEMU",
    [CPUVendor_KVM]         = "KVM",
    [CPUVendor_VMWARE]      = "VMWare",
    [CPUVendor_VIRTUALBOX]  = "VirtualBox",
    [CPUVendor_UNKNOWN]     = "Unknown",
};

enum CPUVendor cpu_get_vendor(void);

static inline qword ticks_to_nanoseconds(qword x)
{ return (x >> 1) + (x >> 2) + (x >> 4) + (x >> 6) + (x >> 7) + (x >> 9) + (x >> 13) + (x >> 14) + (x >> 16) + (x >> 17) + (x >> 18); }
