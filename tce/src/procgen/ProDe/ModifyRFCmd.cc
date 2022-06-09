/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file ModifyRFCmd.cc
 *
 * Definition of ModifyRFCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
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
