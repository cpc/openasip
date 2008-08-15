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
 * @file GUIOptions.cc
 *
 * Implementation of class GUIOptions.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include "Application.hh"
#include "GUIOptions.hh"
#include "Conversion.hh"
#include "SequenceTools.hh"
#include "WxConversion.hh"
#include "CommandRegistry.hh"
#include "GUICommand.hh"
#include "FileSystem.hh"

using std::string;

// initialization of static data members
const string GUIOptions::OSNAME_OPTIONS = "options";
const string GUIOptions::OSKEY_FULL_SCREEN = "fscreen";
const string GUIOptions::OSKEY_WINDOW_WIDTH = "wwidth";
const string GUIOptions::OSKEY_WINDOW_HEIGHT = "wheight"; 
const string GUIOptions::OSKEY_X_POS = "xpos";
const string GUIOptions::OSKEY_Y_POS = "ypos"; 
const string GUIOptions::OSKEY_TOOLBAR_VISIBILITY = "tbvisib";
const string GUIOptions::OSKEY_TOOLBAR_LAYOUT = "tblayout";
const string GUIOptions::OSVALUE_TEXT = "text";
const string GUIOptions::OSVALUE_ICON = "icon";
const string GUIOptions::OSVALUE_BOTH = "both"; 
const string GUIOptions::OSNAME_SEPARATOR = "separator";
const string GUIOptions::OSKEY_POSITION = "pos"; 

const string GUIOptions::TOOLBAR_SEPARATOR = "-- separator --";

#include <iostream>

/**
 * Constructor.
 */
GUIOptions::GUIOptions(std::string name) : 
    fullScreen_(false),  windowWidth_(400), windowHeight_(400), xPosition_(0),
    yPosition_(0), toolbarVisibility_(false), toolbarLayout_(BOTH),
    modified_(false), name_(name) {
}


/**
 * Constructor.
 *
 * Loads the state from the given ObjectState tree.
 * 
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If the given ObjectState tree is
 *            invalid.
 */
GUIOptions::GUIOptions(const ObjectState* state) 
    throw (ObjectStateLoadingException) : 
    modified_(false) {

    loadState(state);
}


/**
 * Copy constructor.
 *
 * This constructor creates identical options with the given options class.
 */
GUIOptions::GUIOptions(const GUIOptions& old): Serializable() {
    
    fullScreen_ = old.fullScreen_;
    windowWidth_ = old.windowWidth_;
    windowHeight_ = old.windowHeight_;
    xPosition_ = old.xPosition_;
    yPosition_ = old.yPosition_;
    toolbarVisibility_ = old.toolbarVisibility_;
    toolbarLayout_ = old.toolbarLayout_;
    toolbarSeparators_ = old.toolbarSeparators_;
    modified_ = old.modified_;

    // deep copy keyboard shortcuts
    for (KSTable::const_iterator iter = old.keyboardShortcuts_.begin();
         iter != old.keyboardShortcuts_.end(); iter++) {

        keyboardShortcuts_.push_back(new KeyboardShortcut(**iter));
    }

    // deep copy toolbar buttons
    for (TBTable::const_iterator iter = old.toolbarButtons_.begin();
         iter != old.toolbarButtons_.end(); iter++) {
        toolbarButtons_.push_back(new ToolbarButton(**iter));
    }
}


/**
 * Destructor.
 */
GUIOptions::~GUIOptions() {
    deleteAllToolbarButtons();
    deleteAllKeyboardShortcuts();
}


/**
 * Returns true, if the application window opens in full screen mode as 
 * default, otherwise false.
 *
 * @return True, if the application window opens in full screen mode as 
 *         default, otherwise false.
 */
bool 
GUIOptions::fullScreen() const {
    return fullScreen_;
}


/**
 * Returns the default width of the application window.
 *
 * @return The default width of the application window.
 */
int 
GUIOptions::windowWidth() const {
    return windowWidth_;
}


/**
 * Returns the default height of the application window.
 *
 * @return The default height of the application window.
 */
int 
GUIOptions::windowHeight() const {
    return windowHeight_;
}


/**
 * Returns the default x position of left side of the application window.
 *
 * @return The default x position of left side of the application window.
 */
int 
GUIOptions::xPosition() const {
    return xPosition_;
}


/**
 * Returns the default y position of upper side of the application window.
 *
 * @return The default y position of upper side of the application window.
 */
int 
GUIOptions::yPosition() const {
    return yPosition_;
}


/**
 * Returns true if the toolbar is visible by default, otherwise false.
 *
 * @return True if the toolbar is visible by default, otherwise false.
 */
