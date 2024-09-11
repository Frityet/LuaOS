/**
 * Copyright (C) 2021-2024 pitust
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

#include "luck/bootloader/limine.h"
#include "luck/memory/manager.h"
#include "luck/memory/magazines.h"
#include "luck/io/log.h"

static qword kalloc_size_arr[16] = {
    16, 32, 48, 64, 96, 128, 176, 224, 288, 368, 480, 624, 800, 1024, 1360, 2048
};

#define $raw_slabs(name, convert, oom) \
    static qword name##_slab_list_head = 0; \
    static qword name##_slab_list_alloc(void* ctx) { \
        (void)ctx; \
        if (name##_slab_list_head == 0) { oom } \
        qword cur = name##_slab_list_head; \
        name##_slab_list_head = *convert(name##_slab_list_head); \
        return cur; \
    } \
    static void name##_slab_list_free(void* ctx, qword pg) { \
        (void)ctx; \
        *convert(pg) = name##_slab_list_head; \
        name##_slab_list_head = pg; \
    }

#define $convert_pg(X) $virt(X, qword)
#define $convert_kalloc(X) ((qword*)(void*)(X))

static qword current_region = 0;

$raw_slabs(page, $convert_pg, {
    while (current_region < bootloader_memmap->entry_count) {
        struct limine_memmap_entry *region = bootloader_memmap->entries[current_region];
        if (region->type == LIMINE_MEMMAP_USABLE) {
            if (region->length >= 0x1000 && (region->base & 0xfff) == 0) {
                region->base += 0x1000;
                region->length -= 0x1000;
                return region->base - 0x1000;
            }
        }
        current_region++;
    }
    $panic("we ran out of fresh pages to allocate!");
});

static Magazine* page_mag = nullptr;
static Magazine* kalloc_mags[32] = {nullptr};
static qword kalloc_heads[32] = {0};
static atomic_ullong addr = 0x0000700000000000;

static qword kalloc_inner(void* ctx) {
    qword i = (qword)ctx;
    if (kalloc_heads[i] == 0) {
        qword pa = page_alloc(PageType_REGULAR);
        qword va = atomic_fetch_add(&addr, 4096);
        pmap_map(va, pa);

        qword ssiz = kalloc_size_arr[i];
        if (ssiz > 4096) {
            $panic("todo: kalloc with size > 4096 (siz={})", ssiz);
        }
        qword cnt = 4096/ssiz;
        if (cnt == 1) {
            return va;
        }
        for (qword ii = 0;ii < cnt;ii++) {
            qword slab = va + ii * ssiz;
            *(qword*)slab = kalloc_heads[i];
            kalloc_heads[i] = slab;
        }
    }
    qword cur = kalloc_heads[i];
    kalloc_heads[i] = *(qword*)kalloc_heads[i];
    return cur;
}
static void kfree_inner(void* ctx, qword pg) {
    qword i = (qword)ctx;
    *(qword*)pg = kalloc_heads[i];
    kalloc_heads[i] = pg;
}

void kalloc_init(void) {
    page_mag = mag_new(page_slab_list_alloc, page_slab_list_free, nullptr);
    qword idx_special = 0xffff;
    for (int i = 0; i < 16 ;i++) {
        if (kalloc_size_arr[i] == 368) idx_special = i;
    }
    if (idx_special == 0xffff) $panic("update 368 with whatever new kalloc size is big enough to hold a Magazine (>312 bytes)");
    kalloc_mags[idx_special] = mag_new(kalloc_inner, kfree_inner, (void*)idx_special);
    for (qword i = 0;i < 32;i++) {
        if (i != idx_special) kalloc_mags[i] = mag_new(kalloc_inner, kfree_inner, (void*)i);
    }
}

qword page_alloc(enum PageType pty) {
	(void)pty; // for now :P

    qword addr = mag_get(page_mag);
    memset($virt(addr, void), 0, 4096);
    return addr;
}

static qword find_kalloc_mag(qword size) {
    if (size > kalloc_size_arr[15]) $panic("cannot kalloc() or kfree() more than {} bytes! (attempted to kalloc/kfree {})", kalloc_size_arr[15], size);
    for (qword i = 0;i < 16;i++) {
        if (kalloc_size_arr[i] >= size) return i;
    }
    $panic("wtf");
}
void *kalloc(qword size) {
    if (size > kalloc_size_arr[15]) {
        size = (size + 0xfff) & ~0xfff;
        qword this_addr = atomic_fetch_add(&addr, size);
        for (qword i = 0;i < size;i += 4096) pmap_map(this_addr + i, page_alloc(PageType_REGULAR));
        return (void*)this_addr;
    }
    qword mag = find_kalloc_mag(size);
earlykalloc:
    if (!kalloc_mags[mag]) {
        if (kalloc_size_arr[mag] < 336) {
            mag = find_kalloc_mag(336);
            goto earlykalloc;
        }
        $panic("cannot (yet) kalloc {} [earlykalloc mode], sclass {}", size, kalloc_size_arr[mag]);
    }
    void* b = (void*)mag_get(kalloc_mags[mag]);
    memset(b, 0xcc, kalloc_size_arr[mag]);
    memset(b, 0x00, size);
    return b;
}

void kfree(void* ptr, qword size) {
    if (size > kalloc_size_arr[15]) {
        size = (size + 0xfff) & ~0xfff;
        $warning("leaking {} bytes at {}", size, ptr);
        return;
    }
    memset(ptr, 0xa5, size);
    qword mag = find_kalloc_mag(size);
    mag_put(kalloc_mags[mag], (qword)ptr);
}

