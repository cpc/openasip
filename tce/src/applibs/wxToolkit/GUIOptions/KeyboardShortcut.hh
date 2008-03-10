/**
 * @file KeyboardShortcut.hh
 *
 * Declaration of KeyboardShortcut class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_KEYBOARD_SHORTCUT_HH
#define TTA_KEYBOARD_SHORTCUT_HH

#include <string>

#include "Serializable.hh"
#include "ObjectState.hh"

/**
 * This class stores the keyboard buttons used in a keyboard
 * shortcut. It contains also the name of the action performed by the
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

    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
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
