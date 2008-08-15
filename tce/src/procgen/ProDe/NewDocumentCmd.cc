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
 * @file NewDocumentCmd.cc
 *
 * Definition of NewDocumentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/docmdi.h>

#include "NewDocumentCmd.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"

using std::string;


/**
 * The Constructor.
 */
NewDocumentCmd::NewDocumentCmd():
    EditorCommand(ProDeConstants::CMD_NAME_NEW_DOC) {

}


/**
 * The Destructor.
 */
NewDocumentCmd::~NewDocumentCmd() {}


/**
 * Executes the command.
 *
 * @return Always false. Returning true here would cause problems with the
 *         wxCommandProcessor, and the command is never undoable so it's
 *         safe to return false even if the command was succesfully executed.
 */
bool
NewDocumentCmd::Do() {
    
    wxDocManager* docManager = wxGetApp().docManager();
    
    // Disable multiple tabs if we're not using wxWidgets 2.8 or above    
    if (wxCHECK_VERSION(2, 8, 0)) {        
        docManager->CreateDocument(_T(""), wxDOC_NEW);
        return false;
    }
    else {
        // close the active document if there is any
        wxDocument *doc = docManager->GetCurrentDocument();
        if (doc) {
            if (doc->Close()) {
                doc->DeleteAllViews();
            }
            else { // cancel was pressed -> cancel the command
                return false;
            }
        }
    }
    
    // create a new document
    docManager->CreateDocument(_T(""), wxDOC_NEW);
    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
NewDocumentCmd::id() const {
    return ProDeConstants::COMMAND_NEW_DOC;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
NewDocumentCmd*
NewDocumentCmd::create() const {
    return new NewDocumentCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
NewDocumentCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_NEW_DOC;
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
NewDocumentCmd::icon() const {
    return ProDeConstants::CMD_ICON_NEW_DOC;
}


/**
 * This command is always executable.
 *
 * @return Always true.
 */
bool
NewDocumentCmd::isEnabled() {
    return true;
}
