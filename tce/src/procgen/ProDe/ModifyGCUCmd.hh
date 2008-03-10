/**
 * @file ModifyGCUCmd.hh
 *
 * Declaration of ModifyGCUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_MODIFY_GCU_CMD_HH
#define TTA_MODIFY_GCU_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command which displays a global control unit dialog with precompleted
 * values for modifying a global control unit.
 */
class ModifyGCUCmd: public ComponentCommand {
public:
    ModifyGCUCmd(EditPart* editPart);
    virtual ~ModifyGCUCmd();
    virtual bool Do();

private:
    /// Global control unit to modify.
    EditPart* editPart_;
};

#endif
