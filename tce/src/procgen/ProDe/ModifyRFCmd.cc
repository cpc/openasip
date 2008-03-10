/**
 * @file ModifyRFCmd.cc
 *
 * Definition of ModifyRFCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include "Application.hh"
#include "ModifyRFCmd.hh"
#include "RegisterFile.hh"
#include "EditPart.hh"
#include "Model.hh"
#include "RFDialog.hh"
#include "ErrorDialog.hh"
#include "ProDe.hh"
#include "MDFDocument.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ModifyRFCmd::ModifyRFCmd(EditPart* editPart):
    editPart_(editPart) {

}


/**
 * The Destructor.
 */
ModifyRFCmd::~ModifyRFCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifyRFCmd::Do() {

    RegisterFile* rf = dynamic_cast<RegisterFile*>(editPart_->model());
    assert (rf != NULL);
    assert (parentWindow() != NULL);

    RFDialog dialog(parentWindow(), rf);

    if (dialog.ShowModal() == wxID_OK) {
	// register file was modified
	return true;
    } else {
	// modification was cancelled
	return false;
    }

    return false;
}
