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
 * @file UnboundedRegisterFile.cc
 *
 * Implementation of UnboundedRegisterFile class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#include <climits>

#include "UnboundedRegisterFile.hh"
#include "Port.hh"

using std::string;

/**
 * The constructor.
 *
 * @param name The name of the register file.
 * @param width Bit width of the registers in the register file.
 * @param type Type of the register file.
 * @exception InvalidName If one of the given names is not a valid component
 *                        name.
 * @exception OutOfRange If the bit width is out of range.
 */
UnboundedRegisterFile::UnboundedRegisterFile(
    const std::string& name,
    int width,
    RegisterFile::Type type)
    throw (InvalidName, OutOfRange) :
    RegisterFile(name, INT_MAX, width, 1, 1, 0, type) {
}


/**
 * The destructor.
 */
UnboundedRegisterFile::~UnboundedRegisterFile() {
}


/**
 * Returns INT_MAX. 
 *
 * UnboundedRegisterFile has "unlimited" amount of registers. 
 * 
 * @return INT_MAX.
 */
int
UnboundedRegisterFile::numberOfRegisters() const {
    return INT_MAX;
}


/**
 * Aborts the program. It is not allowed to call this method of
 * UnboundedRegisterFile. DO NOT CALL THIS METHOD!
 *
 * @param reads Never used.
 * @exception OutOfRange Never thrown.
 */
void
UnboundedRegisterFile::setMaxReads(int)
    throw (OutOfRange) {

    const string procName = "UnboundedRegisterFile::setMaxReads";
    const string errorMsg =
        "Tried to set max reads of UnboundedRegisterFile!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to call this method of
 * UnboundedRegisterFile. DO NOT CALL THIS METHOD!
 *
 * @param maxWrites Never used.
 * @exception OutOfRange Never thrown.
 */
void
UnboundedRegisterFile::setMaxWrites(int)
    throw (OutOfRange) {

    const string procName = "UnboundedRegisterFile::setMaxWrites";
    const string errorMsg = "Tries to set maxWrites of UnboundedRegisterFile!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to set the number of
 * registers of UnboundedRegisterFile. It has always unlimited amount
 * of registers. DO NOT CALL THIS METHOD.
 *
 * @param registers Never used.
 * @exception OutOfRange Never thrown.
 */
void
UnboundedRegisterFile::setNumberOfRegisters(int)
    throw (OutOfRange) {

    const string procName = "UnboundedRegisterFile::setNumberOfRegisters";
    const string errorMsg =
        "Tried to set the number of registers of UnboundedRegisterFile!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to set the bit width of
 * UnboundedRegisterFile once it is set at construction. DO NOT CALL
 * THIS METHOD.
 *
 * @param width Never used.
 * @exception OutOfRange Never thrown.
 */
void
UnboundedRegisterFile::setWidth(int)
    throw (OutOfRange) {

    const string procName = "UnboundedRegisterFile::setWidth";
    const string errorMsg =
        "Tried to set the bit width of UnboundedRegisterFile!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to change the name of
 * UnboundedRegisterFile once it is set at construction. DO NOT CALL
 * THIS METHOD!
 *
 * @param name Never used.
 * @exception ComponentAlreadyExists Never thrown.
 * @exception InvalidName Never thrown.
 */
void
UnboundedRegisterFile::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    const string procName = "UnboundedRegisterFile::setName";
    const string errorMsg =
        "Tried to set the name of UnboundedRegisterFile!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to set the type of
 * UnboundedRegisterFile once it is set at construction. DO NOT CALL
 * THIS METHOD!
 *
 * @param type Never used.
 */
void
UnboundedRegisterFile::setType(RegisterFile::Type) {
    const string procName = "UnboundedRegisterFile::setType";
    const string errorMsg = "Tried to the type of UnboundedRegisterFile!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to load UniversalMachine from
 * an ObjectState tree. DO NOT CALL THIS METHOD!
 *
 * @param state Never used.
 * @exception ObjectStateLoadingException Never thrown.
 */
void
UnboundedRegisterFile::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    const string procName = "UnboundedRegisterFile::loadState";
    const string errorMsg =
        "Tried to load state of UnboundedRegisterFile from an ObjectState"
        "tree!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}
