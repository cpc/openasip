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
 * @file OSEdBuildAllCmd.cc
 *
 * Definition of OSEdBuildAllCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <vector>
#include <boost/format.hpp>
#include <wx/timer.h>

#include "OSEdBuildAllCmd.hh"
#include "OSEdConstants.hh"
#include "OperationBuilder.hh"
#include "ResultDialog.hh"
#include "OperationContainer.hh"
#include "OSEdTextGenerator.hh"
#include "OperationModule.hh"
#include "OperationIndex.hh"
#include "WxConversion.hh"
#include "OSEd.hh"
#include "SimulateDialog.hh"

using std::string;
using std::vector;
using std::map;
using boost::format;

/**
 * Constructor.
 */
OSEdBuildAllCmd::OSEdBuildAllCmd() :
    GUICommand(OSEdConstants::CMD_NAME_BUILD_ALL, NULL) {
}

/**
 * Destructor.
 */
OSEdBuildAllCmd::~OSEdBuildAllCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdBuildAllCmd::id() const {
    return OSEdConstants::CMD_BUILD_ALL;
}

/**
 * Returns a new command.
 *
 * @return A new command.
 */
GUICommand*
OSEdBuildAllCmd::create() const {
    return new OSEdBuildAllCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful.
 */
bool
OSEdBuildAllCmd::Do() {

    wxWindow* simulateDialog =
        wxGetApp().mainFrame()->FindWindowByName(SimulateDialog::DIALOG_NAME);

    if (simulateDialog != NULL) {
        simulateDialog->Close();
    }

    OperationIndex& index = OperationContainer::operationIndex();
    OperationBuilder& builder = OperationBuilder::instance();
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    wxStatusBar* statusBar = wxGetApp().mainFrame()->statusBar();

    vector<OperationModule> buildModules;
    vector<string> paths;
    for (int i = 0; i < index.pathCount(); i++) {
    
        string path = index.path(i);
        for (int k = 0; k < index.moduleCount(path); k++) {
            
            OperationModule& module = index.module(k, path);
            if (module.hasBehaviorSource()) {
                buildModules.push_back(module);
                paths.push_back(path);
            }
        }
    }

    vector<string> output;
    wxStopWatch sw;
    sw.Pause();
    wxBusyCursor* busy = new wxBusyCursor();
    bool compileOk = true;
    for (size_t i = 0; i < buildModules.size(); i++) {
        OperationModule module = buildModules[i];
        string path = paths[i];
        output.push_back(module.name() + ":\n");
        size_t oldSize = output.size();
       
        sw.Resume();
        builder.buildObject(module.name(), 
                            module.behaviorSourceModule(), 
                            path, output);
        sw.Pause();

        if (oldSize != output.size()) {
            compileOk = false;
        }
        output.push_back("\n");
    }

    delete busy;

    if (!compileOk) {
        format fmt = texts.text(OSEdTextGenerator::TXT_STATUS_COMPILE_FAILED);
        statusBar->SetStatusText(WxConversion::toWxString(fmt.str()));
        fmt = texts.text(OSEdTextGenerator::TXT_BUILD_RESULT_DIALOG_TITLE);
        ResultDialog dialog(parentWindow(), output, fmt.str());
        dialog.ShowModal();
    } else {
        format fmt = texts.text(OSEdTextGenerator::TXT_STATUS_COMPILE_SUCCESS);
        fmt % sw.Time();
        statusBar->SetStatusText(WxConversion::toWxString(fmt.str()));
    }
    return true;
}

/**
 * Returns true if command is enabled.
 *
 * Command is enabled, if at least one module has behavior source file.
 *
 * @return True if command is enabled.
 */
bool
OSEdBuildAllCmd::isEnabled() {
    OperationIndex& index = OperationContainer::operationIndex();
    for (int i = 0; i < index.moduleCount(); i++) {
        OperationModule& module = index.module(i);
        if (module.hasBehaviorSource()) {
            return true;
        }
    }
    return false;
}

/**
 * Returns icon path.
 *
 * @return Empty string (icons not used).
 */
string 
OSEdBuildAllCmd::icon() const {
    return "";
}
