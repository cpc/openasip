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
 * @file HDBEditorModifyCmd.cc
 *
 * Implementation of HDBEditorModifyCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
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
