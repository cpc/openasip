/**
 * @file ModifyRFPortCmd.hh
 *
 * Declaration of ModifyRFPortCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MODIFY_RF_PORT_CMD_HH
#define TTA_MODIFY_RF_PORT_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command which displays a register file port dialog with precompleted
 * values for modifying a register file port.
 */
class ModifyRFPortCmd: public ComponentCommand {
public:
    ModifyRFPortCmd(EditPart* editPart);
    virtual ~ModifyRFPortCmd();
    virtual bool Do();

private:
     /// Register file port to modify.
     EditPart* editPart_;
};

#endif
