/**
 * @file OperationGlobals.cc
 *
 * Definition of OperationGlobals singleton class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include "OperationGlobals.hh"
#include <iostream>

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
