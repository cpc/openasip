/**
 * @file FUGuardEncoding.hh
 *
 * Declaration of FUGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_GUARD_ENCODING_HH
#define TTA_FU_GUARD_ENCODING_HH

#include <string>

#include "GuardEncoding.hh"
#include "Exception.hh"

/**
 * The FUGuardEncoding class maps a guard expression with FU output port term
 * to a control code.
 *
 * Instances of this class are always registered to a guard field.
 */
class FUGuardEncoding : public GuardEncoding {
public:
    FUGuardEncoding(
	const std::string& fu,
	const std::string& port,
	bool inverted,
	unsigned int encoding,
	GuardField& parent)
	throw (ObjectAlreadyExists);
    FUGuardEncoding(const ObjectState* state, GuardField& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~FUGuardEncoding();

    std::string functionUnit() const;
    std::string port() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for FU guard encoding.
    static const std::string OSNAME_FU_GUARD_ENCODING;
    /// ObjectState attribute key for the name of the function unit.
    static const std::string OSKEY_FU_NAME;
    /// ObjectState attribute key for the name of the port.
    static const std::string OSKEY_PORT_NAME;

private:
    /// Name of the function unit.
    std::string functionUnit_;
    /// Name of the port.
    std::string port_;
};

#endif
