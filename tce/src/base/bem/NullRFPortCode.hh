/**
 * @file NullRFPortCode.hh
 *
 * Declaration of NullRFPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_RF_PORT_CODE_HH
#define TTA_NULL_RF_PORT_CODE_HH

#include "RFPortCode.hh"

class SocketCodeTable;
class BinaryEncoding;

/**
 * A null version of RFPortCode class.
 */
class NullRFPortCode : public RFPortCode {
public:
    static NullRFPortCode& instance();

private:
    NullRFPortCode();
    virtual ~NullRFPortCode();

    /// The only instance.
    static NullRFPortCode instance_;
    /// The parent socket code table.
    static SocketCodeTable socketCodes_;
    /// The parent binary encoding.
    static BinaryEncoding bem_;
};

#endif
