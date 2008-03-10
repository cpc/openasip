/**
 * @file ModifyBridgeCmd.hh
 *
 * Declaration of ModifyBridgeCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 * @note reviewed Jun 23 2004 by ml, jn, jm, vpj
 */

#ifndef TTA_MODIFY_BRIDGE_CMD_HH
#define TTA_MODIFY_BRIDGE_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command which displays a bridge dialog with precompleted
 * values for modifying a bridge.
 */
class ModifyBridgeCmd: public ComponentCommand {
public:
    ModifyBridgeCmd(EditPart* editPart);
    virtual ~ModifyBridgeCmd();
    virtual bool Do();

private:
    /// Bridge to modify.
    EditPart* editPart_;
};

#endif
