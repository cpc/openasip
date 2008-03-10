/**
 * @file DeleteBusCmd.hh
 *
 * Declaration of DeleteBusCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_DELETE_BUS_CMD_HH
#define TTA_DELETE_BUS_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for deleting buses from the Machine.
 */
class DeleteBusCmd: public ComponentCommand {
public:
    DeleteBusCmd(EditPart* editPart);
    virtual ~DeleteBusCmd();
    bool Do();

private:
    /// Bus to delete.
    EditPart* editPart_;
};
#endif
