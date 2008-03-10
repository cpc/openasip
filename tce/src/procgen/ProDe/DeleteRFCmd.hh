/**
 * @file DeleteRFCmd.hh
 *
 * Declaration of DeleteRFCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_DELETE_RF_CMD_HH
#define TTA_DELETE_RF_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for deleting register files from the Machine.
 */
class DeleteRFCmd: public ComponentCommand {
public:
    DeleteRFCmd(EditPart* editPart);
    virtual ~DeleteRFCmd();
    virtual bool Do();

private:
    /// Register file to delete.
    EditPart* editPart_;
};
#endif
