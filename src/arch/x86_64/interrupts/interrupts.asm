;
; Copyright (C) 2021-2024 pitust
;
; This file is part of LuaOS.
;
; LuaOS is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; LuaOS is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with LuaOS.  If not, see <http://www.gnu.org/licenses/>.
;

bits 64

extern handle_interrupt

%macro idtend 1
    dq isr%1
%endmacro

%macro isrgen 1

isr%1:
%if (%1 >= 0x8 && %1 <= 0xE) || %1 == 0x11 || %1 == 0x1E
    ; i'm a expert at the morgan's tehorems or whatever theyre called
%else
    push 0
%endif
    push %1
    push rbp
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    xor rbp, rbp
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    xor rsi, rsi
    xor rdi, rdi
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15

    lea rdi, [rsp]
    call handle_interrupt
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp
    add rsp, 16
    iretq

%endmacro

section .text

global get_idt_targets
get_idt_targets:
    lea rax, [rel idt_targets]
    ret

%assign i 0
%rep 256
isrgen i
%assign i i+1
%endrep

section .rodata

global idt_targets
idt_targets:
%assign i 0
%rep 256
    idtend i
%assign i i+1
%endrep
