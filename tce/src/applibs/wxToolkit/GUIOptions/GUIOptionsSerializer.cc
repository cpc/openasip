/**
 * @file GUIOptionsSerializer.cc
 *
 * Implementation of GUIOptionsSerializer class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "GUIOptionsSerializer.hh"
#include "KeyboardShortcut.hh"
#include "Conversion.hh"

using std::string;

/// Name of the element containing window declaration.
const string WINDOW = "window";
/// Name of the full-screen element inside window element.
const string WI_FULLSCREEN = "full-screen";
/// Name of the element declaring window width.
const string WI_WIDTH = "width";
/// Name of the element declaring window height.
const string WI_HEIGHT = "height";
/// Name of the element declaring x position of the window.
const string WI_X_POSITION = "x-position";
/// Name of the element declaring y position of the window.
const string WI_Y_POSITION = "y-position";
/// Name of the element containing toolbar declaration.
const string TOOLBAR = "toolbar";
/// Name of the element containing toolbar layout declaration.
const string TB_LAYOUT = "layout";
/// Name of the text element inside layout element.
const string TB_LA_TEXT = "text";
/// Name of the icon element insie layout element.
const string TB_LA_ICON = "icon";
/// Name of the element declaring toolbar visibility.
const string TB_VISIBLE = "visible";
/// Value used for true in attribute and element values.
const string TRUE = "true";
/// Value used for false in attribute and element values.
const string FALSE = "false";
/// Name of the element declaring a toolbar button.
const string TB_SLOT = "slot";
/// Name of the attribute of the slot element.
const string SLOT_POSITION = "position";
/// Name of the element declaring action name inside slot element.
const string TB_SLOT_ACTION = "action";
/// Name of the element declaring a toolbar separator.
const string TB_SEPARATOR = "separator";
/// Name of the attribute of the separator element.
const string SEPARATOR_POSITION = "position";
/// Name of the element declaring a keyboard shortcut.
const string KEYBOARD_SHORTCUT = "keyboard-shortcut";
/// Name of the element declaring key combination.
const string KS_KEY_COMBINATION = "key-combination";
/// Name of the element meaning ctrl key in key combination.
const string KEY_COMB_CTRL = "ctrl";
/// Name of the element meaning alt key in key combination.
const string KEY_COMB_ALT = "alt";
/// Name of the element meaning some function key in key combination.
const string KEY_COMB_FKEY = "F-key";
/// Name of the attribute declaring function key number.
const string FKEY_NUMBER = "number";
/// Name of the element meaning some other key in key combination.
const string KEY_COMB_KEY = "key";
/// Name of the attribute which declares the actual key of the key element.
const string KEY_VALUE = "value";
/// Name of the element declaring the action performed by the keyboard shortcut
const string KS_ACTION = "action";

/// Default window width.
const int DEFAULT_WIDTH = 600;
/// Default window height.
const int DEFAULT_HEIGHT = 500;
/// Default x position.
const int DEFAULT_X_POS = 0;
/// Default y position.
const int DEFAULT_Y_POS = 0;
/// Default undo stack size.
const int DEFAULT_UNDO_SIZE = 10;

/// String to mean delete key.
const string DELETE_KEY = "del";

/**
 * Constructor.
 *
 * @param name Name of the configuration.
 */
GUIOptionsSerializer::GUIOptionsSerializer(std::string name) :
    XMLSerializer(),
    configurationName_(name) {
}


/**
 * Destructor.
 */
GUIOptionsSerializer::~GUIOptionsSerializer() {
}


/**
 * Serializes the given ObjectState tree created by GUIOptions::saveState
 * to an XML file.
 *
 * @param optionsState ObjectState tree created by GUIOptions::saveState.
 * @exception SerializerException If the current destination file cannot be
 *                                written.
 */
void
GUIOptionsSerializer::writeState(const ObjectState* optionsState)
    throw (SerializerException) {

    ObjectState* converted = convertToConfigFileFormat(optionsState);
    XMLSerializer::writeState(converted);
    delete converted;
}


