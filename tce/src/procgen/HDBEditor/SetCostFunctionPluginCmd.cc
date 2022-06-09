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
 * @file SetCostFunctionPluginCmd.cc
 *
 * Implementation of SetCostFunctionPluginCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "SetCostFunctionPluginCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "HDBManager.hh"
#include "HDBBrowserWindow.hh"
#include "Conversion.hh"
#include "CostFunctionPlugin.hh"

using namespace HDB;

/**
 * The Constructor.
 */
SetCostFunctionPluginCmd::SetCostFunctionPluginCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_SET_COST_PLUGIN, NULL) {
}


/**
 * The Destructor.
 */
SetCostFunctionPluginCmd::~SetCostFunctionPluginCmd() {
}

/**
 * Executes the command.
 */
bool
SetCostFunctionPluginCmd::Do() {

    HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow& browser =  *wxGetApp().mainFrame().browser();

    const std::set<RowID> pluginIDs = manager->costFunctionPluginIDs();
    wxString plugins[100];

    std::map<int, RowID> rowIDs;
    std::set<RowID>::iterator iter = pluginIDs.begin();
    plugins[0] = _T("NONE");
    int i = 1;

    // Append plugin names to an array for the choicer dialog.
    for (;iter != pluginIDs.end(); iter++) {

        const CostFunctionPlugin* plugin =
            manager->costFunctionPluginByID(*iter);

        plugins[i] = WxConversion::toWxString(plugin->name());
        delete plugin;
        plugin = NULL;

        rowIDs[i] = *iter;
        i++;
    }

    // Display single choice dialog of cost function plugins.
    wxSingleChoiceDialog dialog(
        parentWindow(), _T("Choose Cost Function Plugin"),
        _T("Choose Cost Function Pluhin"),
        i, plugins);

    if (dialog.ShowModal() != wxID_OK) {
        return false;
    }

    // Set the selected plugin as the entry plugin.
    if (browser.isFUEntrySelected()) {
        // FU entry.
        int entryID = browser.selectedFUEntry();
        if (dialog.GetSelection() > 0) {
            manager->setCostFunctionPluginForFU(
                entryID, rowIDs[dialog.GetSelection()]);
        } else {
            manager->unsetCostFunctionPluginForFU(entryID);
        }
        wxGetApp().mainFrame().update();
        browser.selectFUEntry(entryID);
        return true;
    } else if (browser.isRFEntrySelected()) {
        // RF entry.
        int entryID = browser.selectedRFEntry();
        if (dialog.GetSelection() > 0) {
            manager->setCostFunctionPluginForRF(
                entryID, rowIDs[dialog.GetSelection()]);
        } else {
            manager->unsetCostFunctionPluginForRF(entryID);
        }
        wxGetApp().mainFrame().update();
        browser.selectRFEntry(entryID);
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
SetCostFunctionPluginCmd::icon() const {
    return "";

}


/**
 * Returns the command id.
 *
 * @return Command identifier for this command.
 */
int
SetCostFunctionPluginCmd::id() const {
    return HDBEditorConstants::COMMAND_SET_COST_PLUGIN;
}


/**
 * Creates and returns a new instance of the command.
 *
 * @return Newly created instance of this command.
 */
SetCostFunctionPluginCmd*
SetCostFunctionPluginCmd::create() const {
    return new SetCostFunctionPluginCmd();
}

/**
 * Returns true if the command should be enabled in the menu/toolbar.
 *
 * @return True if the command is enabled, false if not.
 */
bool
SetCostFunctionPluginCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    HDBBrowserWindow* browser = wxGetApp().mainFrame().browser();

    if (browser->isFUEntrySelected() ||
        browser->isRFEntrySelected()) {

        return true;
    }
    return false;
}

