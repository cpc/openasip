/**
 * @file OSEdMemoryCmd.cc
 *
 * Definition of OSEdMemoryCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include "OSEdMemoryCmd.hh"
#include "MemoryDialog.hh"
#include "OSEdConstants.hh"
#include "OSEdInformer.hh"
#include "OSEd.hh"

using std::string;

/**
 * Constructor.
 */
OSEdMemoryCmd::OSEdMemoryCmd() :
    GUICommand(OSEdConstants::CMD_NAME_MEMORY, NULL) {
}

/**
 * Destructor.
 */
OSEdMemoryCmd::~OSEdMemoryCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdMemoryCmd::id() const {
    return OSEdConstants::CMD_MEMORY;
}

/**
 * Creates a new command.
 *
 * @return The created command.
 */
GUICommand*
OSEdMemoryCmd::create() const {
    return new OSEdMemoryCmd();
}

/**
 * Executes the command.
 * 
 * @return Always true.
 */
bool
OSEdMemoryCmd::Do() {
    MemoryDialog* dialog = new MemoryDialog(parentWindow());
    dialog->Show();
    return true;
}

/**
 * Returns true if command is enabled.
 *
 * Memory command is always enabled.
 *
 * @return Always true.
 */
bool
OSEdMemoryCmd::isEnabled() {
    return true;
}

/**
 * Returns icon path.
 *
 * @return Empty string (icons not used).
 */
string
OSEdMemoryCmd::icon() const {
    return "";
}
