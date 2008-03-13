/**
 * @file BaseType.hh
 * @author Pekka J‰‰skel‰inen (pjaaskel@cs.tut.fi) 2003
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

#include <config.h>

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

#if SIZEOF_CHAR == 1

/**
 * Machine-independent definition of byte type.
 */
typedef unsigned char Byte;

#else
#error Host machine size of char not 1 byte as expected.
#endif

/**
 * WORDS_BIGENDIAN is 1 when host machine lays out the most significant part
 * of a word at the lowest memory address.
 */
#ifndef WORDS_BIGENDIAN
#define WORDS_BIGENDIAN 0
#endif

/**
 * TARGET_WORDS_BIGENDIAN is 1 when the target TTA processor lays out the
 * most significant part of a word at the lowest memory address.
 */
#ifndef TARGET_WORDS_BIGENDIAN
#define TARGET_WORDS_BIGENDIAN 0
#endif

/**
 * 32bit unsigned integer (alias for Word).
 */
typedef Word UInt32;

const Byte BYTE_BITWIDTH = 8*sizeof(Byte);
const Byte HALFWORD_BITWIDTH = 8*sizeof(HalfWord);
const Byte WORD_BITWIDTH = 8*sizeof(Word);

/**
 * Maximum sized unsigned integer used in simulation.
 */
typedef Word UIntWord;

/**
 * Maximum sized signed integer used in simulation.
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


#endif

