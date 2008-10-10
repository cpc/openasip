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
 * @file OSEdBuildCmd.cc
 *
 * Definition of OSEdBuildCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <vector>
#include <boost/format.hpp>
#include <wx/timer.h>

#include "OSEdBuildCmd.hh"
#include "OSEdConstants.hh"
#include "OSEd.hh"
#include "OperationBuilder.hh"
#include "OperationContainer.hh"
#include "ResultDialog.hh"
#include "OSEdTextGenerator.hh"
#include "OperationModule.hh"
#include "OSEdTreeView.hh"
#include "WxConversion.hh"
#include "SimulateDialog.hh"

using std::string;
using std::vector;
using boost::format;

/**
 * Constructor.
 */
OSEdBuildCmd::OSEdBuildCmd() : 
    GUICommand(OSEdConstants::CMD_NAME_BUILD, NULL) {
}

/**
 * Destructor.
 */
OSEdBuildCmd::~OSEdBuildCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdBuildCmd::id() const {
    return OSEdConstants::CMD_BUILD;
}

/**
 * Creates a new command.
 *
 * @return A new command.
 */
GUICommand*
OSEdBuildCmd::create() const {
    return new OSEdBuildCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful.
 */
bool
OSEdBuildCmd::Do() {

    wxWindow* simulateDialog =
        wxGetApp().mainFrame()->FindWindowByName(SimulateDialog::DIALOG_NAME);
   
    if (simulateDialog != NULL) {
        simulateDialog->Close();
    }

    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    wxStatusBar* statusBar = wxGetApp().mainFrame()->statusBar();
    OSEdTreeView* treeView = wxGetApp().mainFrame()->treeView();
	
    string modName = treeView->selectedModule();
    wxTreeItemId modId = treeView->selectedModuleId();
    string pathName = treeView->pathOfModule(modId);
    OperationModule& module = OperationContainer::module(pathName, modName);
	
    format fmt = texts.text(OSEdTextGenerator::TXT_STATUS_COMPILING);
    fmt % module.name();
    statusBar->SetStatusText(WxConversion::toWxString(fmt.str()));

    vector<string> output;
    output.push_back(module.name() + ":\n");
    OperationBuilder& builder = OperationBuilder::instance();
    size_t oldSize = output.size();

    wxStopWatch sw;
    wxBusyCursor* busy = new wxBusyCursor();
    builder.buildObject(
        module.name(), module.behaviorSourceModule(), pathName, output);
    delete busy;
    sw.Pause();

    if (oldSize == output.size()) {
        fmt = texts.text(OSEdTextGenerator::TXT_STATUS_COMPILE_SUCCESS);
        fmt % sw.Time();
        statusBar->SetStatusText(WxConversion::toWxString(fmt.str()));
    } else {
        fmt = texts.text(OSEdTextGenerator::TXT_STATUS_COMPILE_FAILED);
        statusBar->SetStatusText(WxConversion::toWxString(fmt.str()));
        fmt = texts.text(OSEdTextGenerator::TXT_BUILD_RESULT_DIALOG_TITLE);
        ResultDialog dialog(parentWindow(), output, fmt.str(), 
                            module.behaviorSourceModule());
        dialog.ShowModal();
    }
    return true;
}

/**
 * Returns true if command is enabled.
 *
 * Command is enabled when module with behavior source file has been
 * selected.
 *
 * @return True if command is enabled.
 */
bool
OSEdBuildCmd::isEnabled() {
    OSEdMainFrame* mainFrame = wxGetApp().mainFrame();
    OSEdTreeView* treeView = mainFrame->treeView();
    string modName = treeView->selectedModule();
    if (modName == "") {
        return false;
    }
    wxTreeItemId modId = treeView->selectedModuleId();
    string pathName = treeView->pathOfModule(modId);
    OperationModule& module = OperationContainer::module(pathName, modName);
    return module.hasBehaviorSource();
}

/**
 * Returns the icon path.
 *
 * @return Empty string (icons not used).
 */
string
OSEdBuildCmd::icon() const {
    return "";
}
