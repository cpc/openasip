/**
 * @file ModifyFUPortCmd.hh
 *
 * Declaration of ModifyFUPortCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MODIFY_FU_PORT_CMD_HH
#define TTA_MODIFY_FU_PORT_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command which displays a function unit port dialog with precompleted
 * values for modifying a function unit port.
 */
class ModifyFUPortCmd: public ComponentCommand {
public:
    ModifyFUPortCmd(EditPart* editPart);
    virtual ~ModifyFUPortCmd();
    virtual bool Do();

private:
     /// Function unit port to modify.
     EditPart* editPart_;
};

#endif
