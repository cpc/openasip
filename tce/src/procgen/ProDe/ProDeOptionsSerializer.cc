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
