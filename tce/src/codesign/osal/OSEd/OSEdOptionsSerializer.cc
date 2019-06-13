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
 * @file OSEdOptionsSerializer.cc
 *
 * Definition of OSEdOptionsSerializer class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
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
OSEdOptionsSerializer::writeState(const ObjectState* state) {
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
OSEdOptionsSerializer::readState() {
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
