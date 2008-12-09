/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file AddRFEntryCmd.cc
 *
 * Implementation of AddRFEntryCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "AddRFEntryCmd.hh"
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
AddRFEntryCmd::AddRFEntryCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_ADD_RF_ENTRY, NULL) {
}


/**
 * The Destructor.
 */
AddRFEntryCmd::~AddRFEntryCmd() {
}

/**
 * Executes the command.
 */
bool
AddRFEntryCmd::Do() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    RowID id = manager->addRFEntry();

    wxGetApp().mainFrame().update();
    wxGetApp().mainFrame().browser()->selectRFEntry(id);

    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddRFEntryCmd::icon() const {
    return "";
}


/**
 * Returns command identifier for this command.
 *
 * @return Command identifier of this command.
 */
int
AddRFEntryCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_RF_ENTRY;
}


/**
 * Creates and returns new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddRFEntryCmd*
AddRFEntryCmd::create() const {
    return new AddRFEntryCmd();
}


/**
 * Returns true, if the command should be enabled in the menu/toolbar.
 *
 * @return True, if the command is enabled, false if not.
 */
bool
AddRFEntryCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    return true;
}

