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
 * @file GUITextGenerator.cc
 *
 * Implementation of the GUITextGenerator class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include "GUITextGenerator.hh"
#include "WxConversion.hh"

GUITextGenerator* GUITextGenerator::instance_ = NULL;

/**
 * The Constructor.
 *
 * Initializes string templates to the text generator.
 */
GUITextGenerator::GUITextGenerator() {
    addText(TXT_BUTTON_ADD, "Add");
    addText(TXT_BUTTON_ADD_DIALOG, "Add...");
    addText(TXT_BUTTON_CANCEL, "&Cancel");
    addText(TXT_BUTTON_DELETE, "Delete");
    addText(TXT_BUTTON_EDIT, "Edit");
    addText(TXT_BUTTON_EDIT_DIALOG, "Edit...");
    addText(TXT_BUTTON_HELP, "&Help");
    addText(TXT_BUTTON_OK, "&OK");
}


/**
 * The Destructor.
 */
GUITextGenerator::~GUITextGenerator() {
}


/**
 * Returns the only instance of this class.
 *
 * @return The singleton instance of GUITextGenerator class.
 */
GUITextGenerator*
GUITextGenerator::instance() {
    if (instance_ == NULL) {
        instance_ = new GUITextGenerator();
    }
    return instance_;
}


/**
 * Deletes the instance of this command if one exists.
 */
void
GUITextGenerator::destroy() {
    if (instance_ != NULL) {
        delete instance_;
        instance_ = NULL;
    }
}
