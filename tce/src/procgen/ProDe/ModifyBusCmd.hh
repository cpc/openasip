/**
 * @file ModifyBusCmd.hh
 *
 * Declaration of ModifyBusCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_MODIFY_BUS_CMD_HH
#define TTA_MODIFY_BUS_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command which displays a bus dialog with precompleted
 * values for modifying a bus.
 */
class ModifyBusCmd: public ComponentCommand {
public:
    ModifyBusCmd(EditPart* editPart);
    virtual ~ModifyBusCmd();
    virtual bool Do();

private:
     /// Bus to modify.
     EditPart* editPart_;
};

#endif
