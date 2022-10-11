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
OSEdOptions::loadState(const ObjectState* state) {
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
