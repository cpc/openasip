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
 * @file ProDeOptionsSerializer.cc
 *
 * Implementation of ProDeOptionsSerializer class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
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
