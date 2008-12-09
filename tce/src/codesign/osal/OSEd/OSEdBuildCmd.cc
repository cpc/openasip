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
