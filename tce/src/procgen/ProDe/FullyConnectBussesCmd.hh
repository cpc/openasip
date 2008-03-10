/**
 * @file FullyConnectBussesCmd.hh
 *
 * Declaration of FullyConnectBussesCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FULLY_CONNECT_BUSSES_CMD_HH
#define TTA_FULLY_CONNECT_BUSSES_CMD_HH

#include "EditorCommand.hh"
#include "Socket.hh"

namespace TTAMachine {
    class FUPort;
    class RFPort;
    class Unit;
    class SpecialRegisterPort;
}

/**
 * Command for connecting all sockets to all busses.
 */
class FullyConnectBussesCmd : public EditorCommand {
public:
    FullyConnectBussesCmd();
    virtual ~FullyConnectBussesCmd();
    virtual bool Do();
    virtual int id() const;
    virtual FullyConnectBussesCmd* create() const;
    virtual bool isEnabled();
    virtual std::string shortName() const;
};

#endif
