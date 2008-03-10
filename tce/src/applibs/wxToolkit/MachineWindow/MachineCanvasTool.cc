/**
 * @file MachineCanvasTool.cc
 * 
 * Definition of MachineCanvasTool class.
 * 
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "MachineCanvasTool.hh"

/**
 * The Constructor.
 *
 * @param canvas MachineCanvas where the tool is used.
 */
MachineCanvasTool::MachineCanvasTool(MachineCanvas* canvas) :
    canvas_(canvas) {
}


/**
 * The Destructor.
 */
MachineCanvasTool::~MachineCanvasTool() {
}


/**
 * Returns figure of the tool, or NULL if the tool has no figure.
 *
 * @return Base class implementation returns always NULL (no figure).
 */
Figure*
MachineCanvasTool::figure() {
    return NULL;
}
