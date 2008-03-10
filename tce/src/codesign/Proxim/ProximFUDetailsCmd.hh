/**
 * @file ProximFUDetailsCmd.hh
 *
 * Declaration of ProximFUDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_FU_DETAILS_CMD_HH
#define TTA_PROXIM_FU_DETAILS_CMD_HH

#include "ComponentCommand.hh"

namespace TTAMachine {
    class FunctionUnit;
}

/**
 * Command for displaying function unit details in the machine state window
 * component details pane.
 */
class ProximFUDetailsCmd : public ComponentCommand {
public:
    ProximFUDetailsCmd(const TTAMachine::FunctionUnit& fu);
    virtual ~ProximFUDetailsCmd();
    virtual bool Do();
private:
    /// Function unit to display.
    const TTAMachine::FunctionUnit& fu_;
};

#endif
