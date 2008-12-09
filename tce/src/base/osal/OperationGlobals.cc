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
 * @file OperationGlobals.cc
 *
 * Definition of OperationGlobals singleton class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
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
