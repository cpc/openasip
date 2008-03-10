/**
 * @file DeleteFUCmd.hh
 *
 * Declaration of DeleteFUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_DELETE_FU_CMD_HH
#define TTA_DELETE_FU_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for deleting function units from the Machine.
 */
class DeleteFUCmd: public ComponentCommand {
public:
    DeleteFUCmd(EditPart* editPart);
    virtual ~DeleteFUCmd();
    virtual bool Do();

private:
    /// Function unit to delete.
    EditPart* editPart_;
};
#endif
