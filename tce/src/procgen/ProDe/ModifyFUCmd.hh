/**
 * @file ModifyFUCmd.hh
 *
 * Declaration of ModifyFUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_MODIFY_FU_CMD_HH
#define TTA_MODIFY_FU_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command which displays a function unit dialog with precompleted
 * values for modifying a function unit.
 */
class ModifyFUCmd: public ComponentCommand {
public:
    ModifyFUCmd(EditPart* editPart);
    virtual ~ModifyFUCmd();
    virtual bool Do();

private:
     /// Function unit to modify.
     EditPart* editPart_;
};

#endif
