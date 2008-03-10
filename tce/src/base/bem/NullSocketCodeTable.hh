/**
 * @file NullSocketCodeTable.hh
 *
 * Declaration of NullSocketCodeTable class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_SOCKET_CODE_TABLE_HH
#define TTA_NULL_SOCKET_CODE_TABLE_HH

#include "SocketCodeTable.hh"

/**
 * A null version of SocketCodeTable class.
 */
class NullSocketCodeTable : public SocketCodeTable {
public:
    static NullSocketCodeTable& instance();

private:
    NullSocketCodeTable();
    virtual ~NullSocketCodeTable();

    /// The only instance.
    static NullSocketCodeTable instance_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
