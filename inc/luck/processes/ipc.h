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

#include "common.h"
#include "lock.h"

#include "luck/memory/manager.h"

$nonnull_begin

struct Message {
    struct Message *nullable next, *nullable previous;
    size_t length;
    byte data[];
};

struct Channel {
    struct Message *nullable read, *nullable write;
    size_t byte_count, byte_max;
    size_t message_count, message_max;
    Lock    lock;
};

static inline void channel_destroy(struct Channel *nonnull channel)
{
    struct Message *nullable msg = channel->read;
    while (msg != nullptr) {
        struct Message *next = msg->next;
        kfree((struct Message *)msg, sizeof(struct Message) + msg->length);
        msg = next;
    }
}

struct Message *nullable message_create(struct Message *nonnull *nullable head, size_t length, const byte data[static nonnull length]);

int channel_send(struct Channel *nonnull channel, struct Message *message);
struct Message *nullable channel_receive(struct Channel *nonnull channel);

$nonnull_end
