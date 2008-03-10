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
