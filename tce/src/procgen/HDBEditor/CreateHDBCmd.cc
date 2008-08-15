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
 * @file CreateHDBCmd.cc
 *
 * Implementation of CreateHDBCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "CreateHDBCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"

/**
 * The Constructor.
 */
CreateHDBCmd::CreateHDBCmd():
    GUICommand(HDBEditorConstants::COMMAND_NAME_CREATE_HDB, NULL) {
}


/**
 * The Destructor.
 */
CreateHDBCmd::~CreateHDBCmd() {
}


/**
 * Executes the command.
 */
bool
CreateHDBCmd::Do() {
    assert(parentWindow() != NULL);

    wxString wildcard = _T("Hardware Database files (*.hdb)|*.hdb");
    wildcard.Append(_T("|All files|*"));
    wxFileDialog dialog(
	parentWindow(), _T("Enter path for the new HDB"), _T(""), _T(""),
	wildcard, wxSAVE);

    if (dialog.ShowModal() == wxID_CANCEL) {
	return false;
    }

    std::string command;
    std::string file = WxConversion::toString(dialog.GetPath());

    return wxGetApp().mainFrame().createHDB(file);
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
CreateHDBCmd::icon() const {
    return "filenew.png";
}


/**
 * Returns ID of this command.
 */
int
CreateHDBCmd::id() const {
    return HDBEditorConstants::COMMAND_CREATE_HDB;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
CreateHDBCmd*
CreateHDBCmd::create() const {
    return new CreateHDBCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
CreateHDBCmd::isEnabled() {
    return true;
}
