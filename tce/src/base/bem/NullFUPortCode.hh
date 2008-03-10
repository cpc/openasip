/**
 * @file NullFUPortCode.hh
 *
 * Declaration of NullFUPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_FU_PORT_CODE_HH
#define TTA_NULL_FU_PORT_CODE_HH

#include "FUPortCode.hh"

class SocketCodeTable;
class BinaryEncoding;

/**
 * A null version of FUPortCode class.
 */
class NullFUPortCode : public FUPortCode {
public:
    static NullFUPortCode& instance();

private:
    NullFUPortCode();
    virtual ~NullFUPortCode();

    /// The only instance.
    static NullFUPortCode instance_;
    /// The parent socket code table.
    static SocketCodeTable socketCodes_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
