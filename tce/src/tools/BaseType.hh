/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

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
 * @file BaseType.hh
 * @author Pekka J‰‰skel‰inen (pjaaskel-no.spam-cs.tut.fi) 2003
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 *
 * This file contains definitions of base data types such as Word, HalfWord,
 * Byte. These types have fixed, machine-independent bit width. If no
 * base type of the host machine that matches the required bit width
 * is found, compilation fails. In this case, it is necessary to add
 * new cases to the type definitions for the host machine at hand.
 *
 * If the host machine does not support IEC 559 (formerly, IEEE-754)
 * floating point types as native built-in types, then the floating point
 * type definitions should be replaced by abstract data types with all the
 * operators applicable to floating-point types appropriately overloaded.
 */

#ifndef TTA_BASETYPE_HH
#define TTA_BASETYPE_HH

#include "tce_config_public.h"

#define SIZEOF_LONG 8

#if SIZEOF_LONG == 8
// proper 64-bit unix system.
typedef unsigned long LongWord;
typedef long SignedLongWord;
typedef unsigned long ULongWord;
typedef long SLongWord;

#else
// 32-bit host or win64 crap.
typedef unsigned long long LongWord;
typedef long long SignedLongWord;
typedef unsigned long long ULongWord;
typedef long long SLongWord;

#endif


#if SIZEOF_FLOAT == 4

/**
 * Machine-independent definition of 4-byte single precision IEC 559
 * floating point type.
 */
typedef float Float;

#else
#error Host machine size of float not 4 bytes as expected.
#endif

#if SIZEOF_DOUBLE == 8

/**
 * Machine-independent definition of 8-byte double precision IEC 559
 * floating point type.
 */
typedef double Double;

#else
#error Host machine size of double not 8 bytes as expected.
#endif

#if SIZEOF_INT == 4

/**
 * Machine-independent definition of 4-byte unsigned word type.
 */
typedef unsigned int Word;

/**
 * Machine-independent definition of 4-byte signed word type.
 */
typedef int SignedWord;


#else
#error Host machine size of int not 4 bytes as expected.
#endif

#if SIZEOF_SHORT == 2

/**
 * Machine-independent definition of 2-byte half-word type.
 */
typedef unsigned short int HalfWord;

#else
#error Host machine size of short int not 2 bytes as expected.
#endif

/**
 * Machine-independent definition of byte type.
 */
typedef unsigned char Byte;

/**
 * HOST_WORDS_BIGENDIAN is 1 when the host machine lays out the most 
 * significant byte of the word at the lowest memory address.
 */
#ifndef WORDS_BIGENDIAN
#define HOST_BIGENDIAN 0
#define WORDS_BIGENDIAN 0
#else
#ifndef HOST_BIGENDIAN
#define HOST_BIGENDIAN 1
#endif
#endif

/**
 * 32bit unsigned integer (alias for Word).
 */
typedef Word UInt32;

const Byte BYTE_BITWIDTH = 8*sizeof(Byte);
const Byte HALFWORD_BITWIDTH = 8*sizeof(HalfWord);
const Byte WORD_BITWIDTH = 8*sizeof(Word);
const Byte LONGWORD_BITWIDTH = 8*sizeof(LongWord);

/**
 * Maximum sized unsigned integer used in simulation in 32-bit mode.
 */
typedef Word UIntWord;

/**
 * Maximum sized signed integer used in simulation in 32-bit mode.
 */
typedef SignedWord SIntWord;

/**
 * Maximum size of integer in simulator.
 */
const Byte INT_WORD_SIZE = 8 * sizeof(UIntWord);

/**
 * A single-precision, IEC 559 floating-point number used in
 * simulation.
 */
typedef float FloatWord;

/**
 * A double-precision, IEC 559 floating-point number used in
 * simulation.
 */
typedef double DoubleWord;

const Byte FLT_WORD_SIZE = 8 * sizeof(FloatWord);
const Byte DBL_WORD_SIZE = 8 * sizeof(DoubleWord);

/*
 * Instruction addresses are represented as regular unsigned 32 bit
 * integers.
 */
typedef UInt32 InstructionAddress;

/// Type for storing addresses to memory image.
typedef UInt32 AddressImage;

/// Type for storing a MAU (must be unsigned type!). This limits
/// the maximum size of the simulated minimum addressable unit in
/// the target's data memory.
typedef Word MinimumAddressableUnit;

/// Type for storing simulation cycle counts.
typedef long long CycleCount;

#endif

