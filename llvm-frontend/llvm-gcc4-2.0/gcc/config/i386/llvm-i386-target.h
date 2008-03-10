/* APPLE LOCAL begin LLVM (ENTIRE FILE!)  */
/* Some target-specific hooks for gcc->llvm conversion
Copyright (C) 2007 Free Software Foundation, Inc.
Contributed by Anton Korobeynikov (asl@math.spbu.ru)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

/* LLVM specific stuff for supporting calling convention output */
#define TARGET_ADJUST_LLVM_CC(CC, type)                         \
  {                                                             \
    tree type_attributes = TYPE_ATTRIBUTES (type);              \
    if (lookup_attribute ("stdcall", type_attributes)) {        \
      CC = CallingConv::X86_StdCall;                            \
    } else if (lookup_attribute("fastcall", type_attributes)) { \
      CC = CallingConv::X86_FastCall;                           \
    }                                                           \
  }                                                             \

/* LLVM specific stuff for converting gcc's `regparm` attribute to LLVM's
   `inreg` parameter attribute */
#define LLVM_TARGET_ENABLE_REGPARM

#define LLVM_TARGET_INIT_REGPARM(local_regparm, type)           \
  {                                                             \
    tree attr;                                                  \
    local_regparm = ix86_regparm;                               \
    attr = lookup_attribute ("regparm",                         \
                              TYPE_ATTRIBUTES (type));          \
    if (attr) {                                                 \
      local_regparm = TREE_INT_CST_LOW (TREE_VALUE              \
                                        (TREE_VALUE (attr)));   \
    }                                                           \
  }

#define LLVM_ADJUST_REGPARM_ATTRIBUTE(Attribute, Size,          \
                                      local_regparm)            \
  {                                                             \
    if (!TARGET_64BIT) {                                        \
      int words = (Size + BITS_PER_WORD - 1) / BITS_PER_WORD;   \
      local_regparm -= words;                                   \
      if (local_regparm>=0) {                                   \
        Attribute |= ParamAttr::InReg;                          \
      } else                                                    \
        local_regparm = 0;                                      \
    }                                                           \
  }

/* APPLE LOCAL end LLVM (ENTIRE FILE!)  */

