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
 * @file SaveOptionsCmd.cc
 *
 * Definition of SaveOptionsCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 */

#include "SaveOptionsCmd.hh"
#include "OptionsDialog.hh"
#include "ProDeConstants.hh"
#include "ProDeOptions.hh"
#include "ProDe.hh"
#include "ProDeOptionsSerializer.hh"
#include "ObjectState.hh"
#include "WxConversion.hh"
#include "CommandRegistry.hh"
#include "ErrorDialog.hh"
#include "Environment.hh"

using std::string;

/**
 * The Constructor.
 */
SaveOptionsCmd::SaveOptionsCmd():
    EditorCommand(ProDeConstants::CMD_NAME_SAVE_OPTIONS) {
}


/**
 * The Destructor.
 */
SaveOptionsCmd::~SaveOptionsCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
SaveOptionsCmd::Do() {

    ProDeOptions* options = wxGetApp().options();
    ProDeOptionsSerializer writer;
    writer.setDestinationFile(Environment::confDirPath("") + "ProDe.conf");

    try {
        writer.writeOptions(*options);
        options->clearModified();
    } catch (Exception e) {
        ErrorDialog errorDialog(parentWindow(),
            WxConversion::toWxString(e.errorMessage()));
        errorDialog.ShowModal();
        return false;
    }
    return true;
}


/**
 * Returns id of this command.
 */
int
SaveOptionsCmd::id() const {
    return ProDeConstants::COMMAND_SAVE_OPTIONS;
}


/**
 * Creates and returns a new instance of this command.
 */
SaveOptionsCmd*
SaveOptionsCmd::create() const {
    return new SaveOptionsCmd();
}



/**
 * Returns short version of the command name.
 */
string
SaveOptionsCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_SAVE_OPTIONS;
}


/**
 * Returns path to the command's icon file.
 */
string
SaveOptionsCmd::icon() const {
    return ProDeConstants::CMD_ICON_SAVE_OPTIONS;
}


/**
 * This command is enabled when the options have been modified.
 *
 * @return True if the options have been modified, otherwise false.
 */
bool
SaveOptionsCmd::isEnabled() {
    return wxGetApp().options()->isModified();
}
