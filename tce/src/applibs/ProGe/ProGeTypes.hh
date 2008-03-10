/**
 * @file ProGeTypes.hh
 *
 * Declaration of the data types used in ProGe.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGE_TYPES_HH
#define TTA_PROGE_TYPES_HH

namespace ProGe {

/// HDLs supported by ProGe.
enum HDL {
    VHDL ///< VHDL
};

/// Data types of hardware ports.
enum DataType {
    BIT, ///< One bit.
    BIT_VECTOR ///< Several bits.
};
}

#endif
