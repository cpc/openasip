/**
 * @file UnconditionalGuardEncoding.hh
 *
 * Declaration of UnconditionalGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_UNCONDITIONAL_GUARD_ENCODING_HH
#define TTA_UNCONDITIONAL_GUARD_ENCODING_HH

#include <string>

#include "GuardEncoding.hh"
#include "Exception.hh"

/**
 * The UnconditionalGuardEncoding class maps always true guard term to a
 * control code.
 *
 * Instances of this class are always registered to a guard field.
 */
class UnconditionalGuardEncoding : public GuardEncoding {
public:
    UnconditionalGuardEncoding(
        bool inverted,
        unsigned int encoding,
        GuardField& parent)
        throw (ObjectAlreadyExists);
    UnconditionalGuardEncoding(const ObjectState* state, GuardField& parent)
        throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~UnconditionalGuardEncoding();

    virtual ObjectState* saveState() const;

    /// ObjectState name for unconditional guard encoding.
    static const std::string OSNAME_UNCONDITIONAL_GUARD_ENCODING;
};

#endif
