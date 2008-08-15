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
 * @file PrintPreviewCmd.cc
 *
 * Definition of PrintPreviewCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/docview.h>

#include "config.h"
#include "PrintPreviewCmd.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"
#include "ErrorDialog.hh"

using std::string;


/**
 * The Constructor.
 */
PrintPreviewCmd::PrintPreviewCmd():
    EditorCommand(ProDeConstants::CMD_NAME_PRINT_PREVIEW) {

}


/**
 * The Destructor.
 */
PrintPreviewCmd::~PrintPreviewCmd() {
}


/**
 * Executes the command.
 *
 * If the wxWidgets printing framework was not compiled in the wxWidgets
 * library, print previeving is not possible. An error message will be
 * displayed instead of previewing.
 *
 * @return Always false. The command is not undoable.
 */
bool
PrintPreviewCmd::Do() {

#if WX_PRINTING_DISABLED

    // wxWidgets was compiled without printing framework, display an
    // error message.

    wxString message =
        wxString(_T("Printing framework is disabled in the wxWidgets\n"));

    message.Append(_T("library. Print previewing is not possible."));

    ErrorDialog error(parentWindow(), message);
    error.ShowModal();
    return false;

#else

    // wxWidgets was compiled with the printing framework.
    wxCommandEvent dummy;
    wxGetApp().docManager()->OnPreview(dummy);
    return false;

#endif // WX_PRINTING_ENABLED

}


/**
 * Return id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
PrintPreviewCmd::id() const {
    return ProDeConstants::COMMAND_PRINT_PREVIEW;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
PrintPreviewCmd*
PrintPreviewCmd::create() const {
    return new PrintPreviewCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
PrintPreviewCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_PRINT_PREVIEW;
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
PrintPreviewCmd::icon() const {
    return ProDeConstants::CMD_ICON_PRINT_PREVIEW;
}


/**
 * Returns true when command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if the command is executable.
 */
bool
PrintPreviewCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
        return true;
    }
    return false;
}
