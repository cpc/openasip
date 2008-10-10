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
 * @file OSEdOptions.cc
 *
 * Definition of OSEdOptions class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "OSEdOptions.hh"
#include "ObjectState.hh"

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