bool 
GUIOptions::toolbarVisibility() const {
    return toolbarVisibility_;
}


/**
 * Returns the layout mode of the toolbar.
 *
 * @return Layout mode of the toolbar.
 */
GUIOptions::ToolbarLayout 
GUIOptions::toolbarLayout() const {
    return toolbarLayout_;
}


/**
 * Sets or unsets the default full screen mode of the application window.
 *
 * @param fullScreen If true, the full screen mode is set, otherwise unset.
 */
void 
GUIOptions::setFullScreen(bool fullScreen) {
    fullScreen_ = fullScreen;
    modified_ = true;
}


/**
 * Sets the default application window size.
 *
 * @param width Pixel width of the window.
 * @param height Pixel height of the window.
 */ 
void 
GUIOptions::setWindowSize(int width, int height) {
    windowWidth_ = width;
    windowHeight_ = height;
    modified_ = true;
}


/**
 * Sets the default position of the window.
 *
 * @param x X-coordinate of left side of the application window.
 * @param y Y-coordinate of upper side of the application window.
 */
void 
GUIOptions::setWindowPosition(int x, int y) {
    xPosition_ = x;
    yPosition_ = y;
    modified_ = true;
}


/**
 * Sets the visibility mode of the toolbar.
 *
 * @param visible If true, the toolbar is visible, otherwise it is hidden.
 */
void 
GUIOptions::setToolbarVisibility(bool visible) {
    toolbarVisibility_ = visible;
    modified_ = true;
}


/**
 * Sets the layout of the toolbar buttons.
 *
 * @param layout Layout of the toolbar buttons.
 */
void 
GUIOptions::setToolbarLayout(ToolbarLayout layout) {
    toolbarLayout_ = layout;
    modified_ = true;
}
    

/**
 * Adds a keyboard shortcut.
 *
 * @param shortcut Keyboard shortcut to be added.
 */ 
void 
GUIOptions::addKeyboardShortcut(KeyboardShortcut* shortcut) {
    keyboardShortcuts_.push_back(shortcut);
    modified_ = true;
}


/**
 * Adds the given button to the toolbar.
 *
 * @param button Button to be added.
 */
void 
GUIOptions::addToolbarButton(ToolbarButton* button) {
    toolbarButtons_.push_back(button);
    modified_ = true;
}


/**
 * Adds a separator to the toolbar to the given position.
 *
 * @param position Number of the slot left side of which the separator is 
 *                 drawn.
 */
void 
GUIOptions::addSeparator(int position) {
    toolbarSeparators_.push_back(position);
    modified_ = true;
}
    

/**
 * Deletes the given keyboard shortcut.
 *
 * @param shortcut The keyboard shortcut to be deleted.
 */
void 
GUIOptions::deleteKeyboardShortcut(KeyboardShortcut* shortcut) {
    KSTable::iterator iter = keyboardShortcuts_.begin();
    while (iter != keyboardShortcuts_.end()) {
        if (*iter == shortcut) {
            keyboardShortcuts_.erase(iter);
            delete shortcut;
            modified_ = true;
            return;
        }
        iter++;
    }
}


/**
 * Deletes the given toolbar button.
 *
 * @param button The toolbar button to be deleted.
 */
void 
GUIOptions::deleteToolbarButton(ToolbarButton* button) {
    TBTable::iterator iter = toolbarButtons_.begin();
    while (iter != toolbarButtons_.end()) {
        if (*iter == button) {
            toolbarButtons_.erase(iter);
            delete button;
            modified_ = true;
            return;
        }
        iter++;
    }
}


/**
 * Deletes a separator from the given position.
 *
 * @param position The slot position left side of which the separator is 
 *                 deleted.
 */
void 
GUIOptions::deleteSeparator(int position) {
    SeparatorTable::iterator iter = toolbarSeparators_.begin();
    while (iter != toolbarSeparators_.end()) {
        if (*iter == position) {
            toolbarSeparators_.erase(iter);
            modified_ = true;
            return;
        }
        iter++;
    }
}

/**
 * Returns keyboard shortcut for the command with given name.
 *
 * @param commandName Name of the command to search shortcut for.
 * @return NULL if a shortcut doesn't exist for the command.
 */
KeyboardShortcut*
GUIOptions::keyboardShortcut(const std::string commandName) const {

    KSTable::const_iterator iter = keyboardShortcuts_.begin();
    
    for (; iter != keyboardShortcuts_.end(); iter++) {
        if ((*iter)->action() == commandName) {
            return *iter;
        }
    }

    return NULL;
}

/**
 * Returns the first keyboard shortcut. If there are no keyboard shortcuts,
 * returns null pointer.
 *
 * @return The first keyboard shortcut.
 */   
