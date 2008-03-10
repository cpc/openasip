/**
 * @file NOPEncoding.hh
 *
 * Declaration of NOPEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NOP_ENCODING_HH
#define TTA_NOP_ENCODING_HH

#include "Encoding.hh"

class SlotField;
class ObjectState;

/**
 * Represents the encoding for NOP (no operation) in the source field.
 */
class NOPEncoding : public Encoding {
public:
    NOPEncoding(
        unsigned int encoding,
        unsigned int extraBits,
        SlotField& parent)
        throw (ObjectAlreadyExists);
    NOPEncoding(const ObjectState* state, SlotField& parent)
        throw (ObjectAlreadyExists, ObjectStateLoadingException);
    virtual ~NOPEncoding();

    SlotField* parent() const;

    virtual int bitPosition() const;
    virtual ObjectState* saveState() const;

    /// ObjectState name for NOP encoding.
    static const std::string OSNAME_NOP_ENCODING;
};

#endif
