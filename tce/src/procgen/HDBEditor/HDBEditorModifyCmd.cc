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
 * @file HDBEditorModifyCmd.cc
 *
 * Implementation of HDBEditorModifyCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "HDBEditorModifyCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "HDBBrowserWindow.hh"
#include "HDBManager.hh"

#include "FUImplementationDialog.hh"
#include "FUImplementation.hh"
#include "FUEntry.hh"
#include "FUArchitecture.hh"

#include "RFImplementationDialog.hh"
#include "RFImplementation.hh"
#include "RFEntry.hh"
#include "RFArchitecture.hh"

#include "CostFunctionPluginDialog.hh"

#include "ErrorDialog.hh"
#include "Application.hh"


/**
 * The Constructor.
 */
HDBEditorModifyCmd::HDBEditorModifyCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_MODIFY, NULL) {
}


/**
 * The Destructor.
 */
HDBEditorModifyCmd::~HDBEditorModifyCmd() {
}

/**
 * Executes the command.
 */
bool
HDBEditorModifyCmd::Do() {

    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow* browser = wxGetApp().mainFrame().browser();


    if (browser->isFUImplementationSelected()) {

        // FU implementation modification.

        int entryID = manager->fuEntryIDOfImplementation(
            browser->selectedFUImplementation());

        HDB::FUEntry* entry = manager->fuByEntryID(entryID);
        
        FUImplementationDialog dialog(
            parentWindow(), -1, entry->implementation(),
            entry->architecture().architecture());

        if (dialog.ShowModal() == wxID_OK) {
            try {
                assert(entry->hasImplementation() &&
                       entry->implementation().hasID());

                int implID = entry->implementation().id();
                manager->removeFUImplementation(implID);
                int id = manager->addFUImplementation(*entry);
                wxGetApp().mainFrame().update();
                wxGetApp().mainFrame().browser()->selectFUImplementation(id);
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
                return false;
            }
        } else {
            delete entry;
        }

        return true;
    } else if (browser->isRFImplementationSelected()) {

        // RF implementation modification.

        int entryID = manager->rfEntryIDOfImplementation(
            browser->selectedRFImplementation());

        HDB::RFEntry* entry = manager->rfByEntryID(entryID);

        RFImplementationDialog dialog(
            parentWindow(), -1, entry->implementation());

        if (dialog.ShowModal() == wxID_OK) {
            try {
                assert(entry->hasImplementation() &&
                       entry->implementation().hasID());

                int implID = entry->implementation().id();
                manager->removeRFImplementation(implID);
                int id = manager->addRFImplementation(
                    entry->implementation(), entryID);

                //wxGetApp().mainFrame().update();
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
                return false;
            }
        } else {
            delete entry;
        }

        return true;
    } else if (browser->isCostFunctionPluginSelected()) {

        CostFunctionPluginDialog dialog(
            parentWindow(), -1,
            *manager, browser->selectedCostFunctionPlugin());

        dialog.ShowModal();
        wxGetApp().mainFrame().update();
        return true;
    }

    return false;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
HDBEditorModifyCmd::icon() const {
    return "";
}

/**
 * Returns the command id.
 *
 * @return Command identifier.
 */
int
HDBEditorModifyCmd::id() const {
    return HDBEditorConstants::COMMAND_MODIFY;
}


/**
 * Creates a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
HDBEditorModifyCmd*
HDBEditorModifyCmd::create() const {
    return new HDBEditorModifyCmd();
}


/**
 * Returns true if the command should be enabled in the tool/menubar.
 *
 * @return True if the command is enabled, false if not.
 */
bool
HDBEditorModifyCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow* browser = wxGetApp().mainFrame().browser();

    if (browser->isFUImplementationSelected() ||
        browser->isRFImplementationSelected() ||
        browser->isCostFunctionPluginSelected()) {
        return true;
    }

    return false;
}
