/**
 * @file ImmediateEncoding.hh
 *
 * Declaration of ImmediateEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMMEDIATE_ENCODING_HH
#define TTA_IMMEDIATE_ENCODING_HH

#include "Encoding.hh"

class SourceField;
class ObjectState;

/**
 * Represents the encoding for inline immediate within the source field.
 */
class ImmediateEncoding : public Encoding {
public:
    ImmediateEncoding(
        unsigned int encoding,
        unsigned int extraBits,
        int immediateWidth,
        SourceField& parent)
        throw (OutOfRange, ObjectAlreadyExists);
    ImmediateEncoding(const ObjectState* state, SourceField& parent)
        throw (ObjectAlreadyExists, ObjectStateLoadingException);
    virtual ~ImmediateEncoding();

    SourceField* parent() const;
    int immediateWidth() const;
    int encodingWidth() const;
    int encodingPosition() const;
    int immediatePosition() const;

    virtual int bitPosition() const;
    virtual int width() const;
    virtual ObjectState* saveState() const;

    /// ObjectState name for immediate encoding.
    static const std::string OSNAME_IMM_ENCODING;
    /// ObjectState attribute key for the immediate width.
    static const std::string OSKEY_IMM_WIDTH;

private:
    int immediateWidth_;
};

#endif
