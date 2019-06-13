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
 * @file AddCostFunctionPluginCmd.cc
 *
 * Implementation of AddCostFunctionPluginCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
