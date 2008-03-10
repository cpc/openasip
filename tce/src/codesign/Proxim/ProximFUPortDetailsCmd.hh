/**
 * @file ProximFUPortDetailsCmd.hh
 *
 * Declaration of ProximFUPortDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_FU_PORT_DETAILS_CMD_HH
#define TTA_PROXIM_FU_PORT_DETAILS_CMD_HH

#include "ComponentCommand.hh"

namespace TTAMachine {
    class FUPort;
}

/**
 * Command for displaying function unit port details in the machine state
 * window component details pane.
 */
class ProximFUPortDetailsCmd : public ComponentCommand {
public:
    ProximFUPortDetailsCmd(const TTAMachine::FUPort& port);
    virtual ~ProximFUPortDetailsCmd();
    virtual bool Do();
private:
    /// Function unit port to display.
    const TTAMachine::FUPort& port_;
};

#endif
