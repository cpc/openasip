/**
 * @file ProximSocketDetailsCmd.hh
 *
 * Declaration of ProximSocketDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_SOCKET_DETAILS_CMD_HH
#define TTA_PROXIM_SOCKET_DETAILS_CMD_HH

#include "ComponentCommand.hh"

namespace TTAMachine {
    class Socket;
}

/**
 * Command for displaying socket details in the machine window component
 * details pane.
 */
class ProximSocketDetailsCmd : public ComponentCommand {
public:
    ProximSocketDetailsCmd(const TTAMachine::Socket& socket);
    virtual ~ProximSocketDetailsCmd();
    virtual bool Do();
private:
    /// Socket to display.
    const TTAMachine::Socket& socket_;
};

#endif
