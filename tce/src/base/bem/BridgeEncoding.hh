/**
 * @file BridgeEncoding.hh
 *
 * Declaration of BridgeEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BRIDGE_ENCODING_HH
#define TTA_BRIDGE_ENCODING_HH

#include <string>

#include "Exception.hh"
#include "Encoding.hh"

class SourceField;
class ObjectState;

/**
 * Class BridgeEncoding maps a bridge to a binary encoding.
 */
class BridgeEncoding : public Encoding {
public:
    BridgeEncoding(
	const std::string& name,
	unsigned int encoding,
	unsigned int extraBits,
	SourceField& parent)
	throw (ObjectAlreadyExists);
    BridgeEncoding(const ObjectState* state, SourceField& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~BridgeEncoding();

    SourceField* parent() const;
    std::string bridgeName() const;
    virtual int bitPosition() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for bridge encoding.
    static const std::string OSNAME_BRIDGE_ENCODING;
    /// ObjectState attribute key for the name of the bridge.
    static const std::string OSKEY_BRIDGE_NAME;

private:
    /// Name of the bridge.
    std::string bridge_;
};

#endif