KeyboardShortcut* 
GUIOptions::firstShortcut() const {
    ksIter_ = keyboardShortcuts_.begin();
    if (ksIter_ != keyboardShortcuts_.end()) {
        return *ksIter_;
    } else {
        return NULL;
    }
}


/**
 * Returns the always the next keyboard shortcut after firstShortcut has been
 * called. Returns null pointer after the last keyboard shortcut. 
 *
 * @return The next keyboard shortcut.
 */
KeyboardShortcut* 
GUIOptions::nextShortcut() const {
    assert(ksIter_ != keyboardShortcuts_.end());
    ksIter_++;
    if (ksIter_ != keyboardShortcuts_.end()) {
        return *ksIter_;
    } else {
        return NULL;
    }
}


/**
 * Returns the first toolbar button. Returns null pointer if there are no
 * toolbar buttons.
 *
 * @return The first toolbar button.
 */
ToolbarButton* 
GUIOptions::firstToolbarButton() const {
    tbIter_ = toolbarButtons_.begin();
    if (tbIter_ != toolbarButtons_.end()) {
        return *tbIter_;
    } else {
        return NULL;
    }
}


/**
 * Returns always the next toolbar button after the firstToolbarButton has
 * been called.
 *
 * @return The next toolbar button.
 */
ToolbarButton* 
GUIOptions::nextToolbarButton() const {
    assert(tbIter_ != toolbarButtons_.end());
    tbIter_++;
    if (tbIter_ != toolbarButtons_.end()) {
        return *tbIter_;
    } else {
        return NULL;
    }
}


/**
 * Returns position of the first toolbar separator. Position is the position
 * of the slot left side of which the separator is. If there are no 
 * separators, returns -1.
 *
 * @return Position of the first toolbar separator.
 */
int 
GUIOptions::firstSeparator() const {
    separatorIter_ = toolbarSeparators_.begin();
    if (separatorIter_ != toolbarSeparators_.end()) {
        return *separatorIter_;
    } else {
        return -1;
    }
}


/**
 * Returns always position of the next toolbar separator. If there are no
 * more separators, returns -1.
 *
 * @return Position of the next toolbar separator.
 */
int 
GUIOptions::nextSeparator() const {
    assert(separatorIter_ != toolbarSeparators_.end());
    separatorIter_++;
    if (separatorIter_ != toolbarSeparators_.end()) {
        return *separatorIter_;
    } else {
        return -1;
    }
}


/**
 * Clears the modified flag.
 *
 * The isModified method returns false after calling this method.
 */
void
GUIOptions::clearModified() {
    modified_ = false;
}


/**
 * Returns true if the options are modified.
 *
 * @return True if the options are modified.
 */
bool
GUIOptions::isModified() const {
    return modified_;
}


/**
 * Validates the state of the options.
 *
 * Checks that there is no gaps in toolbar item positions and no two items
 * at the same position. Additionally checks that there is no same keyboard
 * shortcuts for several actions and no two keyboard shortcuts for the same
 * action.
 *
 * @exception InvalidData If the options are in invalid state.
 */
void
GUIOptions::validate() const
    throw (InvalidData) {

    const string procName = "GUIOptions::validate";
    int buttons = toolbarButtons_.size();
    int separators = toolbarSeparators_.size();

    // check that there is no gaps in toolbar item positions
    for (int i = 0; i < buttons + separators; i++) {
        bool buttonfound = false;
        bool separatorfound = false;
        ToolbarButton* button = firstToolbarButton();
        while (button != NULL) {
            if (button->slot() == i) {
                if (buttonfound) {
                    string errorMsg = "There is at least two toolbar items ";
                    errorMsg += "at position \'" + Conversion::toString(i) +
                        "\'.";
                    throw InvalidData(__FILE__, __LINE__, procName, 
                                        errorMsg);
                }
                buttonfound = true;
            }
            button = nextToolbarButton();
        }
        
        for (int sepIndex = 0; sepIndex < separators; sepIndex++) {
            if (toolbarSeparators_[sepIndex] == i) {
                if (separatorfound || buttonfound) {
                    string errorMsg = "There is at least two toolbar items ";
                    errorMsg += "at position \'" + Conversion::toString(i) +
                        "\'.";
                    throw InvalidData(__FILE__, __LINE__, procName, 
                                        errorMsg);
                }
                separatorfound = true;
            }
        }

        if (!buttonfound && !separatorfound) {
            string errorMsg = "There is no separator or button at slot "
                "position \'" + Conversion::toString(i) + "\'.";
            throw InvalidData(__FILE__, __LINE__, procName, errorMsg);
        }

    }

    // check that there is no same keyboard shortcuts for several actions and
    // no two keyboard shortcuts for same action
    KSTable::const_iterator ksIter = keyboardShortcuts_.begin();
    while (ksIter != keyboardShortcuts_.end()) {
        KeyboardShortcut* observable = *ksIter;
        string action = observable->action();
        KSTable::const_iterator seekerIter = keyboardShortcuts_.begin();
        
        while (seekerIter != keyboardShortcuts_.end()) {
            KeyboardShortcut* sc = *seekerIter;
            if (action == sc->action() && sc != observable) {
                string errorMsg = "Action \'" + action + "\' is assigned for"
                    + " two or more keyboard shortcuts.";
                throw InvalidData(__FILE__, __LINE__, procName, errorMsg);
            }
            if (observable->equals(*sc) && observable != sc) {
                string errorMsg = "Same keyboard shortcut is used for \'" +
                    observable->action() + "\' and \'" + sc->action() +
                    "\' actions.";
                throw InvalidData(__FILE__, __LINE__, procName, errorMsg);
            }
            seekerIter++;
        }
        ksIter++;
    }
}

