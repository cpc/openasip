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
 * @file OSEdOptionsSerializer.cc
 *
 * Definition of OSEdOptionsSerializer class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include "OSEdOptionsSerializer.hh"

using std::string;

/**
 * Constructor.
 */
OSEdOptionsSerializer::OSEdOptionsSerializer() : Serializer() {
}

/**
 * Destructor.
 */
OSEdOptionsSerializer::~OSEdOptionsSerializer() {
}

/**
 * Writes the object state tree to XML file.
 *
 * @param state State to be written.
 */
void
OSEdOptionsSerializer::writeState(const ObjectState* state)
    throw (SerializerException) {
	
    try {
        serializer_.writeState(state);
    } catch (const Exception& e) {
        string method = "OSEdOptionsSerializer::writeState()";
        string msg = "Problems writing the state: " + e.errorMessage();
        throw SerializerException(__FILE__, __LINE__, method, msg);
    }

}

/**
 * Reads the options file and returns the corresponding ObjectState tree.
 *
 * @return The read ObjectState tree.
 */
ObjectState*
OSEdOptionsSerializer::readState()
    throw (SerializerException) {

    ObjectState* root = NULL;
    try {
        root = serializer_.readState();
    } catch (const Exception& e) {
        string method = "OSEdOptionsSerializer::readState()";
        string msg = "Problems reading the state: " + e.errorMessage();
        throw SerializerException(__FILE__, __LINE__, method, msg);
    }
    return root;
}

/**
 * Sets the source file.
 *
 * @param fileName The name of the file.
 */
void
OSEdOptionsSerializer::setSourceFile(const std::string& fileName) {
    serializer_.setSourceFile(fileName);
}

/**
 * Sets the destination file.
 *
 * @param fileName The name of the file.
 */
void
OSEdOptionsSerializer::setDestinationFile(const std::string& fileName) {
    serializer_.setDestinationFile(fileName);
}

/**
 * Sets the schema file.
 *
 * @param fileName The name of the file.
 */
void
OSEdOptionsSerializer::setSchemaFile(const std::string& fileName) {
    serializer_.setSchemaFile(fileName);
}

/**
 * Modifies the usage of schema file (yes/no).
 *
 * @param useSchema True or false, depending on if schema should be used or not.
 */
void
OSEdOptionsSerializer::setUseSchema(bool useSchema) {
    serializer_.setUseSchema(useSchema);
}
