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
 * @file KeyboardShortcut.hh
 *
 * Declaration of KeyboardShortcut class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_KEYBOARD_SHORTCUT_HH
#define TTA_KEYBOARD_SHORTCUT_HH

#include <string>

#include "Serializable.hh"
#include "Exception.hh"

/**
 * This class stores the keyboard buttons used in a keyboard
 * shortcut. 
 *
 * It contains also the name of the action performed by the
 * key combination. This class implements the Serializable interface
 * because keyboard shortcuts are serialized into the configuration
 * file.
 */  
class KeyboardShortcut : public Serializable {
public:
    /// ObjectState name for keyboard shortcut.
    static const std::string OSNAME_KEYBOARD_SHORTCUT;
    /// ObjectState attribute key for action name.
    static const std::string OSKEY_ACTION;
    /// ObjectState attribute key for function key number.
    static const std::string OSKEY_FKEY;
    /// ObjectState attribute key for ctrl key.
    static const std::string OSKEY_CTRL;
    /// ObjectState attribute key for alt key.
    static const std::string OSKEY_ALT;
    /// ObjectState attribute key for normal letter or number key.
    static const std::string OSKEY_KEY;

    KeyboardShortcut(
        const std::string& action,
        int fKey,
        bool ctrl,
        bool alt,
        char key);
    KeyboardShortcut(const ObjectState* state);
    KeyboardShortcut(const KeyboardShortcut& old);
    virtual ~KeyboardShortcut();

    std::string action() const;
    int fKey() const;
    bool ctrl() const;
    bool alt() const;
    char key() const;

    void setFKey(int fKey);
    void setCtrl(bool ctrl);
    void setAlt(bool alt);
    void setKey(char key);

    bool equals(const KeyboardShortcut& sc) const;

    void loadState(const ObjectState* state);
    ObjectState* saveState() const;

private:
    /// Name of the action which is performed by the key combination.
    std::string action_;
    /// Number of the function key if it is used, 0 otherwise
    int fKey_;
    /// True if Ctrl button is used in the key combination.
    bool ctrl_;
    /// True if Alt button is used in the key combination.
    bool alt_;
    /**
     * ASCII character of the button used in the key combination, if the
     * key is not used the value is a NUL character (ASCII 0).
     */
    char key_;

    /// Assigment forbidden.
    KeyboardShortcut& operator=(const KeyboardShortcut);
};

#endif