/**
 * Reads the options from the current XML file set and creates an
 * ObjectState tree which can be read by GUIOptions::loadState.
 *
 * @return The newly created ObjectState tree.
 * @exception SerializerException If an error occurs while reading the file.
 */
ObjectState*
GUIOptionsSerializer::readState()
    throw (SerializerException) {

    ObjectState* state = XMLSerializer::readState();
    ObjectState* converted = convertToOptionsObjectFormat(state);
    delete state;
    return converted;
}


/**
 * Serializes the given options to the file set.
 *
 * @param options The options to be serialized.
 * @exception SerializerException If an error occurs while serializing.
 */
void
GUIOptionsSerializer::writeOptions(const GUIOptions& options)
    throw (SerializerException) {

    ObjectState* optionsState = options.saveState();
    writeState(optionsState);
    delete optionsState;
}


/**
 * Reads the current input file and creates GUIOptions according to it.
 *
 * @return The newly created GUIOptions instance.
 * @exception SerializerException If an error occurs while reading the file.
 * @exception ObjectStateLoadingException If an error occurs while creating
 *                                        options.
 */
GUIOptions*
GUIOptionsSerializer::readOptions()
    throw (SerializerException, ObjectStateLoadingException) {

    ObjectState* optionsState = readState();
    GUIOptions* options = new GUIOptions(optionsState);
    delete optionsState;
    return options;
}


/**
 * Converts the given ObjectState tree created by GUIOptions::saveState
 * to the format of configuration file.
 *
 * @param options ObjectState tree to be converted.
 * @return The newly created ObjectState tree which matches with
 *         configuration file format.
 */
