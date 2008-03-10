/**
 * @file ProximIUDetailsCmd.hh
 *
 * Declaration of ProximIUDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_IU_DETAILS_CMD_HH
#define TTA_PROXIM_IU_DETAILS_CMD_HH

#include "ComponentCommand.hh"

namespace TTAMachine {
    class ImmediateUnit;
}

/**
 * Command for displaying immediate unit details in the machine window
 * component details pane.
 */
class ProximIUDetailsCmd : public ComponentCommand {
public:
    ProximIUDetailsCmd(const TTAMachine::ImmediateUnit& iu);
    virtual ~ProximIUDetailsCmd();
    virtual bool Do();
private:
    /// Immediate unit to display.
    const TTAMachine::ImmediateUnit& iu_;
};

#endif
