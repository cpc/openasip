/*
    Copyright (c) 2002-2026 Tampere University.

    This file is part of OpenASIP.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/
/**
 * @file TCETargetDataLayout.hh
 *
 * Definition of the data layout used by TCE/OpenASIP targets.
 *
 * @author Joonas Multanen 2026
 */

#ifndef TTA_TCE_TARGET_DATALAYOUT_HH
#define TTA_TCE_TARGET_DATALAYOUT_HH

constexpr const char* DataLayoutStringBE =
    "E-p:32:32:32-i1:8:8-i8:8:32-"
    "i16:16:32-i32:32:32-i64:32:32-"
    "f16:16:16-f32:32:32-f64:32:32-v64:64:64-"
#if LLVM_MAJOR_VERSION >= 21
    "i128:128-"
#endif
    "v128:128:128-v256:256:256-v512:512:512-v1024:1024:1024-"
    "v2048:2048:2048-v4096:4096:4096-a0:0:32-n32";

constexpr const char* DataLayoutStringLE =
    "e-p:32:32:32-i1:8:8-i8:8:32-"
    "i16:16:32-i32:32:32-i64:32:32-"
    "f32:32:32-f64:32:32-v64:64:64-"
#if LLVM_MAJOR_VERSION >= 21
    "i128:128-"
#endif
    "v128:128:128-v256:256:256-v512:512:512-v1024:1024:1024-"
    "v2048:2048:2048-v4096:4096:4096-a0:0:32-n32";

constexpr const char* DataLayoutStringLE64 =
    "e-p:64:64:64-i1:8:64-i8:8:64-"
    "i16:16:64-i32:32:64-i64:64:64-"
    "f16:16:64-f32:32:64-f64:64:64-v64:64:64-"
#if LLVM_MAJOR_VERSION >= 21
    "i128:128-"
#endif
    "v128:128:128-v256:256:256-v512:512:512-v1024:1024:1024-"
    "v2048:2048:2048-v4096:4096:4096-a0:0:64-n64";

#endif