ObjectState*
GUIOptionsSerializer::convertToConfigFileFormat(
    const ObjectState* options) const {

    ObjectState* root = new ObjectState(configurationName_);

    // add window element
    ObjectState* window = new ObjectState(WINDOW);
    root->addChild(window);

    ObjectState* fullScreen = new ObjectState(WI_FULLSCREEN);
    window->addChild(fullScreen);
    if (options->intAttribute(GUIOptions::OSKEY_FULL_SCREEN)) {
        fullScreen->setValue(TRUE);
    } else {
        fullScreen->setValue(FALSE);
    }

    ObjectState* width = new ObjectState(WI_WIDTH);
    window->addChild(width);
    width->setValue(options->stringAttribute(GUIOptions::OSKEY_WINDOW_WIDTH));

    ObjectState* height = new ObjectState(WI_HEIGHT);
    window->addChild(height);
    height->setValue(
        options->stringAttribute(GUIOptions::OSKEY_WINDOW_HEIGHT));

    ObjectState* xPos = new ObjectState(WI_X_POSITION);
    window->addChild(xPos);
    xPos->setValue(options->stringAttribute(GUIOptions::OSKEY_X_POS));

    ObjectState* yPos = new ObjectState(WI_Y_POSITION);
    window->addChild(yPos);
    yPos->setValue(options->stringAttribute(GUIOptions::OSKEY_Y_POS));

    // add toolbar element
    ObjectState* toolbar = new ObjectState(TOOLBAR);
    root->addChild(toolbar);

    ObjectState* layout = new ObjectState(TB_LAYOUT);
    toolbar->addChild(layout);

    string toolbarLayout = options->
        stringAttribute(GUIOptions::OSKEY_TOOLBAR_LAYOUT);
    if (toolbarLayout == GUIOptions::OSVALUE_TEXT) {
        layout->addChild(new ObjectState(TB_LA_TEXT));
    } else if (toolbarLayout == GUIOptions::OSVALUE_ICON) {
        layout->addChild(new ObjectState(TB_LA_ICON));
    } else {
        layout->addChild(new ObjectState(TB_LA_TEXT));
        layout->addChild(new ObjectState(TB_LA_ICON));
    }

    ObjectState* visible = new ObjectState(TB_VISIBLE);
    toolbar->addChild(visible);
    int vis =
        options->intAttribute(GUIOptions::OSKEY_TOOLBAR_VISIBILITY);
    if (vis) {
        visible->setValue(TRUE);
    } else {
        visible->setValue(FALSE);
    }

    // add toolbar buttons
    for (int i = 0; i < options->childCount(); i++) {
        ObjectState* child = options->child(i);
        if (child->name() == ToolbarButton::OSNAME_TOOLBAR_BUTTON) {
            ObjectState* slot = new ObjectState(TB_SLOT);
            toolbar->addChild(slot);
            slot->setAttribute(SLOT_POSITION,
                               child->stringAttribute(
                                   ToolbarButton::OSKEY_SLOT));
            ObjectState* action = new ObjectState(TB_SLOT_ACTION);
            slot->addChild(action);
            action->setValue(child->stringAttribute(
                                 ToolbarButton::OSKEY_ACTION));
        }
    }

    // add toolbar separators
    for (int i = 0; i < options->childCount(); i++) {
        ObjectState* child = options->child(i);
        if (child->name() == GUIOptions::OSNAME_SEPARATOR) {
            ObjectState* separator = new ObjectState(TB_SEPARATOR);
            toolbar->addChild(separator);
            separator->setAttribute(SEPARATOR_POSITION,
                                    child->stringAttribute(
                                        GUIOptions::OSKEY_POSITION));
        }
    }

    // add keyboard shortcuts
    for (int i = 0; i < options->childCount(); i++) {
        ObjectState* child = options->child(i);
        if (child->name() !=
            KeyboardShortcut::OSNAME_KEYBOARD_SHORTCUT) {
            continue;
        }
        ObjectState* sc = new ObjectState(KEYBOARD_SHORTCUT);
        root->addChild(sc);
        ObjectState* keyComb = new ObjectState(KS_KEY_COMBINATION);
        sc->addChild(keyComb);

        if (child->intAttribute(KeyboardShortcut::OSKEY_CTRL)) {
            keyComb->addChild(new ObjectState(KEY_COMB_CTRL));
        }
        if (child->intAttribute(KeyboardShortcut::OSKEY_ALT)) {
            keyComb->addChild(new ObjectState(KEY_COMB_ALT));
        }
        if (child->hasAttribute(KeyboardShortcut::OSKEY_FKEY)) {
            ObjectState* fKey = new ObjectState(KEY_COMB_FKEY);
            keyComb->addChild(fKey);
            fKey->setAttribute(FKEY_NUMBER,
                               child->stringAttribute(
                                   KeyboardShortcut::OSKEY_FKEY));
        }
        if (child->hasAttribute(KeyboardShortcut::OSKEY_KEY)) {
            ObjectState* key = new ObjectState(KEY_COMB_KEY);
            keyComb->addChild(key);
            char charKey = child->intAttribute(
                KeyboardShortcut::OSKEY_KEY);
            string stringKey;

            // check if key is 'del'
            if (charKey == 127) {
                stringKey = DELETE_KEY;
            } else {
                stringKey = Conversion::toString(charKey);
            }

            key->setAttribute(KEY_VALUE, stringKey);
        }
        ObjectState* action = new ObjectState(KS_ACTION);
        sc->addChild(action);
        action->setValue(child->stringAttribute(
                             KeyboardShortcut::OSKEY_ACTION));
    }

    return root;
}


/**
 * Creates a new ObjectState tree which can be given to GUIOptions
 * constructor. The tree is created according to the given tree which matches
 * with the syntax of the options file.
 *
 * @param root Root node of the ObjectState tree to be converted.
 */
