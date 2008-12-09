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
 * @file ModelConstants.hh
 *
 * Declaration of ModelConstants class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 */

#ifndef TTA_MODEL_CONSTANTS_HH
#define TTA_MODEL_CONSTANTS_HH

#include <string>

/**
 * Constants for component properties.
 */
class ModelConstants {
public:

    // default values for component properties

    /// Default bit width.
    static const int DEFAULT_WIDTH;
    /// Default register file type.
    static const int DEFAULT_RF_SIZE;
    /// Default maximum read ports of register file.
    static const int DEFAULT_RF_MAX_READS;
    /// Default maximum read ports while writing for a register file.
    static const int DEFAULT_RF_MAX_WRITES;
    /// Default local RF guard latency.
    static const int DEFAULT_RF_GUARD_LATENCY;
    /// Default immediate unit size.
    static const int DEFAULT_IU_SIZE;
    /// Default immediate unit cycles.
    static const int DEFAULT_IU_CYCLES;
    /// Default maximum read ports of a immediate unit.
    static const int DEFAULT_IU_MAX_READS;
    /// Default maximum read ports while writing for a immediate unit.
    static const int DEFAULT_IU_MAX_WRITES;
    /// Default local IU guard latency.
    static const int DEFAULT_IU_GUARD_LATENCY;
    /// Default address space width.
    static const int DEFAULT_AS_MAU_WIDTH;
    /// Default address space min address
    static const int DEFAULT_AS_MIN_ADDRESS;
    /// Default address space max address
    static const int DEFAULT_AS_MAX_ADDRESS;
    /// Default address space interleaving factor
    static const int DEFAULT_AS_INTERLEAVING_FACTOR;

};

#endif