/**
 * Checks whether these options has file name assigned.
 *
 * @return True if these options has file name assigned, otherwise false.
 */
bool
GUIOptions::hasFileName() const {
    if (fileName_ == "") {
        return false;
    }
    return true;
}


/**
 * Returns the file name assigned to these options.
 *
 * @return The file name assigned to these options.
 */
string
GUIOptions::fileName() const {
    return fileName_;
}


/**
 * Sets the file name assigned into these options.
 *
 * @param fileName The file name assigned into these options.
 */
void
GUIOptions::setFileName(const std::string& fileName) {
    fileName_ = fileName;
}


/**
 * Loads the state of the object from the given ObjectState object.
 *
 * @param state ObjectState from which the state is loaded.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
GUIOptions::loadState(const ObjectState* state) 
    throw (ObjectStateLoadingException) {

    const string procName = "GUIOptions::loadState";

    if (state->name() != OSNAME_OPTIONS) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
        fullScreen_ = state->intAttribute(OSKEY_FULL_SCREEN);
        windowWidth_ = state->intAttribute(OSKEY_WINDOW_WIDTH);
        windowHeight_ = state->intAttribute(OSKEY_WINDOW_HEIGHT);
        xPosition_ = state->intAttribute(OSKEY_X_POS);
        yPosition_ = state->intAttribute(OSKEY_Y_POS);
        toolbarVisibility_ = state->intAttribute(OSKEY_TOOLBAR_VISIBILITY);
    
        string layout = state->stringAttribute(OSKEY_TOOLBAR_LAYOUT);
        if (layout == OSVALUE_TEXT) {
            toolbarLayout_ = TEXT;
        } else if (layout == OSVALUE_ICON) {
            toolbarLayout_ = ICON;
        } else if (layout == OSVALUE_BOTH) {
            toolbarLayout_ = BOTH;
        } else {
            throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
        }

        // get toolbar separators, keyboard shortcuts and toolbar buttons
        for (int i = 0; i < state->childCount(); i++) {
            ObjectState* child = state->child(i);
            if (child->name() == OSNAME_SEPARATOR) {
                addSeparator(child->intAttribute(OSKEY_POSITION));
                
            } else if (child->name() ==
                       KeyboardShortcut::OSNAME_KEYBOARD_SHORTCUT) {
                addKeyboardShortcut(new KeyboardShortcut(child));
            } else if (child->name() == ToolbarButton::OSNAME_TOOLBAR_BUTTON) {
                addToolbarButton(new ToolbarButton(child));
            } else {
                throw ObjectStateLoadingException(__FILE__, __LINE__, 
                                                  procName);
            }
        }
    } catch (...) {
        deleteAllKeyboardShortcuts();
        deleteAllToolbarButtons();
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }
}


/**
 * Creates an ObjectState object and saves the state of the object into it.
 *
 * @return The created ObjectState object.
 */
