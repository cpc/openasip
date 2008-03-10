/**
 * @file FUPortCode.hh
 *
 * Declaration of FUPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_PORT_CODE_HH
#define TTA_FU_PORT_CODE_HH

#include <string>

#include "PortCode.hh"
#include "Exception.hh"

class SocketCodeTable;

/**
 * Class FUPortCode represents the control code that identifies an FU port
 * or, if the port carries an opcode, the combination of port and operation.
 */
class FUPortCode : public PortCode {
public:
    FUPortCode(
	const std::string& fu,
	const std::string& port,
	unsigned int encoding,
	unsigned int extraBits,
	SocketCodeTable& parent)
	throw (ObjectAlreadyExists, OutOfRange);
    FUPortCode(
	const std::string& fu,
	const std::string& port,
	const std::string& operation,
	unsigned int encoding,
	unsigned int extraBits,
	SocketCodeTable& parent)
	throw (ObjectAlreadyExists, OutOfRange);
    FUPortCode(const ObjectState* state, SocketCodeTable& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~FUPortCode();

    std::string portName() const;
    std::string operationName() const
	throw (InstanceNotFound);
    bool hasOperation() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for FU port code.
    static const std::string OSNAME_FU_PORT_CODE;
    /// ObjectState attribute key for the name of the port.
    static const std::string OSKEY_PORT_NAME;
    /// ObjectState attribute key for the name of the operation.
    static const std::string OSKEY_OPERATION_NAME;

private:
    /// Name of the port.
    std::string port_;
    /// Name of the operation.
    std::string opName_;
};

#endif
