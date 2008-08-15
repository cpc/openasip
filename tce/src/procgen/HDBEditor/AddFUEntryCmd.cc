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
 * @file AddFUEntryCmd.cc
 *
 * Implementation of AddFUEntryCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "AddFUEntryCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "DBTypes.hh"
#include "HDBManager.hh"
#include "HDBBrowserWindow.hh"

/**
 * The Constructor.
 */
AddFUEntryCmd::AddFUEntryCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_ADD_FU_ENTRY, NULL) {
}


/**
 * The Destructor.
 */
AddFUEntryCmd::~AddFUEntryCmd() {
}

/**
 * Executes the command.
 */
bool
AddFUEntryCmd::Do() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    RowID id = manager->addFUEntry();

    wxGetApp().mainFrame().update();
    wxGetApp().mainFrame().browser()->selectFUEntry(id);

    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddFUEntryCmd::icon() const {
    return "";
}

/**
 * Returns command id.
 *
 * @return Command identifier.
 */
int
AddFUEntryCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_FU_ENTRY;
}


/**
 * Creates a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddFUEntryCmd*
AddFUEntryCmd::create() const {
    return new AddFUEntryCmd();
}


/**
 * Returns true if the command should be enabled in the menu/toolbar.
 *
 * @return True if the command is enabled, false if not.
 */
bool
AddFUEntryCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    return true;
}
