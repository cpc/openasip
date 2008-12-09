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
 * @file HDBEditorDeleteCmd.cc
 *
 * Implementation of HDBEditorDeleteCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
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

