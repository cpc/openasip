/**
 * @file Encoding.hh
 *
 * Declaration of Encoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ENCODING_HH
#define TTA_ENCODING_HH

#include <string>
#include "Exception.hh"

class InstructionField;
class ObjectState;

/**
 * Represents an encoding of a source of destination within a move slot.
 * This is a base class for different encodings.
 */
class Encoding {
public:
    virtual ~Encoding();

    InstructionField* parent() const;
    
    unsigned int encoding() const;
    unsigned int extraBits() const;
    virtual int width() const;

    /**
     * Returns the position of the encoding within the parent field.
     */
    virtual int bitPosition() const = 0;

    virtual ObjectState* saveState() const;

    /// ObjectState name for Encoding class.
    static const std::string OSNAME_ENCODING;
    /// ObjectState attribute key for the encoding.
    static const std::string OSKEY_ENCODING;
    /// ObjectState attribute key for the number of extra bits.
    static const std::string OSKEY_EXTRA_BITS;

protected:
    Encoding(
        unsigned int encoding,
        unsigned int extraBits,
        InstructionField* parent);
    Encoding(const ObjectState* state, InstructionField* parent)
        throw (ObjectStateLoadingException);
    void setParent(InstructionField* parent);

private:
    /// The encoding.
    unsigned int encoding_;
    /// The number of extra bits.
    unsigned int extraBits_;
    /// The parent instruction field.
    InstructionField* parent_;
};

#endif
