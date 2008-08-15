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
 * @file GUITextGenerator.hh
 *
 * Declaration of the GUITextGenerator class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@.cs.tut.fi)
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
