/**
 * @file DeleteBridgeCmd.hh
 *
 * Declaration of DeleteBridgeCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 * @note reviewed Jun 23 2004 by ml, jn, jm, vpj
 */

#ifndef TTA_DELETE_BRIDGE_CMD_HH
#define TTA_DELETE_BRIDGE_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for deleting bridges from the Machine.
 */
class DeleteBridgeCmd: public ComponentCommand {
public:
    DeleteBridgeCmd(EditPart* editPart);
    virtual ~DeleteBridgeCmd();
    virtual bool Do();

private:
    /// Bridge to delete.
    EditPart* editPart_;
};
#endif
