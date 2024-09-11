/**
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

#include "common.h"
#include "lock.h"

#define _EVAL(...) __VA_ARGS__
#define EVAL(...) _EVAL(__VA_ARGS__)

#include "luck/bootloader/limine.h"
#include "luck/io/log.h"
#include "luck/memory/magazines.h"
#include "luck/memory/manager.h"

#define mag_each_size 512
#define max_static_mag 2
#define max_ready_mags 32

static Magazine static_mags[max_static_mag];
static atomic_llong num_static_mag = 0;
static MagazinePerCPU mag_percpu[max_static_mag * 256];

static qword static_mag_alloc[mag_each_size * 32];
static qword static_mag_alloc_idx = 0;

static atomic_llong contended, total_attempts, mag_size_max = 128;

static volatile dword *lapic_id;

[[gnu::naked]]
static qword get_lapic_addr_dyn(void) {
	$asm ("movl $0x1b, %ecx");
	$asm ("rdmsr");
	$asm ("shrq $32, %rdx");
	$asm ("orq %rax, %rdx");
	$asm ("retq");
}

void magazine_init(void)
{
	lapic_id = $virt(get_lapic_addr_dyn() & ~0xfff, dword);
}

#define max_lapic_id 2 // TODO: change this to the actual value once we do smp

Magazine *mag_new(qword(*get)(void *ctx), void (*put)(void *ctx, qword item), void *ctx) {
	Magazine *mag = nullptr;
	uint64_t mag_id = atomic_fetch_add(&num_static_mag, 1);
	if (mag_id < max_static_mag) {
		mag = &static_mags[mag_id];
		memset(mag, 0, sizeof(Magazine));
		mag->mag_percpu = &mag_percpu[mag_id * 256];
		memset(mag->mag_percpu, 0, sizeof(MagazinePerCPU) * 256);
		if (*lapic_id >= 256) $panic("arch_get_max_cpu_id() >= 256; luaOS supports a maximum of 256 cores!");
	} else {
		mag = kalloc(sizeof(Magazine));
		mag->mag_percpu = kalloc(sizeof(MagazinePerCPU) * max_lapic_id);
	}
	mag->get = get;
	mag->put = put;
	mag->ctx = ctx;
	return mag;
}
static qword *alloc_mag(void) {
	if (static_mag_alloc_idx >= 32) $panic("out of static magazine allocation space!");
	return &static_mag_alloc[static_mag_alloc_idx++ * mag_each_size + 1];
}
static void free_mag(const qword *mag) {
	(void)mag;
	$panic("TODO: free magazines (needs kmalloc)");
}

#define SWAP(a, b) do { __auto_type _tmp = (a); (a) = (b); (b) = _tmp; } while (0)
void mag_put(Magazine *mag, qword item) {
	MagazinePerCPU *mag_cpu = &mag->mag_percpu[*lapic_id >> 24];
	acquire_lock(&mag_cpu->locked);
free:
	if (mag_cpu->current && mag_cpu->current[-1] < mag_size_max) {
		// TODO: i really really really don't want this in the hot path for free
		if (__builtin_expect(!mag_cpu->current, 0)) mag_cpu->current = alloc_mag();
		mag_cpu->current[mag_cpu->current[-1]++] = item;
	} else if (mag_cpu->previous && mag_cpu->previous[-1] == 0) {
		SWAP(mag_cpu->previous, mag_cpu->current);
		goto free;
	} else if (atomic_load((atomic_ullong*)&mag->num_ready_mags) < max_ready_mags) {
		bool done = false;
		acquire_lock(&mag->locked);
		if (mag->num_ready_mags < max_ready_mags) {
			if (mag_cpu->current) mag->ready_mags[mag->num_ready_mags++] = mag_cpu->current;
			if (mag->num_free_mags) {
				mag_cpu->current = mag->free_mags[--mag->num_free_mags];
			} else {
				mag_cpu->current = alloc_mag();
			}
			done = true;
			release_lock(&mag->locked);
		} else {
			release_lock(&mag->locked);
			goto release;
		}
	} else {
release:;
		if (!mag_cpu->current) $panic("this is prooobably dead code");
		uint64_t mag_cur_size = mag_cpu->current[-1];
		for (uint64_t i = 0;i < mag_cur_size;i++) {
			mag->put(mag->ctx, mag_cpu->current[i]);
			mag_cpu->current[i] = 0;
		}
		mag_cpu->current[-1] = 0;
	}
	release_lock(&mag_cpu->locked);
}
bool mag_xget(Magazine *mag, qword *out, uint64_t flags) {
	MagazinePerCPU *mag_cpu = &mag->mag_percpu[*lapic_id >> 24];
	bool ok = false;
	acquire_lock(&mag_cpu->locked);
cur_mag_populated:
	if (mag_cpu->current && mag_cpu->current[-1]) {
		*out = mag_cpu->current[--mag_cpu->current[-1]];
		ok = true;
	} else if (mag_cpu->previous && mag_cpu->previous[-1]) {
		SWAP(mag_cpu->previous, mag_cpu->current);
		goto cur_mag_populated;
	} else if (atomic_load((atomic_ullong*)&mag->num_ready_mags)) {
		acquire_lock(&mag->locked);
		if (mag->num_ready_mags) {
			if (mag->num_free_mags < 16) {
				mag->free_mags[mag->num_free_mags++] = mag_cpu->current;
			} else {
				// todo: is the if needed?
				if (mag_cpu->current) {
					free_mag(mag_cpu->current);
				}
			}
			mag_cpu->current = mag->ready_mags[--mag->num_ready_mags];
			release_lock(&mag->locked);
			// current magazine has been populated
			goto cur_mag_populated;
		} else {
			release_lock(&mag->locked);
			// we need to get more resources
			goto allocate_new_obj;
		}
	} else {
allocate_new_obj:;
		if (flags & MAG_MUSTGET) {
			acquire_lock(&mag->locked);
			*out = mag->get(mag->ctx);
			release_lock(&mag->locked);
			ok = true;
		}
	}
	release_lock(&mag_cpu->locked);
	return ok;
}
qword mag_get(Magazine *mag) {
	qword res = 0;
	mag_xget(mag, &res, MAG_MUSTGET);
	return res;
}
void mag_100ms_adjust(void) {
	qword contention = atomic_exchange(&contended, 0);
	qword tattempts = atomic_exchange(&total_attempts, 0);
	sqword delta = 0;
	if (contention * 30 < tattempts) delta = -1;
	if (contention * 10 >= tattempts) delta = 1;
	if (delta + mag_size_max >= mag_each_size - 1 && delta == 1) return;
	if (delta + mag_size_max < 16u && delta == -1) return;
	atomic_fetch_add(&mag_size_max, delta);
}
