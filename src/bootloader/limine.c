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

#include "luck/io/log.h"

#include "luck/bootloader/limine.h"


#define DECLARE_MODULE(name) \
    volatile const struct limine_##name##_response *nullable bootloader_##name

#define MODULE_REQUEST(name) \
	static volatile struct limine_##name##_request name##_request =

DECLARE_MODULE(hhdm);
DECLARE_MODULE(kernel_address);
DECLARE_MODULE(module);
DECLARE_MODULE(framebuffer);
DECLARE_MODULE(rsdp);
DECLARE_MODULE(memmap);


MODULE_REQUEST(hhdm) { LIMINE_HHDM_REQUEST, 0, nullptr };
MODULE_REQUEST(kernel_address) { LIMINE_KERNEL_ADDRESS_REQUEST, 0, nullptr };
MODULE_REQUEST(module) { LIMINE_MODULE_REQUEST, 0, nullptr };
MODULE_REQUEST(framebuffer) { LIMINE_FRAMEBUFFER_REQUEST, 0, nullptr };
MODULE_REQUEST(rsdp) { LIMINE_RSDP_REQUEST, 0, nullptr };
MODULE_REQUEST(memmap) { LIMINE_MEMMAP_REQUEST, 0, nullptr };

qword VIRTUAL_MEMORY_HIGH = 0;

uint64_t limine_virt_to_phys(uint64_t virt) {
	if (VIRTUAL_MEMORY_HIGH == 0) VIRTUAL_MEMORY_HIGH = hhdm_request.response->offset;
	if (virt >= kernel_address_request.response->virtual_base) return virt - kernel_address_request.response->virtual_base + kernel_address_request.response->physical_base;
	if (virt >= 0xffff800000000000) return virt - hhdm_request.response->offset;
	$panic("invalid call to virt_to_phys({:#x}) [invalid va]", virt);
}

uint64_t limine_phys_to_virt(uint64_t phys) {
	if (VIRTUAL_MEMORY_HIGH == 0) VIRTUAL_MEMORY_HIGH = hhdm_request.response->offset;
	if (phys >= 0xffff800000000000)  $panic("invalid call to phys_to_virt()");
	return phys + hhdm_request.response->offset;
}

struct limine_file *nullable find_module(const char *nonnull name)
{
    size_t name_len = string_length(name);
    for (size_t i = 0; i < module_request.response->module_count; i++) {
        const char *nullable modnam = get_file_name(module_request.response->modules[i]->cmdline);
        if (modnam && string_compare(string_length((const char *nonnull)modnam), (const char *nonnull)modnam, name_len, name) == 0)
            return module_request.response->modules[i];
    }
    return nullptr;
}

void bootloader_init()
{
	bootloader_hhdm 			= hhdm_request.response;
	bootloader_kernel_address 	= kernel_address_request.response;
	bootloader_module 			= module_request.response;
	bootloader_framebuffer 		= framebuffer_request.response;
	bootloader_rsdp 			= rsdp_request.response;
	bootloader_memmap 			= memmap_request.response;
}
