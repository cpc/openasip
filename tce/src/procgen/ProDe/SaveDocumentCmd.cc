/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file SaveDocumentCmd.cc
 *
 * Definition of SaveDocumentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "SaveDocumentCmd.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"

using std::string;

/**
 * The Constructor.
 */
SaveDocumentCmd::SaveDocumentCmd():
    EditorCommand(ProDeConstants::CMD_NAME_SAVE_DOC) {

}


/**
 * The Destructor.
 */
SaveDocumentCmd::~SaveDocumentCmd() {}


/**
 * Executes the command.
 *
 * @return Always false. Returning true here would cause problems with the
 *         wxCommandProcessor, and the command is never undoable so it's
 *         safe to return false even if the command was succesfully executed.
 */
bool
SaveDocumentCmd::Do() {
    wxDocument* doc = wxGetApp().docManager()->GetCurrentDocument();
    assert(doc != NULL);

    // warning: Dirty trick to avoid file dialog on saving a document
    // already assigned into a specific file name. wxID_SAVE handler
    // always opens file dialog. Checking that file name does not
    // contain a slash should verify that the document has not been
    // saved before.
    wxString currentDocFilename = doc->GetFilename();
    if (doc->GetFilename().Find('/') == -1) {
        wxCommandEvent dummy(wxID_SAVE, 0);
        wxGetApp().docManager()->OnFileSave(dummy);
    } else {
        doc->OnSaveDocument(currentDocFilename);
    }

    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
SaveDocumentCmd::id() const {
    return ProDeConstants::COMMAND_SAVE_DOC;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
SaveDocumentCmd*
SaveDocumentCmd::create() const {
    return new SaveDocumentCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
SaveDocumentCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_SAVE_DOC;
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
SaveDocumentCmd::icon() const {
    return ProDeConstants::CMD_ICON_SAVE_DOC;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
SaveDocumentCmd::isEnabled() {
    wxDocument* doc = wxGetApp().docManager()->GetCurrentDocument();
    if (doc != NULL && doc->IsModified()) {
	return true;
    }
    return false;
}

