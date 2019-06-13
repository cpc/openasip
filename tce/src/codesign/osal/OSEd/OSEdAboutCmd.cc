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
 * @file OSEdAboutCmd.cc
 *
 * Definition of OSEdAboutCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "OSEdAboutCmd.hh"
#include "OSEdConstants.hh"
#include "InformationDialog.hh"
#include "WxConversion.hh"
#include "OSEdTextGenerator.hh"
#include "OSEdAboutDialog.hh"

using boost::format;
using std::string;

/**
 * Constructor.
 */
OSEdAboutCmd::OSEdAboutCmd() : 
    GUICommand(OSEdConstants::CMD_NAME_ABOUT, NULL) {
}

/**
 * Destructor.
 */
OSEdAboutCmd::~OSEdAboutCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdAboutCmd::id() const {
    return OSEdConstants::CMD_ABOUT;
}

/**
 * Creates a new command.
 *
 * @return The created command.
 */
GUICommand*
OSEdAboutCmd::create() const {
    return new OSEdAboutCmd();
}

/**
 * Executes the command.
 *
 * @return True.
 */
bool
OSEdAboutCmd::Do() {
    OSEdAboutDialog about(parentWindow());
    about.ShowModal();
    return true;
}

/**
 * Returns true, if command is enabled.
 *
 * @return Always true.
 */
bool
OSEdAboutCmd::isEnabled() {
    return true;
}

/**
 * Returns the icon path.
 *
 * @return Empty string (no icons used).
 */
string
OSEdAboutCmd::icon() const {
    return "";
}
