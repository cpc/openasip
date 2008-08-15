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
 * @file HDBEditorDeleteCmd.cc
 *
 * Implementation of HDBEditorDeleteCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "HDBEditorDeleteCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "DBTypes.hh"
#include "HDBManager.hh"
#include "HDBBrowserWindow.hh"
#include "InformationDialog.hh"

/**
 * The Constructor.
 */
HDBEditorDeleteCmd::HDBEditorDeleteCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_DELETE, NULL) {
}


/**
 * The Destructor.
 */
HDBEditorDeleteCmd::~HDBEditorDeleteCmd() {
}

/**
 * Executes the command.
 */
bool
HDBEditorDeleteCmd::Do() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow* browser = wxGetApp().browser();
    if (browser->isFUArchitectureSelected()) {
        int id = browser->selectedFUArchitecture();
        if (manager->canRemoveFUArchitecture(id)) {
            manager->removeFUArchitecture(id);
        } else {
            wxString message = _T("FU Architecture ");
            message.Append(WxConversion::toWxString(id));
            message.Append(_T(" cannot be removed,\n"));
            message.Append(_T("because it has an implementation."));
            InformationDialog dialog(parentWindow(), message);
            dialog.ShowModal();
            return false;
        }
    } else if (browser->isRFArchitectureSelected()) {
        int id = browser->selectedRFArchitecture();
        if (manager->canRemoveRFArchitecture(id)) {
            manager->removeRFArchitecture(id);
        } else {
            wxString message = _T("RF Architecture ");
            message.Append(WxConversion::toWxString(id));
            message.Append(_T(" cannot be removed,\n"));
            message.Append(_T("because it has an implementation."));
            InformationDialog dialog(parentWindow(), message);
            dialog.ShowModal();
            return false;
        }
    } else if (browser->isFUImplementationSelected()) {
        int id = manager->fuEntryIDOfImplementation(
            browser->selectedFUImplementation());
        manager->removeFUEntry(id);
    } else if (browser->isRFImplementationSelected()) {
        int id = manager->rfEntryIDOfImplementation(
            browser->selectedRFImplementation());
        manager->removeRFEntry(id);
    } else if (browser->isCostFunctionPluginSelected()) {
        int id = browser->selectedCostFunctionPlugin();
        manager->removeCostFunctionPlugin(id);
    } else if (browser->isFUEntrySelected()) {
        int id = browser->selectedFUEntry();
        manager->removeFUEntry(id);
    } else if (browser->isRFEntrySelected()) {
        int id = browser->selectedRFEntry();
        manager->removeRFEntry(id);
    } else if (browser->isBusEntrySelected()) {
        int id = browser->selectedBusEntry();
        manager->removeBusEntry(id);
    } else if (browser->isSocketEntrySelected()) {
        int id = browser->selectedSocketEntry();
        manager->removeSocketEntry(id);
    } else {
        return false;
    }

    wxGetApp().mainFrame().update();

    return true;
}


/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
HDBEditorDeleteCmd::icon() const {
    return "";
}


/**
 * Returns the command id.
 *
 * @return Command identifier of this command.
 */
int
HDBEditorDeleteCmd::id() const {
    return HDBEditorConstants::COMMAND_DELETE;
}


/**
 * Creates a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
HDBEditorDeleteCmd*
HDBEditorDeleteCmd::create() const {
    return new HDBEditorDeleteCmd();
}


/**
 * Returns true if the command should be enabled in the menu/toolbar.
 *
 * @return True if the command is enabled, false if not.
 */
bool
HDBEditorDeleteCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow* browser = wxGetApp().browser();

    if (browser->isFUArchitectureSelected() ||
        browser->isRFArchitectureSelected() ||
        browser->isFUImplementationSelected() ||
        browser->isRFImplementationSelected() ||
        browser->isCostFunctionPluginSelected() ||
        browser->isFUEntrySelected() ||
        browser->isRFEntrySelected() ||
        browser->isBusEntrySelected() ||
        browser->isSocketEntrySelected()) {

        return true;
    }

    return false;
}

