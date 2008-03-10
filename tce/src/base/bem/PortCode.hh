/**
 * @file PortCode.hh
 *
 * Declaration of PortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PORT_CODE_HH
#define TTA_PORT_CODE_HH

#include <string>
#include "Exception.hh"

class SocketCodeTable;
class ObjectState;

/**
 * Base class for FUPortCode, RFPortCode and IUPortCode.
 */
class PortCode {
public:
    virtual ~PortCode();

    std::string unitName() const;

    bool hasEncoding() const;
    unsigned int encoding() const
        throw (NotAvailable);
    unsigned int extraBits() const;
    int width() const;
    int encodingWidth() const;
    int indexWidth() const;

    SocketCodeTable* parent() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for PortCode.
    static const std::string OSNAME_PORT_CODE;
    /// ObjectState attribute key for the name of the unit.
    static const std::string OSKEY_UNIT_NAME;
    /// ObjectState attribute key for the encoding.
    static const std::string OSKEY_ENCODING;
    /// ObjectState attribute key for the number of extra bits.
    static const std::string OSKEY_EXTRA_BITS;
    /// ObjectState attribute key for the widht of register index.
    static const std::string OSKEY_INDEX_WIDTH;

protected:
    PortCode(
        const std::string& unitName,
        unsigned int encoding,
        unsigned int extraBits,
        int indexWidth)
        throw (OutOfRange);
    PortCode(
        const std::string& unitName,
        int indexWidth)
        throw (OutOfRange);
    PortCode(const ObjectState* state)
        throw (ObjectStateLoadingException);
    void setParent(SocketCodeTable* parent);

private:
    /// Name of the unit.
    std::string unitName_;
    /// The encoding.
    unsigned int encoding_;
    /// The number of extra bits in the encoding.
    unsigned int extraBits_;
    /// The width of the register index in the port code.
    int indexWidth_;
    /// Tells whether the port code has an encoding.
    bool hasEncoding_;
    /// The parent socket code table.
    SocketCodeTable* parent_;
};

#endif
