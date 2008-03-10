/**
 * @file ProximShowPortsCmd.cc
 *
 * Implementation of ProximShowPortsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximShowPortsCmd.hh"
#include "EditPart.hh"
#include "ProximPortWindow.hh"
#include "MachinePart.hh"
#include "ProximToolbox.hh"

/**
 * The Constructor.
 */
ProximShowPortsCmd::ProximShowPortsCmd(
    EditPart* part) :
    ComponentCommand(),
    part_(part) {

}


/**
 * The Destructor.
 */
ProximShowPortsCmd::~ProximShowPortsCmd() {
}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 *
 */
bool
ProximShowPortsCmd::Do() {

    std::string fuName =
        dynamic_cast<TTAMachine::Component*>(part_->model())->name();
    
    ProximPortWindow* portWin =
        new ProximPortWindow(ProximToolbox::mainFrame(), -1);

    ProximToolbox::addFramedWindow(
        portWin, _T("Ports"), false, wxSize(270, 200));
    
    portWin->GetParent()->SetSize(270, 300);

    portWin->showFunctionUnit(fuName);

    return true;
}
