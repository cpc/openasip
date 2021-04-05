/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file BlocksConnectICCmd.cc
 *
 * Definition of BlocksConnectICCmd class. (Derived from VLIWConnectICCmd.cc)
 *
 * @author Kanishkan Vadivel 2021 (k.vadivel-no.spam-tue.nl)
 * @note rating: red
 */

#include "BlocksConnectICCmd.hh"

#include <wx/docview.h>

#include <string>

#include "DSDBManager.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "FileSystem.hh"
#include "MDFDocument.hh"
#include "Machine.hh"
#include "Model.hh"
#include "ProDe.hh"
#include "ProDeConstants.hh"

using namespace TTAMachine;

BlocksConnectICCmd::BlocksConnectICCmd()
    : EditorCommand(ProDeConstants::CMD_NAME_BLOCKS_CONNECT_IC) {}

BlocksConnectICCmd::~BlocksConnectICCmd() {}

/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
BlocksConnectICCmd::Do() {
    assert(view() != NULL);

    TTAMachine::Machine* machine =
        dynamic_cast<MDFDocument*>(view()->GetDocument())
            ->getModel()
            ->getMachine();

    Model* model =
        dynamic_cast<MDFDocument*>(view()->GetDocument())->getModel();

    model->pushToStack();

    // open up a temporary dsdb
    const std::string dsdbFile = "tmp.dsdb";
    FileSystem::removeFileOrDirectory(dsdbFile);
    DSDBManager* dsdb = DSDBManager::createNew(dsdbFile);
    RowID archID = dsdb->addArchitecture(*machine);

    DSDBManager::MachineConfiguration confIn(archID, false, ILLEGAL_ROW_ID);
    RowID confID = dsdb->addConfiguration(confIn);

    // load BlocksConnectIC explorer plugin
    DesignSpaceExplorer explorer;
    DesignSpaceExplorerPlugin* plugin =
        explorer.loadExplorerPlugin("BlocksConnectIC", dsdb);
    plugin->setDSDB(*dsdb);

    try {
        std::vector<RowID> result = plugin->explore(confID, 0);
        // store the new machine
        TTAMachine::Machine& newMachine =
            *dsdb->architecture(confID + result.size());
        machine->copyFromMachine(newMachine);
    } catch (Exception e) {
        std::cerr << "Could not create Blocks Connect IC" << std::endl;
        FileSystem::removeFileOrDirectory(dsdbFile);
        model->popFromStack();
        return false;
    }
    FileSystem::removeFileOrDirectory(dsdbFile);
    model->notifyObservers();

    return true;
}

/**
 * Returns id of this command.
 */
int
BlocksConnectICCmd::id() const {
    return ProDeConstants::COMMAND_BLOCKS_CONNECT_IC;
}

/**
 * Creates and returns a new instance of this command.
 */
BlocksConnectICCmd*
BlocksConnectICCmd::create() const {
    return new BlocksConnectICCmd();
}

/**
 * Returns short version of the command name.
 */
std::string
BlocksConnectICCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_BLOCKS_CONNECT_IC;
}

/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
BlocksConnectICCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    wxView* view = manager->GetCurrentView();

    if (view == NULL) {
        return false;
    }

    Model* model =
        dynamic_cast<MDFDocument*>(view->GetDocument())->getModel();

    if (model == NULL) {
        return false;
    }
    return true;
}
