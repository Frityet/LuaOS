/**
 * Copyright (C) 2021-2024 pitulst
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

#include "luck/processes/scheduler.h"

#include <lua.h>
#include "string.h"

#include "luck/arch/x86_64/cpu.h"
#include "luck/io/log.h"
#include "luck/memory/manager.h"
#include "luck/bootloader/limine.h"

#include "lj-libc/limits.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define RFLAGS 0x0202

void *ljsup_alloc(void *ud, void *ptr, size_t osize, size_t nsize);

#define _lua_openmodule(mname, module) ({ \
    lua_pushcfunction(L, luaopen_##module); \
    lua_pushstring(L, mname); \
    lua_call(L, 1, 0); \
})
#define lua_openmodule(module) _lua_openmodule(#module, module)

int luaopen_kernel(lua_State *L);

[[noreturn]]
static void thread_entry(Thread *nonnull t)
{
    $debug("Starting thread {}", t->name);
    lua_State *L = t->lua;

    if (lua_pcall(L, 0, 0, 0) == LUA_OK) {
        lua_pop(L, lua_gettop(L));
    } else {
        $error("lua error: {}", lua_tostring(L, lua_gettop(L)));
        lua_pop(L, lua_gettop(L));
    }

    $debug("Killing thread {}", t->name);
    t->kill = true;
    while (true) $asm("HLT");
}

static Lock sched_lock = false;
static Thread *nullable ready = nullptr;
static Thread *nullable ready_tail = nullptr;
static Thread *nullable idle = nullptr;

//During spawn of a thread, we need to lock the scheduler
static Lock spawn_lock = false;

Thread *spawn_thread(void *addr, size_t size, const char *name)
{
    $debug("Creating a thread called {}", name);
    Thread *t = kalloc(sizeof(Thread));

    lua_State *L = t->lua = lua_newstate(ljsup_alloc, nullptr);
    if (L == nullptr)
        return nullptr;

    _lua_openmodule("", base);
    lua_openmodule(table);
    lua_openmodule(string);
    lua_openmodule(math);
    lua_openmodule(debug);
    lua_openmodule(bit);
    luaopen_kernel(L);

    size_t name_len = string_length(name);
    string_copy(64, t->name, name_len, name);

    sdword v = luaL_loadbuffer(L, addr, size, t->name);
    if (v != 0)
        $panic("Could not spawn thread! Reason: {}", lua_tostring(L, -1));

    t->stack_base = $assert_nonnull(kalloc(Thread_STACKSIZE), "failed to allocate stack");
    t->cpu_context.rsp = (qword)(t->stack_base + Thread_STACKSIZE);
    t->cpu_context.rip = (qword)(thread_entry);
    t->cpu_context.rdi = (qword)(t);
    t->cpu_context.rflags = RFLAGS; // all the flags that we need
    t->cpu_context.cs = 0x28;
    t->cpu_context.ss = 0x30;
    t->ready = true;

    acquire_lock(&sched_lock);
    {
        t->next_task = ready;

        if (ready) {
            ready->previous_task = t;
        } else {
            // TODO: in debug (or some kind of "safe") mode only
            if (ready_tail) $panic("i'm an expert at linked lists");
            ready_tail = t;
        }

        ready = t;
    }
    release_lock(&sched_lock);

    return t;
}

static volatile dword *lapic_id;

[[gnu::naked]]
static qword get_lapic_addr_dyn(void) {
    $asm ("movl $0x1b, %ecx");
    $asm ("rdmsr");
    $asm ("shrq $32, %rdx");
    $asm ("orq %rax, %rdx");
    $asm ("retq");
}

static Thread *nullable threads[Thread_COUNT];
static CPUContext idle_tasks[Thread_COUNT];
static bool was_threadsweeping[Thread_COUNT];
void scheduler_init(void)
{
    lapic_id = $virt(get_lapic_addr_dyn() & ~0xfff, dword);
}

[[noreturn]]
[[gnu::used]]
static void idle_task(void)
{
    $info("welcome to the luaOS idle task for whatever core i'm on. I'm just chillin' here...");
    while (true) $asm("hlt");
}
static _Atomic(bool) threadsweeper_lock;
static __attribute__((aligned(16))) uint8_t threadsweeper_stack[16384];

static void threadsweeper(Thread *nonnull t)
{
    $debug("Cleaning up thread {}", t->name);
    acquire_lock(&t->lock);
    lua_close(t->lua);
    t->cpu_context.rip = 0xdeadbeefdeadbeef;
    kfree(t->stack_base, 16384);
    kfree(t, sizeof(Thread));
    release_lock(&t->lock);
    $asm("MOVQ $0, (%%RDI)\n1: INT3\njmp 1b" :: "D"(&threadsweeper_lock));
}

void reschedule(CPUContext *nonnull ctx)
{
    if (atomic_load(&sched_lock)) return;
    acquire_lock(&sched_lock);
    qword lapic = *lapic_id >> 24;
    if (ctx->interrupt_number == 3) {
        if (!was_threadsweeping[lapic]) {
            $panic("invalid int3!");
        }
    }

    if (threads[lapic]) {
        Thread *t_old = threads[lapic];
        if (atomic_load(&t_old->lock)) return;
        acquire_lock(&t_old->lock);
        t_old->cpu_context = *ctx;
        if (t_old->ready) {
            if (ready_tail) {
                ready_tail->next_task = t_old;
                t_old->previous_task = ready_tail;
            } else {
                ready = ready_tail = t_old;
            }
        } else {
            if (idle) idle->previous_task = t_old;
            t_old->next_task = idle;
            idle = t_old;
        }
        release_lock(&threads[lapic]->lock);
    } else {
        if (!was_threadsweeping[lapic]) idle_tasks[lapic] = *ctx;
    }

    if (ready) {
        auto tnew = (Thread *nonnull)ready;

        if (atomic_load(&tnew->lock)) goto idle;
        acquire_lock(&tnew->lock);
        if (tnew->next_task) {
            tnew->next_task->previous_task = nullptr;
            ready = tnew->next_task;
            tnew->next_task = nullptr;
        } else {
            ready_tail = ready = nullptr;
        }
        threads[lapic] = tnew;
        *ctx = tnew->cpu_context;
        if (!tnew->ready) $panic("how is a non-ready task on the ready list?");
        if (tnew->kill) {
            acquire_lock(&threadsweeper_lock);
            ctx->rip = (uint64_t)(threadsweeper);
            ctx->rsp = (uint64_t)(threadsweeper_stack + 16384);
            ctx->rdi = (uint64_t)(tnew);
            ctx->rflags = 0x2;
            was_threadsweeping[lapic] = true;
            threads[lapic] = nullptr;
        } else {
            ctx->rflags = RFLAGS;
        }
        release_lock(&tnew->lock);
    } else {
    idle:
        *ctx = idle_tasks[lapic];
        threads[lapic] = nullptr;
        ctx->rflags = RFLAGS;
    }
    release_lock(&sched_lock);
}

void wait_for_thread(Thread *thread)
{
    acquire_lock(&thread->lock);

    while (!thread->kill) {
        release_lock(&thread->lock);
        reschedule(&thread->cpu_context); // This will cause the current thread to yield
        release_lock(&thread->lock);
    }

    release_lock(&thread->lock);
}

void wake_futex(Futex *mtx)
{
    acquire_lock(&mtx->lock);

    if (mtx->head) {
        Thread *to_wake = mtx->head;
        auto raw = to_wake->next_mutex;
        if (raw == nullptr) goto done;
        mtx->head = (Thread *nonnull)raw;

        acquire_lock(&to_wake->lock);\
        to_wake->next_mutex = nullptr;
        to_wake->ready = true;
        release_lock(&to_wake->lock);
    }

    done:
    release_lock(&mtx->lock);
}

void wake_all_futexes(Futex *mtx)
{
    acquire_lock(&mtx->lock);

    while (mtx->head) {
        Thread *to_wake = mtx->head;

        auto raw = to_wake->next_mutex;
        if (raw == nullptr) goto done;
        mtx->head = (Thread *nonnull)raw;

        acquire_lock(&to_wake->lock);\
        to_wake->next_mutex = nullptr;
        to_wake->ready = true;
        release_lock(&to_wake->lock);
    }

    done:
    acquire_lock(&mtx->lock);
}

// function scheduler.spawn(modulename: string): Thread?, string?
static int libscheduler_spawn(lua_State *nonnull L)
{
    size_t n = 0;
    const char *modulename = luaL_checklstring(L, 1, &n);
    char expanded[n + 5]; //`modulename`.lua
    struct limine_file *nullable f = find_module(modulename);
    if (f == nullptr) {
        string_copy(sizeof(expanded), expanded, n, modulename);
        string_concatenate(sizeof(expanded), expanded, 5, ".lua");
        f = find_module(expanded);
    }

    if (f == nullptr) {
        lua_pushnil(L);
        lua_pushfstring(L, "module '%s' not found", modulename);
        return 2;
    }

    Thread *thr = spawn_thread(f->address, f->size, modulename);

    lua_pushlightuserdata(L, thr);
    return 1;
}

static const luaL_Reg libscheduler[] = {
    { "spawn", libscheduler_spawn },
    { nullptr, nullptr }
};

int luaopen_scheduler(lua_State *nonnull L)
{
    luaL_newlib(L, libscheduler);
    return 1;
}
