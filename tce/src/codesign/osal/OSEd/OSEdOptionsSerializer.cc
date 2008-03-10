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
