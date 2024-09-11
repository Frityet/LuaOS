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

#pragma once

#define _stringify(...) #__VA_ARGS__
#define $stringify(...) _stringify(__VA_ARGS__)

#define _concat(x, y) x##y
#define $concat(x, y) _concat(x, y)

#define $foreach(F, a, ...) __VA_OPT__(_foreach_ladder0(F, a, __VA_ARGS__))
#define _foreach_ladder0(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder1(F, a, __VA_ARGS__))
#define _foreach_ladder1(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder2(F, a, __VA_ARGS__))
#define _foreach_ladder2(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder3(F, a, __VA_ARGS__))
#define _foreach_ladder3(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder4(F, a, __VA_ARGS__))
#define _foreach_ladder4(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder5(F, a, __VA_ARGS__))
#define _foreach_ladder5(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder6(F, a, __VA_ARGS__))
#define _foreach_ladder6(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder7(F, a, __VA_ARGS__))
#define _foreach_ladder7(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder8(F, a, __VA_ARGS__))
#define _foreach_ladder8(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder9(F, a, __VA_ARGS__))
#define _foreach_ladder9(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder10(F, a, __VA_ARGS__))
#define _foreach_ladder10(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder11(F, a, __VA_ARGS__))
#define _foreach_ladder11(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder12(F, a, __VA_ARGS__))
#define _foreach_ladder12(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder13(F, a, __VA_ARGS__))
#define _foreach_ladder13(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder14(F, a, __VA_ARGS__))
#define _foreach_ladder14(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder15(F, a, __VA_ARGS__))
#define _foreach_ladder15(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder16(F, a, __VA_ARGS__))
#define _foreach_ladder16(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder17(F, a, __VA_ARGS__))
#define _foreach_ladder17(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder18(F, a, __VA_ARGS__))
#define _foreach_ladder18(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder19(F, a, __VA_ARGS__))
#define _foreach_ladder19(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder20(F, a, __VA_ARGS__))
#define _foreach_ladder20(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder21(F, a, __VA_ARGS__))
#define _foreach_ladder21(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder22(F, a, __VA_ARGS__))
#define _foreach_ladder22(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder23(F, a, __VA_ARGS__))
#define _foreach_ladder23(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder24(F, a, __VA_ARGS__))
#define _foreach_ladder24(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder25(F, a, __VA_ARGS__))
#define _foreach_ladder25(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder26(F, a, __VA_ARGS__))
#define _foreach_ladder26(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder27(F, a, __VA_ARGS__))
#define _foreach_ladder27(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder28(F, a, __VA_ARGS__))
#define _foreach_ladder28(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder29(F, a, __VA_ARGS__))
#define _foreach_ladder29(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder30(F, a, __VA_ARGS__))
#define _foreach_ladder30(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder31(F, a, __VA_ARGS__))
#define _foreach_ladder31(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder32(F, a, __VA_ARGS__))
#define _foreach_ladder32(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder33(F, a, __VA_ARGS__))
#define _foreach_ladder33(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder34(F, a, __VA_ARGS__))
#define _foreach_ladder34(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder35(F, a, __VA_ARGS__))
#define _foreach_ladder35(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder36(F, a, __VA_ARGS__))
#define _foreach_ladder36(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder37(F, a, __VA_ARGS__))
#define _foreach_ladder37(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder38(F, a, __VA_ARGS__))
#define _foreach_ladder38(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder39(F, a, __VA_ARGS__))
#define _foreach_ladder39(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder40(F, a, __VA_ARGS__))
#define _foreach_ladder40(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder41(F, a, __VA_ARGS__))
#define _foreach_ladder41(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder42(F, a, __VA_ARGS__))
#define _foreach_ladder42(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder43(F, a, __VA_ARGS__))
#define _foreach_ladder43(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder44(F, a, __VA_ARGS__))
#define _foreach_ladder44(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder45(F, a, __VA_ARGS__))
#define _foreach_ladder45(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder46(F, a, __VA_ARGS__))
#define _foreach_ladder46(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder47(F, a, __VA_ARGS__))
#define _foreach_ladder47(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder48(F, a, __VA_ARGS__))
#define _foreach_ladder48(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder49(F, a, __VA_ARGS__))
#define _foreach_ladder49(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder50(F, a, __VA_ARGS__))
#define _foreach_ladder50(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder51(F, a, __VA_ARGS__))
#define _foreach_ladder51(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder52(F, a, __VA_ARGS__))
#define _foreach_ladder52(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder53(F, a, __VA_ARGS__))
#define _foreach_ladder53(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder54(F, a, __VA_ARGS__))
#define _foreach_ladder54(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder55(F, a, __VA_ARGS__))
#define _foreach_ladder55(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder56(F, a, __VA_ARGS__))
#define _foreach_ladder56(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder57(F, a, __VA_ARGS__))
#define _foreach_ladder57(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder58(F, a, __VA_ARGS__))
#define _foreach_ladder58(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder59(F, a, __VA_ARGS__))
#define _foreach_ladder59(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder60(F, a, __VA_ARGS__))
#define _foreach_ladder60(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder61(F, a, __VA_ARGS__))
#define _foreach_ladder61(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder62(F, a, __VA_ARGS__))
#define _foreach_ladder62(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder63(F, a, __VA_ARGS__))
#define _foreach_ladder63(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder64(F, a, __VA_ARGS__))
#define _foreach_ladder64(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder65(F, a, __VA_ARGS__))
#define _foreach_ladder65(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder66(F, a, __VA_ARGS__))
#define _foreach_ladder66(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder67(F, a, __VA_ARGS__))
#define _foreach_ladder67(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder68(F, a, __VA_ARGS__))
#define _foreach_ladder68(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder69(F, a, __VA_ARGS__))
#define _foreach_ladder69(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder70(F, a, __VA_ARGS__))
#define _foreach_ladder70(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder71(F, a, __VA_ARGS__))
#define _foreach_ladder71(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder72(F, a, __VA_ARGS__))
#define _foreach_ladder72(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder73(F, a, __VA_ARGS__))
#define _foreach_ladder73(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder74(F, a, __VA_ARGS__))
#define _foreach_ladder74(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder75(F, a, __VA_ARGS__))
#define _foreach_ladder75(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder76(F, a, __VA_ARGS__))
#define _foreach_ladder76(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder77(F, a, __VA_ARGS__))
#define _foreach_ladder77(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder78(F, a, __VA_ARGS__))
#define _foreach_ladder78(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder79(F, a, __VA_ARGS__))
#define _foreach_ladder79(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder80(F, a, __VA_ARGS__))
#define _foreach_ladder80(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder81(F, a, __VA_ARGS__))
#define _foreach_ladder81(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder82(F, a, __VA_ARGS__))
#define _foreach_ladder82(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder83(F, a, __VA_ARGS__))
#define _foreach_ladder83(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder84(F, a, __VA_ARGS__))
#define _foreach_ladder84(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder85(F, a, __VA_ARGS__))
#define _foreach_ladder85(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder86(F, a, __VA_ARGS__))
#define _foreach_ladder86(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder87(F, a, __VA_ARGS__))
#define _foreach_ladder87(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder88(F, a, __VA_ARGS__))
#define _foreach_ladder88(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder89(F, a, __VA_ARGS__))
#define _foreach_ladder89(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder90(F, a, __VA_ARGS__))
#define _foreach_ladder90(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder91(F, a, __VA_ARGS__))
#define _foreach_ladder91(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder92(F, a, __VA_ARGS__))
#define _foreach_ladder92(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder93(F, a, __VA_ARGS__))
#define _foreach_ladder93(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder94(F, a, __VA_ARGS__))
#define _foreach_ladder94(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder95(F, a, __VA_ARGS__))
#define _foreach_ladder95(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder96(F, a, __VA_ARGS__))
#define _foreach_ladder96(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder97(F, a, __VA_ARGS__))
#define _foreach_ladder97(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder98(F, a, __VA_ARGS__))
#define _foreach_ladder98(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder99(F, a, __VA_ARGS__))
#define _foreach_ladder99(F, a, n, ...) F(a, n) __VA_OPT__(_foreach_ladder100(F, a, __VA_ARGS__))
#define _foreach_ladder100(F, a, n, ...) F(a, n)
