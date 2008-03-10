/**
 * @file GuardEncoding.hh
 *
 * Declaration of GuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_GUARD_ENCODING_HH
#define TTA_GUARD_ENCODING_HH

#include "Exception.hh"

class GuardField;
class ObjectState;

/**
 * GuardEncoding class captures the properties shared by all types of guard
 * encoding.
 */
class GuardEncoding {
public:
    virtual ~GuardEncoding();

    GuardField* parent() const;
    bool isGuardInverted() const;
    unsigned int encoding() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for guard encoding.
    static const std::string OSNAME_GUARD_ENCODING;
    /// ObjectState attribute key for invert flag.
    static const std::string OSKEY_INVERTED;
    /// ObjectState attribute key for encoding.
    static const std::string OSKEY_ENCODING;

protected:
    GuardEncoding(bool inverted, unsigned int encoding);
    GuardEncoding(const ObjectState* state)
	throw (ObjectStateLoadingException);
    void setParent(GuardField* parent);

private:
    /// The "invert" flag.
    bool inverted_;
    /// The encoding.
    unsigned int encoding_;
    /// The parent guard field.
    GuardField* parent_;
};

#endif
