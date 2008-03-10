/**
 * @file GUIOptions.hh
 *
 * Declaration of class GUIOptions.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_GUI_OPTIONS_HH
#define TTA_GUI_OPTIONS_HH

#include <string>
#include <vector>

#include "Serializable.hh"
#include "KeyboardShortcut.hh"
#include "ToolbarButton.hh"

class CommandRegistry;
class wxToolBar;
class wxWindow;
class wxString;

/**
 * Represents the options of a GUI.
 * 
 * Base class for GUI options. This class provides handling of options common
 * to all GUIs. If a GUI needs application specific options, specialized
 * options class can be derived from this class. See design documentation for
 * details.
 * This class implements the Serializable interface because the options are
 * going to be stored in an xml configuration file.
 */
class GUIOptions : public Serializable {
public:

    /**
     * Layout of the buttons in the toolbar.
     */
    enum ToolbarLayout {
        TEXT, ///< Buttons contains only text.
        ICON, ///< Buttons contains only icon.
        BOTH  ///< Buttons contains text and icon.
    };

    GUIOptions(std::string name);
    GUIOptions(const GUIOptions& old);
    GUIOptions(const ObjectState* state)
        throw (ObjectStateLoadingException);

    virtual ~GUIOptions();

    bool hasFileName() const;
    std::string fileName() const;
    void setFileName(const std::string& fileName);

    virtual void validate() const
        throw (InvalidData);
 
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ObjectState* saveState() const;


    bool fullScreen() const;
    int windowWidth() const;
    int windowHeight() const;
    int xPosition() const;
    int yPosition() const;
    bool toolbarVisibility() const;
    ToolbarLayout toolbarLayout() const;
    
    void setFullScreen(bool fullScreen);
    void setWindowSize(int width, int height);
    void setWindowPosition(int x, int y);
    void setToolbarVisibility(bool visible);
    void setToolbarLayout(ToolbarLayout layout);

    void addKeyboardShortcut(KeyboardShortcut* shortcut);
    void addToolbarButton(ToolbarButton* button);
    void addSeparator(int position);

    void deleteKeyboardShortcut(KeyboardShortcut* shortcut);
    void deleteToolbarButton(ToolbarButton* button);
    void deleteSeparator(int position);

    KeyboardShortcut* keyboardShortcut(const std::string commandName) const;
    KeyboardShortcut* firstShortcut() const;
    KeyboardShortcut* nextShortcut() const;

    ToolbarButton* firstToolbarButton() const;
    ToolbarButton* nextToolbarButton() const;

    int firstSeparator() const;
    int nextSeparator() const;

    void clearModified();
    bool isModified() const;

    wxToolBar* createToolbar(
        wxWindow* parent,
        CommandRegistry& registry,
        const wxString& iconsPath);

    /// Toolbar separator name.
    static const std::string TOOLBAR_SEPARATOR;
    /// ObjectState name for the options.
    static const std::string OSNAME_OPTIONS;
    /// ObjectState attribute key for full screen feature.
    static const std::string OSKEY_FULL_SCREEN;
    /// ObjectState attribute key for window width.
    static const std::string OSKEY_WINDOW_WIDTH;
    /// ObjectState attribute key for window height.
    static const std::string OSKEY_WINDOW_HEIGHT;
    /// ObjectState attribute key for window x position.
    static const std::string OSKEY_X_POS;
    /// ObjectState attribute key for window y position.
    static const std::string OSKEY_Y_POS;
    /// ObjectState attribute key for toolbar visibility.
    static const std::string OSKEY_TOOLBAR_VISIBILITY;
    /// ObjectState attribute key for toolbar layout.
    static const std::string OSKEY_TOOLBAR_LAYOUT;
    /// ObjectState attribute value for text layout.
    static const std::string OSVALUE_TEXT;
    /// ObjectState attribute value for icon layout.
    static const std::string OSVALUE_ICON;
    /// ObjectState attribute value for text & icon layout.
    static const std::string OSVALUE_BOTH;
    /// ObjectState name for separator.
    static const std::string OSNAME_SEPARATOR;
    /// ObjectState attribute key for separator position.
    static const std::string OSKEY_POSITION;

protected:
    void deleteAllKeyboardShortcuts();
    void deleteAllToolbarButtons();

private:

    /// Table for the toolbar separator positions.
    typedef std::vector<int> SeparatorTable;
    /// Table for keyboard shortcuts.
    typedef std::vector<KeyboardShortcut*> KSTable;
    /// Table for toolbar buttons
    typedef std::vector<ToolbarButton*> TBTable;

    /// File name assigned to these options.
    std::string fileName_;
    /// If true, the application window will open in full screen mode. 
    bool fullScreen_;
    /// Default width of the window.
    int windowWidth_;
    /// Default height of the window.
    int windowHeight_;
    /// Default x position of left side of the application window.
    int xPosition_;
    /// Default y position of the upper side of the application window.
    int yPosition_;
    /// If true, the toolbar is visible.
    bool toolbarVisibility_;
    /// Layout of the toolbar.
    ToolbarLayout toolbarLayout_;
    /// Toolbar separators.
    SeparatorTable toolbarSeparators_;
    /// Keyboard shortcuts.
    KSTable keyboardShortcuts_;
    /// Toolbar buttons.
    TBTable toolbarButtons_;

    /// Iterator used in firstShortcut and nextShortcut functions.
    mutable KSTable::const_iterator ksIter_;
    /// Iterator used in firstToolbarButton and nextToolbarButton functions.
    mutable TBTable::const_iterator tbIter_;
    /// Iterator used in firstSeparator and nextSeparator functions.
    mutable SeparatorTable::const_iterator separatorIter_;

    /// Indicates whether the options are modified or not.
    bool modified_;
    /// Name of the options.
    std::string name_;
};

#endif
