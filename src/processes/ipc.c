/**
 * Copyright (C) 2021-2024 Frityet
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

#include "luck/processes/ipc.h"
#include "common.h"
#include "memory.h"

$nonnull_begin

struct Message *nullable message_create(struct Message *nonnull *nullable head, size_t length, const byte data[static nonnull length])
{
    struct Message *message = kalloc(sizeof(struct Message) + length);
    if (message == nullptr) return nullptr;
    message->length = length;
    memory_copy(message->data, data, length);
    message->next = nullptr;
    message->previous = nullptr;
    if (head != nullptr) {
        (*head)->previous = message;
        message->next = *head;
        *head = message;
    }

    return message;
}

int channel_send(struct Channel *nonnull channel, struct Message *message)
{
    if (channel->byte_count + message->length > channel->byte_max)
        return -1;

    if (channel->message_count + 1 > channel->message_max)
        return -1;

    if (channel->write == nullptr) {
        channel->write = message;
        channel->read = message;
    } else {
        message->next = channel->write;
        channel->write->previous = message;
        channel->write = message;
    }
    channel->byte_count += message->length;
    channel->message_count += 1;
    return 0;
}

struct Message *nullable channel_receive(struct Channel *nonnull channel)
{
    if (channel->read == nullptr)
        return nullptr;

    struct Message *message = channel->read;
    channel->read = message->previous;
    if (channel->read != nullptr)
        channel->read->next = nullptr;
    else
        channel->write = nullptr;

    channel->byte_count -= message->length;
    channel->message_count -= 1;
    return message;
}

$nonnull_end
