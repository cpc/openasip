/**
 * @file NullBridgeEncoding.hh
 *
 * Declaration of NullBridgeEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_BRIDGE_ENCODING_HH
#define TTA_NULL_BRIDGE_ENCODING_HH

#include "BridgeEncoding.hh"

class SourceField;
class BinaryEncoding;
class MoveSlot;

/**
 * A null version of SocketEncoding class.
 */
class NullBridgeEncoding : public BridgeEncoding {
public:
    static NullBridgeEncoding& instance();

private:
    NullBridgeEncoding();
    virtual ~NullBridgeEncoding();

    /// The only instance.
    static NullBridgeEncoding instance_;
    /// The parent source field.
    static SourceField sourceField_;
    /// The parent move slot.
    static MoveSlot moveSlot_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
