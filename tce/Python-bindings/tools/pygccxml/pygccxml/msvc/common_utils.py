# Copyright (c) 2002-2009 Tampere University of Technology.
#
# This file is part of TTA-Based Codesign Environment (TCE).
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
import ctypes
import ctypes.wintypes
import config as msvc_cfg

class UNDECORATE_NAME_OPTIONS:
    UNDNAME_COMPLETE = 0x0000 #Enables full undecoration.
    UNDNAME_NO_LEADING_UNDERSCORES = 0x0001 #Removes leading underscores from Microsoft extended keywords.
    UNDNAME_NO_MS_KEYWORDS = 0x0002 #Disables expansion of Microsoft extended keywords.
    UNDNAME_NO_FUNCTION_RETURNS = 0x0004 #Disables expansion of return type for primary declaration.
    UNDNAME_NO_ALLOCATION_MODEL = 0x0008 #Disables expansion of the declaration model.
    UNDNAME_NO_ALLOCATION_LANGUAGE = 0x0010 #Disables expansion of the declaration language specifier.
    UNDNAME_RESERVED1 = 0x0020 #RESERVED.
    UNDNAME_RESERVED2 = 0x0040 #RESERVED.
    UNDNAME_NO_THISTYPE = 0x0060 #Disables all modifiers on the this type.
    UNDNAME_NO_ACCESS_SPECIFIERS = 0x0080 #Disables expansion of access specifiers for members.
    UNDNAME_NO_THROW_SIGNATURES = 0x0100 #Disables expansion of "throw-signatures" for functions and pointers to functions.
    UNDNAME_NO_MEMBER_TYPE = 0x0200 #Disables expansion of static or virtual members.
    UNDNAME_NO_RETURN_UDT_MODEL = 0x0400 #Disables expansion of the Microsoft model for UDT returns.
    UNDNAME_32_BIT_DECODE = 0x0800 #Undecorates 32-bit decorated names.
    UNDNAME_NAME_ONLY = 0x1000 #Gets only the name for primary declaration; returns just [scope::]name. Expands template params.
    UNDNAME_TYPE_ONLY = 0x2000 #Input is just a type encoding; composes an abstract declarator.
    UNDNAME_HAVE_PARAMETERS = 0x4000 #The real template parameters are available.
    UNDNAME_NO_ECSU = 0x8000 #Suppresses enum/class/struct/union.
    UNDNAME_NO_IDENT_CHAR_CHECK = 0x10000 #Suppresses check for valid identifier characters.
    UNDNAME_NO_PTR64 = 0x20000 #Does not include ptr64 in output.

    UNDNAME_SCOPES_ONLY = UNDNAME_NO_LEADING_UNDERSCORES \
                          | UNDNAME_NO_MS_KEYWORDS \
                          | UNDNAME_NO_FUNCTION_RETURNS \
                          | UNDNAME_NO_ALLOCATION_MODEL \
                          | UNDNAME_NO_ALLOCATION_LANGUAGE \
                          | UNDNAME_NO_ACCESS_SPECIFIERS \
                          | UNDNAME_NO_THROW_SIGNATURES \
                          | UNDNAME_NO_MEMBER_TYPE \
                          | UNDNAME_NO_ECSU \
                          | UNDNAME_NO_IDENT_CHAR_CHECK

#__unDName definition was taken from:
#http://www.tech-archive.net/Archive/VC/microsoft.public.vc.language/2006-02/msg00754.html
msvcrxx = ctypes.CDLL( msvc_cfg.msvcr_path, mode=ctypes.RTLD_GLOBAL)

free_type = ctypes.CFUNCTYPE( None, ctypes.c_void_p ) #free type
malloc_type = ctypes.CFUNCTYPE( ctypes.c_void_p, ctypes.c_uint ) #malloc type


__unDName = msvcrxx.__unDName
__unDName.argtypes = [ ctypes.c_char_p #undecorated name
                       , ctypes.c_char_p #decorated name
                       , ctypes.c_int #sizeof undecorated name
                       , malloc_type
                       , free_type
                       , ctypes.c_ushort #flags
                     ]
__unDName.restype = ctypes.c_char_p


def undecorate_name( name, options=None ):
    if not name:
        return ''
    if options is None:
        options = UNDECORATE_NAME_OPTIONS.UNDNAME_NO_ECSU
    buffer_size = 1024 * 32
    undecorated_name = ctypes.create_string_buffer('\0' * buffer_size) #should be enouph for any symbol
    __unDName( undecorated_name
               , name
               , buffer_size
               , malloc_type( msvcrxx.malloc )
               , free_type( msvcrxx.free )
               , options )
    return undecorated_name.value
