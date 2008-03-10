/**
 * @file ModifySocketCmd.hh
 *
 * Declaration of ModifySocketCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_MODIFY_SOCKET_CMD_HH
#define TTA_MODIFY_SOCKET_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command which displays a socket dialog with precompleted
 * values for modifying a socket.
 */
class ModifySocketCmd: public ComponentCommand {
public:
    ModifySocketCmd(EditPart* editPart);
    virtual ~ModifySocketCmd();
    virtual bool Do();

private:
    /// Socket to modify.
    EditPart* editPart_;
};

#endif
