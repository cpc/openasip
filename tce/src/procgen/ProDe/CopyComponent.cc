/**
 * @file CopyComponent.cc
 *
 * Definition of CopyComponent class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include "Application.hh"
#include "CopyComponent.hh"
#include "FunctionUnit.hh"
#include "EditPart.hh"
#include "ProDeClipboard.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Function unit to delete.
 */
CopyComponent::CopyComponent(EditPart* editPart): editPart_(editPart) {
}


/**
 * The Destructor.
 */
CopyComponent::~CopyComponent() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
CopyComponent::Do() {
    Component* component = dynamic_cast<Component*>(editPart_->model());
    assert (component != NULL);
    ProDeClipboard* clipboard = ProDeClipboard::instance();
    clipboard->setContents(component->saveState());
    return true;
}


/**
 * Undoing this command is not possible, returns always false.
 *
 * @return Always false.
 */
bool
CopyComponent::Undo() {
    return false;
}

