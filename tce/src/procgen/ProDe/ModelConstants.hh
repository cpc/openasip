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
