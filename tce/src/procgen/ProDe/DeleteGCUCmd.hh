/**
 * @file DeleteGCUCmd.hh
 *
 * Declaration of DeleteGCUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_DELETE_GCU_CMD_HH
#define TTA_DELETE_GCU_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for deleting global control unit from the Machine.
 */
class DeleteGCUCmd: public ComponentCommand {
public:
    DeleteGCUCmd(EditPart* editPart);
    virtual ~DeleteGCUCmd();
    virtual bool Do();

private:
    /// Global control unit to delete.
    EditPart* editPart_;
};
#endif
