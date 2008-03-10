/**
 * @file ModifyRFCmd.hh
 *
 * Declaration of ModifyRFCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_MODIFY_RF_CMD_HH
#define TTA_MODIFY_RF_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command which displays a register file dialog with precompleted
 * values for modifying a register file.
 */
class ModifyRFCmd: public ComponentCommand {
public:
    ModifyRFCmd(EditPart* editPart);
    virtual ~ModifyRFCmd();
    virtual bool Do();

private:
    /// Register file to modify.
    EditPart* editPart_;
};

#endif
