/**
 * @file DeleteIUCmd.hh
 *
 * Declaration of DeleteIUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_DELETE_IU_CMD_HH
#define TTA_DELETE_IU_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for deleting immediate units from the Machine.
 */
class DeleteIUCmd: public ComponentCommand {
public:
    DeleteIUCmd(EditPart* editPart);
    virtual ~DeleteIUCmd();
    virtual bool Do();

private:
    /// Immediate unit to delete.
    EditPart* editPart_;
};
#endif
