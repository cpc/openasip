/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ModelConstants.hh
 *
 * Declaration of ModelConstants class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
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
