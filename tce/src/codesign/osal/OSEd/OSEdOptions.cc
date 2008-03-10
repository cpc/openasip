/**
 * @file OSEdOptions.cc
 *
 * Definition of OSEdOptions class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include "OSEdOptions.hh"

using std::string;

const string OSED_CONF = "osed-configuration";
const string OSED_EDITOR = "editor";

/**
 * Constructor.
 */
OSEdOptions::OSEdOptions() : editor_("") {
}

/**
 * Destructor.
 */
OSEdOptions::~OSEdOptions() {
}

/**
 * Returns the editor.
 *
 * @return The editor.
 */
string
OSEdOptions::editor() const {
    return editor_;
}

/**
 * Sets the editor.
 *
 * @param editor. Editor to be set.
 */
void
OSEdOptions::setEditor(const std::string& editor) {
    editor_ = editor;
}

/**
 * Loads the state from the object tree.
 *
 * @param state The to be loaded.
 */
void
OSEdOptions::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {
	
    try {
        for (int i = 0; i < state->childCount(); i++) {
            ObjectState* child = state->child(i);
            if (child->name() == OSED_EDITOR) {
                editor_ = child->stringValue();
            }
        }
    } catch (const Exception& e) {
        string method = "OSEdOptions::loadState()";
        string msg = "Problems loading state: " + e.errorMessage();
        throw ObjectStateLoadingException(__FILE__, __LINE__, method, msg);
    }
}

/**
 * Saves the state of the options in ObjectState tree.
 *
 * @return The created ObjectState tree.
 */
ObjectState*
OSEdOptions::saveState() const {
    ObjectState* root = new ObjectState(OSED_CONF);
    ObjectState* editor = new ObjectState(OSED_EDITOR);
    editor->setValue(editor_);
    root->addChild(editor);
    return root;
}
