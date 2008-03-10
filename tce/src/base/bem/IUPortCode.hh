/**
 * @file IUPortCode.hh
 *
 * Declaration if IUPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_IU_PORT_CODE_HH
#define TTA_IU_PORT_CODE_HH

#include "PortCode.hh"

/**
 * IUPortCode defined a binary encoding for an immediate unit port.
 */
class IUPortCode : public PortCode {
public:
    IUPortCode(
	const std::string& immediateUnit,
	unsigned int encoding,
	unsigned int extraBits,
	int indexWidth,
	SocketCodeTable& parent)
	throw (ObjectAlreadyExists, OutOfRange);
    IUPortCode(
	const std::string& immediateUnit,
	int indexWidth,
	SocketCodeTable& parent)
	throw (ObjectAlreadyExists, OutOfRange);
    IUPortCode(const ObjectState* state, SocketCodeTable& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~IUPortCode();

    virtual ObjectState* saveState() const;

    /// ObjectState name for RF port code.
    static const std::string OSNAME_IU_PORT_CODE;
};

#endif
