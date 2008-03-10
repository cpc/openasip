/**
 * @file HDBTypes.hh
 *
 * Declarations of data types used in HDB.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_TYPES_HH
#define TTA_HDB_TYPES_HH

namespace HDB {
    /// Direction of port.
    enum Direction {
        IN, ///< Input port.
        OUT, ///< Output port.
        BIDIR ///< Bidirectional port.
    };
}

#endif
