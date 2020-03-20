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
 * @file GUITextGenerator.cc
 *
 * Implementation of the GUITextGenerator class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
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
