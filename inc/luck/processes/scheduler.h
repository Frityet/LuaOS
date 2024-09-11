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

#include <lua.h>

#include "common.h"
#include "lock.h"
#include "luck/arch/x86_64/cpu.h"
#include "luck/processes/ipc.h"

$nonnull_begin

typedef struct Futex {
    struct Thread *head;
    Lock lock;
} Futex;

typedef struct Thread {
    char name[64];
    CPUContext cpu_context;
    void *stack_base;
    struct Thread *nullable next_task, *nullable previous_task;
    lua_State *lua;
    bool ready, kill;
    Lock lock;

    Futex *nullable next_mutex; //?
} Thread;

enum {
    Thread_STACKSIZE = 0x4000,
    Thread_COUNT = 0x100,
};

void wait_for_thread(Thread *thread);
void wake_futex(Futex *mtx);
void wake_all_futexes(Futex *mutexes);

Thread *nullable spawn_thread(void *addr, size_t size, const char *vpi);
void reschedule(CPUContext *ctx);
void scheduler_init(void);

int luaopen_scheduler(lua_State *L);

$nonnull_end
