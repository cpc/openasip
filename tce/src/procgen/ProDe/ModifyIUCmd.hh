/**
 * @file ModifyIUCmd.hh
 *
 * Declaration of ModifyIUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_MODIFY_IU_CMD_HH
#define TTA_MODIFY_IU_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command which displays a immediate unit dialog with precompleted
 * values for modifying a immediate unit.
 */
class ModifyIUCmd: public ComponentCommand {
public:
    ModifyIUCmd(EditPart* editPart);
    virtual ~ModifyIUCmd();
    virtual bool Do();

private:
    /// Immediate unit to modify.
    EditPart* editPart_;
};

#endif
