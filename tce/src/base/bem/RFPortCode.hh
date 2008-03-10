/**
 * @file RFPortCode.hh
 *
 * Declaration of RFPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_PORT_CODE_HH
#define TTA_RF_PORT_CODE_HH

#include <string>

#include "PortCode.hh"
#include "Exception.hh"

/**
 * RFPortCode class defines a binary encoding for a register file port.
 */
class RFPortCode : public PortCode {
public:
    RFPortCode(
	const std::string& regFile,
	unsigned int encoding,
	unsigned int extraBits,
	int indexWidth,
	SocketCodeTable& parent)
	throw (ObjectAlreadyExists, OutOfRange);
    RFPortCode(
	const std::string& regFile,
	int indexWidth,
	SocketCodeTable& parent)
	throw (ObjectAlreadyExists, OutOfRange);
    RFPortCode(const ObjectState* state, SocketCodeTable& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~RFPortCode();

    virtual ObjectState* saveState() const;

    /// ObjectState name for RF port code.
    static const std::string OSNAME_RF_PORT_CODE;
};

#endif
