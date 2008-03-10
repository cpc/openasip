/**
 * @file NullBinary.hh
 *
 * Declaration of NullBinary class.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_BINARY_HH
#define TTA_NULL_BINARY_HH

#include "Binary.hh"

namespace TPEF {

/**
 * A singleton class that represents a null binary.
 *
 * Calling any method causes the program to abort.
 */
class NullBinary : public Binary {
public:
    virtual ~NullBinary();
    static NullBinary& instance();

protected:
    NullBinary();

private:
    /// Copying not allowed.
    NullBinary(const NullBinary&);
    /// Assignment not allowed.
    NullBinary& operator=(const NullBinary&);

    /// Unique instance of NullBinary.
    static NullBinary instance_;
};

}

#endif
