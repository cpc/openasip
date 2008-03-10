/**
 * @file DeleteSegmentCmd.hh
 *
 * Declaration of DeleteSegmentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DELETE_SEGMENT_CMD_HH
#define TTA_DELETE_SEGMENT_CMD_HH

#include "ComponentCommand.hh"

class EditPart;

/**
 * Command for deleting segments from a machine.
 */
class DeleteSegmentCmd: public ComponentCommand {
public:
    DeleteSegmentCmd(EditPart* editPart);
    virtual ~DeleteSegmentCmd();
    virtual bool Do();

private:
    /// Segment to delete.
    EditPart* editPart_;
};
#endif
