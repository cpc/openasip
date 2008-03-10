/**
 * @file ModifyIUPortCmd.hh
 *
 * Declaration of ModifyIUPortCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MODIFY_IU_PORT_CMD_HH
#define TTA_MODIFY_IU_PORT_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command which displays an immediate unit port dialog with precompleted
 * values for modifying an imediate unit port.
 */
class ModifyIUPortCmd: public ComponentCommand {
public:
    ModifyIUPortCmd(EditPart* editPart);
    virtual ~ModifyIUPortCmd();
    virtual bool Do();

private:
     /// Immediate unit port to modify.
     EditPart* editPart_;
};

#endif
