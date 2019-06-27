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
 * @file GUITextGenerator.hh
 *
 * Declaration of the GUITextGenerator class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-.cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_GUI_TEXT_GENERATOR_HH
#define TTA_GUI_TEXT_GENERATOR_HH

#include <wx/string.h>
#include "TextGenerator.hh"

/**
 * Provides common texts for tce graphical user interfaces.
 *
 * The class is implemented using the 'singleton' design pattern.
 * Only one instance of this class is created, and the instance
 * can be accessed using the instance() method.
 */
class GUITextGenerator : public Texts::TextGenerator {
public:
    static GUITextGenerator* instance();
    static void destroy();

    /// Ids for the generated texts.
    enum {
        TXT_BUTTON_ADD = Texts::LAST__, ///< Label for an add button.
        TXT_BUTTON_ADD_DIALOG,  ///< Label for add button (with trailing ...).
        TXT_BUTTON_CANCEL,      ///< Label for cancel button.
        TXT_BUTTON_DELETE,      ///< Label for delete button.
        TXT_BUTTON_EDIT,        ///< Label for edit button.
        TXT_BUTTON_EDIT_DIALOG, ///< Label for edit button (with trailing ...).
        TXT_BUTTON_OK,          ///< Label for OK button.
        TXT_BUTTON_HELP         ///< Label for help button.
    };

private:
    GUITextGenerator();
    ~GUITextGenerator();
    static GUITextGenerator* instance_;
};

#endif
