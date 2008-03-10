/**
 * @file ProximBusDetailsCmd.hh
 *
 * Declaration of ProximBusDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_BUS_DETAILS_CMD_HH
#define TTA_PROXIM_BUS_DETAILS_CMD_HH

#include "ComponentCommand.hh"

namespace TTAMachine {
    class Bus;
}

/**
 * Command for displaying bus details in the machine window component details
 * pane.
 */
class ProximBusDetailsCmd : public ComponentCommand {
public:
    ProximBusDetailsCmd(const TTAMachine::Bus& bus);
    virtual ~ProximBusDetailsCmd();
    virtual bool Do();
private:
    /// Bus to display.
    const TTAMachine::Bus& bus_;
};

#endif
