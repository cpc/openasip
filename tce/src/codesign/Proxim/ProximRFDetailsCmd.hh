/**
 * @file ProximRFDetailsCmd.hh
 *
 * Declaration of ProximRFDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_RF_DETAILS_CMD_HH
#define TTA_PROXIM_RF_DETAILS_CMD_HH

#include "ComponentCommand.hh"

namespace TTAMachine {
    class RegisterFile;
}

/**
 * Command for displaying register file details in the machine window
 * component details pane.
 */
class ProximRFDetailsCmd : public ComponentCommand {
public:
    ProximRFDetailsCmd(const TTAMachine::RegisterFile& rf);
    virtual ~ProximRFDetailsCmd();
    virtual bool Do();
private:
    /// Register file to display.
    const TTAMachine::RegisterFile& rf_;
};

#endif