ObjectState*
GUIOptionsSerializer::convertToOptionsObjectFormat(
    const ObjectState* root) const {

    ObjectState* options = new ObjectState(GUIOptions::OSNAME_OPTIONS);

    // set window properties
    if (root->hasChild(WINDOW)) {
        ObjectState* window = root->childByName(WINDOW);
        setWindowProperties(window, options);
    } else {
        options->setAttribute(GUIOptions::OSKEY_FULL_SCREEN, true);
        options->setAttribute(GUIOptions::OSKEY_WINDOW_WIDTH, DEFAULT_WIDTH);
        options->setAttribute(GUIOptions::OSKEY_WINDOW_HEIGHT, DEFAULT_HEIGHT);
        options->setAttribute(GUIOptions::OSKEY_X_POS, DEFAULT_X_POS);
        options->setAttribute(GUIOptions::OSKEY_Y_POS, DEFAULT_Y_POS);
    }

    // set toolbar properties
    if (root->hasChild(TOOLBAR)) {
        ObjectState* toolbar = root->childByName(TOOLBAR);
        setToolbarProperties(toolbar, options);
    } else {
        options->setAttribute(GUIOptions::OSKEY_TOOLBAR_VISIBILITY, false);
        options->setAttribute(GUIOptions::OSKEY_TOOLBAR_LAYOUT,
                              GUIOptions::OSVALUE_BOTH);
    }

    // set keyboard shortcuts
    for (int i = 0; i < root->childCount(); i++) {
        ObjectState* child = root->child(i);
        if (child->name() == KEYBOARD_SHORTCUT) {
            addKeyboardShortcut(child, options);
        }
    }

    return options;
}


/**
 * Sets the window properties for the given options according to the given
 * window element.
 *
 * @param windowElem ObjectState representing window element in options file.
 * @param options Options ObjectState which is modified.
 */
void
GUIOptionsSerializer::setWindowProperties(
    const ObjectState* windowElem,
    ObjectState* options) const {

    ObjectState* fullscreenElem = windowElem->childByName(WI_FULLSCREEN);
    if (fullscreenElem->stringValue() == TRUE) {
        options->setAttribute(GUIOptions::OSKEY_FULL_SCREEN, true);
    } else if (fullscreenElem->stringValue() == FALSE) {
        options->setAttribute(GUIOptions::OSKEY_FULL_SCREEN, false);
    } else {
        assert(false);
    }

    ObjectState* widthElem = windowElem->childByName(WI_WIDTH);
    options->setAttribute(GUIOptions::OSKEY_WINDOW_WIDTH,
                          widthElem->stringValue());

    ObjectState* heightElem = windowElem->childByName(WI_HEIGHT);
    options->setAttribute(GUIOptions::OSKEY_WINDOW_HEIGHT,
                          heightElem->stringValue());

    ObjectState* xPosElem = windowElem->childByName(WI_X_POSITION);
    options->setAttribute(GUIOptions::OSKEY_X_POS,
                          xPosElem->stringValue());

    ObjectState* yPosElem = windowElem->childByName(WI_Y_POSITION);
    options->setAttribute(GUIOptions::OSKEY_Y_POS,
                          yPosElem->stringValue());
}


/**
 * Sets the toolbar properties for the given options according to the given
 * toolbar element.
 *
 * @param toolbarElem ObjectState representing toolbar element in options
 *                    file.
 * @param options Options ObjectState which is modified.
 */
