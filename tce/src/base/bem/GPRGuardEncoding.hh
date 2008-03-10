/**
 * @file GPRGuardEncoding.hh
 *
 * Declaration of GPRGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_GPR_GUARD_ENCODING_HH
#define TTA_GPR_GUARD_ENCODING_HH

#include <string>

#include "GuardEncoding.hh"
#include "Exception.hh"

/**
 * The GPRGuardEncoding class maps a guard expression with general purpose
 * register term to a control code.
 *
 * Instances of this class are always registered to a guard field.
 */
class GPRGuardEncoding : public GuardEncoding {
public:
    GPRGuardEncoding(
	const std::string& regFile,
	int index,
	bool inverted,
	unsigned int encoding,
	GuardField& parent)
	throw (ObjectAlreadyExists);
    GPRGuardEncoding(const ObjectState* state, GuardField& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~GPRGuardEncoding();

    std::string registerFile() const;
    int registerIndex() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for GPR guard encoding.
    static const std::string OSNAME_GPR_GUARD_ENCODING;
    /// ObjectState attribute key for the name of the register file.
    static const std::string OSKEY_RF_NAME;
    /// ObjectState attribute key for register index.
    static const std::string OSKEY_REGISTER_INDEX;

private:
    /// Name of the register file.
    std::string regFile_;
    /// Register index.
    int index_;
};

#endif
