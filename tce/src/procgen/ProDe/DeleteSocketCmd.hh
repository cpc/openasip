/**
 * @file DeleteSocketCmd.hh
 *
 * Declaration of DeleteSocketCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_DELETE_SOCKET_CMD_HH
#define TTA_DELETE_SOCKET_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for deleting sockets from the Machine.
 */
class DeleteSocketCmd: public ComponentCommand {
public:
    DeleteSocketCmd(EditPart* editPart);
    virtual ~DeleteSocketCmd();
    virtual bool Do();

private:
    /// Socket to delete.
    EditPart* editPart_;
};
#endif
