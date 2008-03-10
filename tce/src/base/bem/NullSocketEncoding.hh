/**
 * @file NullSocketEncoding.hh
 *
 * Declaration of NullSocketEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_SOCKET_ENCODING_HH
#define TTA_NULL_SOCKET_ENCODING_HH

#include "SocketEncoding.hh"

class SourceField;
class BinaryEncoding;
class MoveSlot;

/**
 * A null version of SocketEncoding class.
 */
class NullSocketEncoding : public SocketEncoding {
public:
    static NullSocketEncoding& instance();

private:
    NullSocketEncoding();
    virtual ~NullSocketEncoding();

    /// The only instance.
    static NullSocketEncoding instance_;
    /// The parent source field.
    static SourceField sourceField_;
    /// The parent move slot.
    static MoveSlot moveSlot_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
