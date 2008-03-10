/**
 * @file NullIUPortCode.hh
 *
 * Declaration of NullIUPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_IU_PORT_CODE_HH
#define TTA_NULL_IU_PORT_CODE_HH

#include "IUPortCode.hh"

class SocketCodeTable;
class BinaryEncoding;

/**
 * A null version of IUPortCode class.
 */
class NullIUPortCode : public IUPortCode {
public:
    static NullIUPortCode& instance();

private:
    NullIUPortCode();
    virtual ~NullIUPortCode();

    /// The only instance.
    static NullIUPortCode instance_;
    /// The parent socket code table.
    static SocketCodeTable socketCodes_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
