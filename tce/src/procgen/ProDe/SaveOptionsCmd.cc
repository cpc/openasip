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
 * @file SaveOptionsCmd.cc
 *
 * Definition of SaveOptionsCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
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
