/**
 * @file ProximShowRegistersCmd.cc
 *
 * Implementation of ProximShowRegistersCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximShowRegistersCmd.hh"
#include "EditPart.hh"
#include "ProximRegisterWindow.hh"
#include "MachinePart.hh"
#include "ProximToolbox.hh"

/**
 * The Constructor.
 */
ProximShowRegistersCmd::ProximShowRegistersCmd(
    EditPart* part) :
    ComponentCommand(),
    part_(part) {

}


/**
 * The Destructor.
 */
ProximShowRegistersCmd::~ProximShowRegistersCmd() {
}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 *
 */
bool
ProximShowRegistersCmd::Do() {

    std::string rfName =
        dynamic_cast<TTAMachine::Component*>(part_->model())->name();

    ProximRegisterWindow* regWindow = new ProximRegisterWindow(
        ProximToolbox::mainFrame(), -1);

    ProximToolbox::addFramedWindow(
        regWindow, _T("Registers"), false, wxSize(270, 200));

    regWindow->GetParent()->SetSize(270, 300);

    regWindow->showRegisterFile(rfName);

    return true;
}
