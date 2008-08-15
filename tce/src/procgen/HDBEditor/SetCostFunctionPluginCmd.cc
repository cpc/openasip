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
 * @file SetCostFunctionPluginCmd.cc
 *
 * Implementation of SetCostFunctionPluginCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
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

