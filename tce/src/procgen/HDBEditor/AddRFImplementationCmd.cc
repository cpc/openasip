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
 * @file AddRFImplementationCmd.cc
 *
 * Implementation of AddRFImplementationCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "AddRFImplementationCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "HDBBrowserWindow.hh"
#include "HDBManager.hh"
#include "RFImplementationDialog.hh"
#include "RFImplementation.hh"
#include "RFEntry.hh"
#include "ErrorDialog.hh"


/**
 * The Constructor.
 */
AddRFImplementationCmd::AddRFImplementationCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_ADD_RF_IMPLEMENTATION, NULL) {
}


/**
 * The Destructor.
 */
AddRFImplementationCmd::~AddRFImplementationCmd() {
}

/**
 * Executes the command.
 */
bool
AddRFImplementationCmd::Do() {

    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow* browser = wxGetApp().mainFrame().browser();

    HDB::RFImplementation* implementation
        = new HDB::RFImplementation("", "", "","", "", "", "");

    HDB::RFEntry* entry = NULL;

    if (browser->isRFEntrySelected()) {
        entry = manager->rfByEntryID(browser->selectedRFEntry());
    } else if (browser->isRFArchitectureSelected()) {
        int id = manager->addRFEntry();
        manager->setArchitectureForRF(id, browser->selectedRFArchitecture());
        entry = manager->rfByEntryID(id);
    } else {
        return false;
    }

    RFImplementationDialog dialog(parentWindow(), -1, *implementation);

    if (dialog.ShowModal() == wxID_OK) {
        try {
            int id =
                manager->addRFImplementation(*implementation, entry->id());
            wxGetApp().mainFrame().update();
            wxGetApp().mainFrame().browser()->selectRFImplementation(id);
        } catch (Exception& e) {
            wxString message = _T("Error:\n");
            message.Append(WxConversion::toWxString(e.errorMessage()));
            message.Append(_T("\n"));
            message.Append(WxConversion::toWxString(e.lineNum()));
             message.Append(_T(": "));
            message.Append(WxConversion::toWxString(e.fileName()));
            ErrorDialog dialog(parentWindow(), message);
            dialog.ShowModal();
            delete entry;
            delete implementation;
            return false;
        }
    } else {
        delete implementation;
    }

    delete entry;
    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddRFImplementationCmd::icon() const {
    return "";
}

/**
 * Returns the command id.
 *
 * @return Command identifier.
 */
int
AddRFImplementationCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_RF_IMPLEMENTATION;
}


/**
 * Creates a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddRFImplementationCmd*
AddRFImplementationCmd::create() const {
    return new AddRFImplementationCmd();
}


/**
 * Returns true if the command should be enabled in the tool/menubar.
 *
 * @return True if the command is enabled, false if not.
 */
bool
AddRFImplementationCmd::isEnabled() {

    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow* browser = wxGetApp().mainFrame().browser();


    if (browser->isRFArchitectureSelected()) {
        return true;
    } else if (browser->isRFEntrySelected()) {
        HDB::RFEntry* entry = manager->rfByEntryID(browser->selectedRFEntry());
        if (entry->hasArchitecture() && !entry->hasImplementation()) {
            delete entry;
            return true;
        }
        delete entry;
    }

    return false;
}
