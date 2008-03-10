/**
 * @file DeleteSegmentCmd.cc
 *
 * Definition of DeleteSegmentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "DeleteSegmentCmd.hh"
#include "Segment.hh"
#include "EditPart.hh"


using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Segment to delete.
 */
DeleteSegmentCmd::DeleteSegmentCmd(EditPart* editPart):
    ComponentCommand(), editPart_(editPart) {
}


/**
 * The Destructor.
 */
DeleteSegmentCmd::~DeleteSegmentCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
DeleteSegmentCmd::Do() {
    Segment* segment = dynamic_cast<Segment*>(editPart_->model());
    assert (segment != NULL);
    delete segment;
    return true;
}
