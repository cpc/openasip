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
 * @file AddCostFunctionPluginCmd.cc
 *
 * Implementation of AddCostFunctionPluginCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "AddCostFunctionPluginCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"
#include "HDBManager.hh"
#include "ErrorDialog.hh"
#include "HDBBrowserWindow.hh"
#include "CostFunctionPlugin.hh"
#include "CostFunctionPluginDialog.hh"

using namespace HDB;

/**
 * The Constructor.
 */
AddCostFunctionPluginCmd::AddCostFunctionPluginCmd() :
    GUICommand(HDBEditorConstants::COMMAND_NAME_ADD_COST_PLUGIN, NULL) {
}


/**
 * The Destructor.
 */
AddCostFunctionPluginCmd::~AddCostFunctionPluginCmd() {
}

/**
 * Executes the command.
 */
bool
AddCostFunctionPluginCmd::Do() {

    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    if (manager == NULL) {
        return false;
    }

    CostFunctionPlugin plugin(0, "", "", "", CostFunctionPlugin::COST_FU);
    RowID id = manager->addCostFunctionPlugin(plugin);
    CostFunctionPluginDialog dialog(parentWindow(), -1, *manager, id);

    if (dialog.ShowModal() == wxID_OK) {
        wxGetApp().mainFrame().update();
        wxGetApp().mainFrame().browser()->selectCostFunctionPlugin(id);
        return true;
    } else {
        manager->removeCostFunctionPlugin(id);
        return false;
    }
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddCostFunctionPluginCmd::icon() const {
    return "";
}


/**
 * Returns the command identifier.
 *
 * @return Command identifier for this command.
 */
int
AddCostFunctionPluginCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_COST_PLUGIN;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return A new instance of this command.
 */
AddCostFunctionPluginCmd*
AddCostFunctionPluginCmd::create() const {
    return new AddCostFunctionPluginCmd();
}


/**
 * Returns true if the command should be enabled in menu/toolbar, false if not.
 *
 * @return True, if the command is enabled, false if not.
 */
bool
AddCostFunctionPluginCmd::isEnabled() {

    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }
    return true;
}
