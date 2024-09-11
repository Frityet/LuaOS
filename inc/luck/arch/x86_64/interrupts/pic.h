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

$nonnull_begin

//#define MASTER_PIC      (0x20)
//#define SLAVE_PIC       (0xA0)
//#define MASTER_PIC_CMD  (MASTER_PIC)
//#define MASTER_PIC_DATA (MASTER_PIC + 1)
//#define SLAVE_PIC_CMD   (SLAVE_PIC)
//#define SLAVE_PIC_DATA  (SLAVE_PIC + 1)
//#define PIC_END_OF_INT  (0x20)
$closed_enum PIC {
    PIC_MASTER      = 0x20,
    PIC_SLAVE       = 0xA0,
    PIC_MASTER_CMD  = PIC_MASTER,
    PIC_MASTER_DATA = PIC_MASTER + 1,
    PIC_SLAVE_CMD   = PIC_SLAVE,
    PIC_SLAVE_DATA  = PIC_SLAVE + 1,
    PIC_END_OF_INT  = 0x20,
};

$closed_enum ICW1 {
    ICW1_ICW4 = 0x1,
    ICW1_SINGLE = 0x2,
    ICW1_INTERVAL4 = 0x4,
    ICW1_LEVEL = 0x8,
    ICW1_INIT = 0x10,
};

$closed_enum ICW4 {
    ICW4_8086 = 0x1,
    ICW4_AUTO = 0x2,
    ICW4_BUF_SLAVE = 0x8,
    ICW4_BUF_MASTER = 0xC,
    ICW4_SFNM = 0x10,
};

void pic_init(int master_offset, int slave_offset);
void pic_send_end_of_interrupt(byte irq);

$nonnull_end
