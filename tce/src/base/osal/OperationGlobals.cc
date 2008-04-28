/**
 * @file OperationGlobals.cc
 *
 * Definition of OperationGlobals singleton class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include "Operation.hh"
#include "OperationGlobals.hh"
#include "Exception.hh"
#include <iostream>
#include <string>
#include "TCEString.hh"

std::ostream* OperationGlobals::outputStream_ = &std::cout;


/**
 * Returns the current output stream
 * 
 * @return the current output stream
 */
std::ostream& 
OperationGlobals::outputStream() {
    return *outputStream_;
}

/**
 * Sets a new output stream for operation globals
 * 
 * @param newOutputStream new output stream to set
 */
void
OperationGlobals::setOutputStream(std::ostream& newOutputStream) {
    outputStream_ = &newOutputStream;
}

/**
 * Throws an exception with a message
 * 
 * @param message Message
 * @param file __FILE__
 * @param line __LINE__
 * @param parent parent operation
 * @exception SimulationExecutionError thrown always.
 */
void
OperationGlobals::runtimeError(
    const char* message, 
    const char* file, 
    int line,
    const Operation& parent) {
    throw SimulationExecutionError(std::string(file), line,
        parent.name(), std::string(" ") + parent.name() + ": " + message);
}
