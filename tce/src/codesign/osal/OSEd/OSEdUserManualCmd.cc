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
 * @file OSEdUserManualCmd.cc
 *
 * Definition of OSEdUserManualCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "OSEdUserManualCmd.hh"
#include "OSEdConstants.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "HelpBrowser.hh"
#include "OSEdTextGenerator.hh"
#include "WxConversion.hh"

using std::string;
using boost::format;

/**
 * Constructor.
 */
OSEdUserManualCmd::OSEdUserManualCmd() :
    GUICommand(OSEdConstants::CMD_NAME_USER_MANUAL, NULL) {
}

/**
 * Destructor.
 */
OSEdUserManualCmd::~OSEdUserManualCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdUserManualCmd::id() const {
    return OSEdConstants::CMD_USER_MANUAL;
}

/**
 * Creates new command.
 *
 * @return New command.
 */
GUICommand*
OSEdUserManualCmd::create() const {
    return new OSEdUserManualCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful.
 */
bool
OSEdUserManualCmd::Do() {

    string manualFile = userManual();
		
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    format fmt = texts.text(OSEdTextGenerator::TXT_USER_MANUAL_TITLE);

    HelpBrowser* browser = new HelpBrowser(
        WxConversion::toWxString(fmt.str()), 
        WxConversion::toWxString(manualFile), wxDefaultPosition,
        wxSize(800, 600));

    browser->Show(true);
    return true;
}

/**
 * Returns true if command is enabled.
 *
 * Command is enabled when user manual exists.
 *
 * @return True if command is enabled.
 */
bool
OSEdUserManualCmd::isEnabled() {
    string manualFile = userManual();
    return FileSystem::fileExists(manualFile);
}

/**
 * Returns the icon path.
 *
 * @return Empty string (icons not used).
 */
string
OSEdUserManualCmd::icon() const {
    return "";
}

/**
 * Returns the user manual location.
 *
 * @return User manual location.
 */
std::string
OSEdUserManualCmd::userManual() const {
    return Environment::manDirPath("OSALGUI") +
        FileSystem::DIRECTORY_SEPARATOR + "OSALGUI" +
        FileSystem::DIRECTORY_SEPARATOR + OSEdConstants::USER_MANUAL_NAME;
}