ObjectState*
GUIOptions::saveState() const {

    ObjectState* state = new ObjectState(OSNAME_OPTIONS);

    state->setAttribute(OSKEY_FULL_SCREEN, fullScreen_);
    state->setAttribute(OSKEY_WINDOW_WIDTH, windowWidth_);
    state->setAttribute(OSKEY_WINDOW_HEIGHT, windowHeight_);
    state->setAttribute(OSKEY_X_POS, xPosition_);
    state->setAttribute(OSKEY_Y_POS, yPosition_);
    state->setAttribute(OSKEY_TOOLBAR_VISIBILITY, toolbarVisibility_);

    if (toolbarLayout_ == TEXT) {
        state->setAttribute(OSKEY_TOOLBAR_LAYOUT, OSVALUE_TEXT);
    } else if (toolbarLayout_ == ICON) {
        state->setAttribute(OSKEY_TOOLBAR_LAYOUT, OSVALUE_ICON);
    } else {
        state->setAttribute(OSKEY_TOOLBAR_LAYOUT, OSVALUE_BOTH);
    }

    // add toolbar separators
    for (int i = 0; i < (int)toolbarSeparators_.size(); i++) {
        ObjectState* separator = new ObjectState(OSNAME_SEPARATOR);
        state->addChild(separator);
        separator->setAttribute(OSKEY_POSITION, toolbarSeparators_[i]);
    }

    // add keyboard shortcuts
    for (int i = 0; i < (int)keyboardShortcuts_.size(); i++) {
        KeyboardShortcut* sc = keyboardShortcuts_[i];
        state->addChild(sc->saveState());
    }

    // add toolbar buttons
    for (int i = 0; i < (int)toolbarButtons_.size(); i++) {
        ToolbarButton* button = toolbarButtons_[i];
        state->addChild(button->saveState());
    }

    return state;
}

/**
 * Deletes all the keyboard shortcuts.
 */
void
GUIOptions::deleteAllKeyboardShortcuts() {
    SequenceTools::deleteAllItems(keyboardShortcuts_);
}


/**
 * Deletes all the toolbar buttons.
 */
void
GUIOptions::deleteAllToolbarButtons() {
    SequenceTools::deleteAllItems(toolbarButtons_);
}


/**
 * Creates a toolbar defined in the options.
 *
 * @param parent Parent window of the created toolbar.
 * @param registry Command registry containing the toolbar commands.
 * @param iconPath Path of the toolbar icon files.
 * @return A new toolbar corresponding to the one defined in the options.
 */
wxToolBar*
GUIOptions::createToolbar(
    wxWindow* parent, CommandRegistry& registry,
    const wxString& iconsPath) {

    int layout = toolbarLayout();
    long style = 0;

    // check toolbar layout from options
    if (layout == GUIOptions::BOTH) {
        style = wxTB_HORIZONTAL | wxNO_BORDER | wxTB_TEXT;
    } else if (layout == GUIOptions::ICON) {
        style = wxTB_HORIZONTAL | wxNO_BORDER;
    } else if (layout == GUIOptions::TEXT) {
        style = wxTB_HORIZONTAL | wxNO_BORDER | wxTB_TEXT | wxTB_NOICONS;
    } else {
        assert(false);
    }

    // create a new toolbar
    wxToolBar* toolbar = new wxToolBar(
        parent, -1, wxDefaultPosition, wxDefaultSize, style);

    bool found = true;
    int slot = 0;
    wxBitmap icon;

    // Add buttons and separators for each slot, until a slot without
    // button or separator is found.
    while (found) {

        found = false;

        // check if a toolbar button exists for the slot
        ToolbarButton* tool = firstToolbarButton();
        while (tool != NULL) {
            if (tool->slot() == slot) {
                // button found for the slot, add it
                found = true;

                wxString iconPath = iconsPath;
	        iconPath.Append(WxConversion::toWxString(
		    FileSystem::DIRECTORY_SEPARATOR));
                iconPath.Append(
                    WxConversion::toWxString(
                        registry.commandIcon(tool->action())));

                if (!icon.LoadFile(iconPath, wxBITMAP_TYPE_PNG)) {
                    std::cerr << "Toolbar icon file "
                              << WxConversion::toString(iconPath)
                              << " not found!"
                              << std::endl;

                    icon.Create(32, 32);
                }

                toolbar->AddTool(registry.commandId(tool->action()),
                                  WxConversion::toWxString(
                                  registry.commandShortName(tool->action())),
                                  icon, wxNullBitmap, wxITEM_NORMAL,
                                  WxConversion::toWxString(
                                  tool->action()),
                                  WxConversion::toWxString(
                                  tool->action()));
                found = true;
                // disable button if command is not executable at the moment
                if (!registry.isEnabled(tool->action())) {
                    toolbar->EnableTool(registry.commandId(tool->action()),
                                         false);
                }
            }
            tool = nextToolbarButton();
        }


        // check if a separator exists for the slot
        int separator = firstSeparator();
        while (separator != -1) {
            if (separator == slot) {
                // separator found for the slot, add it
                toolbar->AddSeparator();
                found = true;
            }
            separator = nextSeparator();
        }
        slot++;
    }

    toolbar->Realize();
    return toolbar;
}
