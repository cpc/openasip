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
