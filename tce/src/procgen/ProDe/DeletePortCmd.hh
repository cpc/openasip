/**
 * @file DeletePortCmd.hh
 *
 * Declaration of DeletePortCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DELETE_PORT_CMD_HH
#define TTA_DELETE_PORT_CMD_HH

#include "ComponentCommand.hh"
#include "Port.hh"

class EditPart;

/**
 * Command for deleting ports from a machine.
 */
class DeletePortCmd: public ComponentCommand {
public:
    DeletePortCmd(EditPart* editPart);
    virtual ~DeletePortCmd();
    virtual bool Do();

private:
    /// Port to delete.
    EditPart* editPart_;
};
#endif
