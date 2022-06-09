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
 * @file SetRFArchitectureCmd.cc
 *
 * Implementation of SetRFArchitectureCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "SetRFArchitectureCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "HDBManager.hh"
#include "RFEntry.hh"
#include "HDBBrowserWindow.hh"
#include "Conversion.hh"

using namespace HDB;

/**
 * The Constructor.
 */
SetRFArchitectureCmd::SetRFArchitectureCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_SET_RF_ARCH, NULL) {
}


/**
 * The Destructor.
 */
SetRFArchitectureCmd::~SetRFArchitectureCmd() {
}

/**
 * Executes the command.
 */
bool
SetRFArchitectureCmd::Do() {

    HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    const RFEntry* entry = manager->rfByID(
        wxGetApp().mainFrame().browser()->selectedRFEntry());

    wxString rfs[1000];
    const std::set<RowID> rfIDs = manager->rfArchitectureIDs();
    std::set<RowID>::iterator iter = rfIDs.begin();
    int i = 0;
    for (;iter != rfIDs.end(); iter++) {
        rfs[i] = WxConversion::toWxString(*iter);
        i++;
    }
    
    wxSingleChoiceDialog choicer(
        parentWindow(), _T("Choose Architecture"), _T("Choose Architecture"),
        i, rfs);

    if (choicer.ShowModal() != wxID_OK) {
        return false;
    }


    std::string archID = WxConversion::toString(choicer.GetStringSelection());
    RowID rowID = Conversion::toInt(archID);
    manager->unsetArchitectureForRF(entry->id());
    manager->setArchitectureForRF(entry->id(), rowID);

    wxGetApp().mainFrame().update();
    wxGetApp().mainFrame().browser()->selectRFEntry(entry->id());

    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
SetRFArchitectureCmd::icon() const {
    return "";

}


/**
 * Returns the command id.
 *
 * @return Command identifier for this command.
 */
int
SetRFArchitectureCmd::id() const {
    return HDBEditorConstants::COMMAND_SET_RF_ARCH;
}


/**
 * Creates and returns a new instance of the command.
 *
 * @return Newly created instance of this command.
 */
SetRFArchitectureCmd*
SetRFArchitectureCmd::create() const {
    return new SetRFArchitectureCmd();
}

/**
 * Returns true if the command should be enabled in the menu/toolbar.
 *
 * @return True if the command is enabled, false if not.
 */
bool
SetRFArchitectureCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    if (!wxGetApp().mainFrame().browser()->isRFEntrySelected()) {
        return false;
    }
    return true;
}

