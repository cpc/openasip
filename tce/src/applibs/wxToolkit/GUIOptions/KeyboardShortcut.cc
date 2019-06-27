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
 * @file KeyboardShortcut.cc
 *
 * Implementation of KeyboardShortcut class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "KeyboardShortcut.hh"
#include "Conversion.hh"
#include "ObjectState.hh"

using std::string;

// initialization of static data members
const string KeyboardShortcut::OSNAME_KEYBOARD_SHORTCUT = "keybsc";
const string KeyboardShortcut::OSKEY_ACTION = "action";
const string KeyboardShortcut::OSKEY_FKEY = "fkey";
const string KeyboardShortcut::OSKEY_CTRL = "ctrl";
const string KeyboardShortcut::OSKEY_ALT = "alt";
const string KeyboardShortcut::OSKEY_KEY = "key";

/**
 * Constructor.
 *
 * @param action Name of the action performed by the key combination.
 * @param fKey Number of the function key used in the shortcut, if the value
 *             is not between 1 and 12 it is ignored and any function key
 *             won't be used.
 * @param ctrl True if the Ctrl button is used.
 * @param alt True if the Alt button is used.
 * @param key Character used in the key combination, NUL character if no
 *            character is used. If delete is used, apply ascii 127.
 */
KeyboardShortcut::KeyboardShortcut(
    const std::string& action,
    int fKey,
    bool ctrl,
    bool alt,
    char key):
    action_(action), fKey_(fKey), ctrl_(ctrl), alt_(alt), key_(key) {

    // validity checks
    if (key < '0' && key != 0) {
        assert(false);
    }
    if (key > '9' && key < 'A') {
        assert(false);
    }
    if (key > 'Z' && key < 'a') {
        assert(false);
    }
    if (key > 'z' && key != 127) {
        assert(false);
    }
    
    // if key is lower case letter, convert it to lower case
    if (key_ >= 'a' && key_ <= 'z') {
        key_ -= 32;
    }

    // validity checks
    if (this->fKey() != 0) {
        assert(this->key() == 0);
    }
    if (this->key() != 0) {
        assert(this->fKey() == 0);
        assert(ctrl_ == true || alt_ == true);
    }
}


/**
 * Constructor.
 *
 * Loads the state of the object from the given ObjectState object.
 *
 * @param state ObjectState from which the state is loaded.
 */
KeyboardShortcut::KeyboardShortcut(const ObjectState* state) {
    loadState(state);
}


/**
 * Copy constructor.
 */
KeyboardShortcut::KeyboardShortcut(
    const KeyboardShortcut& old): Serializable() {
    
    action_ = old.action_;
    fKey_ = old.fKey_;
    ctrl_ = old.ctrl_;
    alt_ = old.alt_;
    key_ = old.key_;
}

    
/**
 * Destructor.
 */
KeyboardShortcut::~KeyboardShortcut() {
}


/**
 * Returns the name of the action performed by this key combination.
 *
 * @return Name of the action performed by this key combination.
 */
std::string
KeyboardShortcut::action() const {
    return action_;
}


/**
 * Returns the number of the function key used in the key combination.
 *
 * @return Number of the function key used in the key combination, 0 if
 *         no function key is used.
 */
int 
KeyboardShortcut::fKey() const {
    if (fKey_ < 1 || fKey_ > 12) {
        return 0;
    } else {
        return fKey_;
    }
}


/**
 * Returns true if Ctrl button is used in the key combination.
 *
 * @return True if Ctrl button is used in the key combination, otherwise
 *         false.
 */
bool 
KeyboardShortcut::ctrl() const {
    return ctrl_;
}


/**
 * Returns true if Alt button is used in the key combination.
 *
 * @return True if Alt button is used in the key combination, otherwise
 *         false.
 */
bool 
KeyboardShortcut::alt() const {
    return alt_;
}


/**
 * Returns the character used in the key combination or NUL character if no
 * character is used.
 *
 * @return The character used in the key combination or NUL character if no
 *         character is used.
 */
char 
KeyboardShortcut::key() const {
    return key_;
}


/**
 * Sets control button state of the shortcut.
 *
 * @param ctrl Ctrl-button state of the shortcut.
 */
void
KeyboardShortcut::setCtrl(bool ctrl) {
    ctrl_ = ctrl;
}


/**
 * Sets alt-button state of the shortcut.
 *
 * @param alt Alt-button state of the shortcut.
 */
void
KeyboardShortcut::setAlt(bool alt) {
    alt_ = alt;
}

/**
 * Sets the character key of the keyboard shortcut.
 *
 * @param key Character key of the shortcut.
 */
void
KeyboardShortcut::setKey(char key) {
    key_ = key;
}

/**
 * Sets the function key of the keyboard shortcut. Set as 0 if the shortcut
 * doesn't use function key.
 *
 * @param key Function key of the shortcut.
 */
void
KeyboardShortcut::setFKey(int fKey) {
    if (fKey < 0 || fKey > 12) {
        fKey = 0;
    } else {
        fKey_ = fKey;
    }
}

/**
 * Returns true if the given keyboard shortcut has the same key combination
 * as this.
 *
 * @param sc Keyboard shortcut.
 * @return True if the given keyboard shortcut has the same key combination
 *         as this.
 */
bool
KeyboardShortcut::equals(const KeyboardShortcut& sc) const {
    if (ctrl_ == sc.ctrl_ && alt_ == sc.alt_ && key_ == sc.key_ &&
        fKey() == sc.fKey()) {
        return true;
    } else {
        return false;
    }
}


/**
 * Loads the state of the object from the given ObjectState object.
 *
 * @param state ObjectState from which the state is loaded.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
KeyboardShortcut::loadState(const ObjectState* state) {
    const string procName = "KeyboardShortcut::loadState";

    if (state->name() != OSNAME_KEYBOARD_SHORTCUT) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
        action_ = state->stringAttribute(OSKEY_ACTION);

        if (state->hasAttribute(OSKEY_FKEY)) {
            fKey_ = state->intAttribute(OSKEY_FKEY);
        } else {
            fKey_ = 0;
        }

        ctrl_ = state->intAttribute(OSKEY_CTRL);
        alt_ = state->intAttribute(OSKEY_ALT);
        
        if (state->hasAttribute(OSKEY_KEY)) {
            key_ = state->intAttribute(OSKEY_KEY);
            
            // convert to upper case if lower case
            if (key_ >= 'a' && key_ <= 'z') {
                key_ -= 32;
            }
            
        } else {
            key_ = 0;
        }

    } catch (...) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }
}

/**
 * Creates an ObjectState object and saves the state of the object into it.
 *
 * @return The created ObjectState object.
 */
ObjectState*
KeyboardShortcut::saveState() const {
    
    ObjectState* state = new ObjectState(OSNAME_KEYBOARD_SHORTCUT);

    state->setAttribute(OSKEY_ACTION, action_);

    // if function key is used
    if (fKey() != 0) {
        state->setAttribute(OSKEY_FKEY, fKey());
    }

    state->setAttribute(OSKEY_CTRL, ctrl_);
    state->setAttribute(OSKEY_ALT, alt_);
    
    // if key is not nul character
    if (key_ != 0) {
        state->setAttribute(OSKEY_KEY, key_);
    }

    return state;
}
