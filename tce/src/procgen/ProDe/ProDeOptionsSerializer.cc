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
 * @file ProDeOptionsSerializer.cc
 *
 * Implementation of ProDeOptionsSerializer class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProDeOptionsSerializer.hh"
#include "Application.hh"
#include "Conversion.hh"

using std::string;

/// Name of the element declaring the size of the undo stack.
const string UNDO = "undo-stack";
/// Name of the attribute of the undo element.
const string UNDO_SIZE = "size";

/// Default undo stack size.
const int DEFAULT_UNDO_SIZE = 10;


/**
 * Constructor.
 */
ProDeOptionsSerializer::ProDeOptionsSerializer() :
    GUIOptionsSerializer(ProDeOptions::CONFIGURATION_NAME) {
}


/**
 * Destructor.
 */
ProDeOptionsSerializer::~ProDeOptionsSerializer() {
}


/**
 * Converts the given ObjectState tree created by ProDeOptions::saveState
 * to the format of configuration file.
 *
 * @param options ObjectState tree to be converted.
 * @return The newly created ObjectState tree which matches with
 *         configuration file format.
 */
ObjectState*
ProDeOptionsSerializer::convertToConfigFileFormat(
    const ObjectState* options) const {

    ObjectState* root =
        GUIOptionsSerializer::convertToConfigFileFormat(options);

    // add undo stack size
    ObjectState* undoStack = new ObjectState(UNDO);
    root->addChild(undoStack);
    undoStack->setAttribute(
        UNDO_SIZE, options->stringAttribute(
          ProDeOptions::OSKEY_UNDO_STACK_SIZE));

    return root;
}


/**
 * Creates a new ObjectState tree which can be given to ProDeOptions
 * constructor. The tree is created according to the given tree which matches
 * with the syntax of the options file.
 *
 * @param root Root node of the ObjectState tree to be converted.
 */
ObjectState*
ProDeOptionsSerializer::convertToOptionsObjectFormat(
    const ObjectState* root) const {

    ObjectState* options =
        GUIOptionsSerializer::convertToOptionsObjectFormat(root);

    // set undo stack size
    if (root->hasChild(UNDO)) {
        ObjectState* undo = root->childByName(UNDO);
        options->setAttribute(
            ProDeOptions::OSKEY_UNDO_STACK_SIZE,
            undo->stringAttribute(UNDO_SIZE));
    } else {
        options->setAttribute(
            ProDeOptions::OSKEY_UNDO_STACK_SIZE, DEFAULT_UNDO_SIZE);
    }

    return options;
}