void
GUIOptionsSerializer::setToolbarProperties(
    const ObjectState* toolbarElem,
    ObjectState* options) const {

    // set layout
    ObjectState* layoutElem = toolbarElem->childByName(TB_LAYOUT);
    if (layoutElem->hasChild(TB_LA_TEXT) &&
        layoutElem->hasChild(TB_LA_ICON)) {
        options->setAttribute(GUIOptions::OSKEY_TOOLBAR_LAYOUT,
                              GUIOptions::OSVALUE_BOTH);
    } else if (layoutElem->hasChild(TB_LA_TEXT)) {
        options->setAttribute(GUIOptions::OSKEY_TOOLBAR_LAYOUT,
                              GUIOptions::OSVALUE_TEXT);
    } else if (layoutElem->hasChild(TB_LA_ICON)) {
        options->setAttribute(GUIOptions::OSKEY_TOOLBAR_LAYOUT,
                              GUIOptions::OSVALUE_ICON);
    } else {
        assert(false);
    }

    // set visibility
    ObjectState* visibleElem = toolbarElem->childByName(TB_VISIBLE);
    if (visibleElem->stringValue() == TRUE) {
        options->setAttribute(GUIOptions::OSKEY_TOOLBAR_VISIBILITY,
                              true);
    } else if (visibleElem->stringValue() == FALSE) {
        options->setAttribute(GUIOptions::OSKEY_TOOLBAR_VISIBILITY,
                              false);
    }

    // set buttons and separators
    for (int i = 0; i < toolbarElem->childCount(); i++) {
        ObjectState* child = toolbarElem->child(i);
        if (child->name() == TB_SLOT) {
            ObjectState* button = new ObjectState(
                ToolbarButton::OSNAME_TOOLBAR_BUTTON);
            options->addChild(button);

            string slot = child->stringAttribute(SLOT_POSITION);
            button->setAttribute(ToolbarButton::OSKEY_SLOT, slot);

            ObjectState* action = child->childByName(TB_SLOT_ACTION);
            string actionString = action->stringValue();
            button->setAttribute(ToolbarButton::OSKEY_ACTION, actionString);

        } else if (child->name() == TB_SEPARATOR) {
            ObjectState* separator =
                new ObjectState(GUIOptions::OSNAME_SEPARATOR);
            options->addChild(separator);
            string position = child->stringAttribute(SEPARATOR_POSITION);
            separator->setAttribute(GUIOptions::OSKEY_POSITION, position);
        }
    }
}


/**
 * Adds a keyboard shortcut to the given options according to the given
 * keyboard-shortcut element.
 *
 * @param ksElem ObjectState representing a keyboard-shortcut element in
 *               options file.
 * @param options Options ObjectState which is modified.
 */
void
GUIOptionsSerializer::addKeyboardShortcut(
    const ObjectState* ksElem,
    ObjectState* options) const {

    ObjectState* shortcut =
        new ObjectState(KeyboardShortcut::OSNAME_KEYBOARD_SHORTCUT);
    options->addChild(shortcut);

    ObjectState* keyCombElem = ksElem->childByName(KS_KEY_COMBINATION);
    if (keyCombElem->hasChild(KEY_COMB_CTRL)) {
        shortcut->setAttribute(KeyboardShortcut::OSKEY_CTRL, true);
    } else {
        shortcut->setAttribute(KeyboardShortcut::OSKEY_CTRL, false);
    }

    if (keyCombElem->hasChild(KEY_COMB_ALT)) {
        shortcut->setAttribute(KeyboardShortcut::OSKEY_ALT, true);
    } else {
        shortcut->setAttribute(KeyboardShortcut::OSKEY_ALT, false);
    }

    if (keyCombElem->hasChild(KEY_COMB_KEY)) {
        ObjectState* keyElem = keyCombElem->childByName(KEY_COMB_KEY);
        string key = keyElem->stringAttribute(KEY_VALUE);
        char keyChar = 0;

        // delete key requires extra checking
        if (key == DELETE_KEY) {
            keyChar = 127; // ASCII 127 = DEL
        } else {
            keyChar = *(key.c_str());
        }

        shortcut->setAttribute(KeyboardShortcut::OSKEY_KEY, keyChar);
    }

    if (keyCombElem->hasChild(KEY_COMB_FKEY)) {
        ObjectState* fKeyElem = keyCombElem->childByName(KEY_COMB_FKEY);
        int number = fKeyElem->intAttribute(FKEY_NUMBER);
        shortcut->setAttribute(KeyboardShortcut::OSKEY_FKEY, number);
    }

    ObjectState* actionElem = ksElem->childByName(KS_ACTION);
    string action = actionElem->stringValue();
    shortcut->setAttribute(KeyboardShortcut::OSKEY_ACTION, action);
}
