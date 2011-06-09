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
 * @file UniversalFUPort.cc
 *
 * Implementation of UniversalFUPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#include "UniversalFUPort.hh"
#include "UniversalFunctionUnit.hh"

using std::string;

/**
 * The constructor.
 *
 * @param name Name of the port.
 * @param width Bit width of the port.
 * @param parent The function unit to which the port belongs.
 * @param isTriggering If true, writing (or reading) this port starts the
 *                     execution of a new operation.
 * @param setsOpcode If true, writing (or reading) this port selects the
 *                   operation to be executed. Opcode-setting ports must
 *                   be triggering.
 * @exception ComponentAlreadyExists If the function unit already has another
 *                                   port by the same name.
 * @exception OutOfRange If the given bit width is less or equal to zero.
 * @exception IllegalParameters If setsOpcode argument is true and
 *                              isTriggering false.
 * @exception InvalidName If the given name is not a valid component name.
 */
UniversalFUPort::UniversalFUPort(
    const std::string& name,
    int width,
    UniversalFunctionUnit& parent,
    bool isTriggering,
    bool setsOpcode)
    throw (ComponentAlreadyExists, OutOfRange, IllegalParameters,
           InvalidName) :
    FUPort(name, width, parent, isTriggering, setsOpcode, true, true) {
}


/**
 * The destructor.
 */
UniversalFUPort::~UniversalFUPort() {
}


/**
 * Aborts the program. It is not allowed to set the name of UniversalFUPort.
 * DO NOT CALL THIS METHOD!
 *
 * @param name Never used.
 * @exception ComponentAlreadyExists Never thrown.
 * @exception InvalidName Never thrown.
 */
void
UniversalFUPort::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    const string procName = "UniversalFUPort::setName";
    const string errorMsg = "Tried to set the name of UniversalFUPort!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to set the width of UniversalFUPort.
 * DO NOT CALL THIS METHOD!
 *
 * @param width Never used.
 * @exception OutOfRange Never thrown.
 */
void
UniversalFUPort::setWidth(int)
    throw (OutOfRange) {

    const string procName = "UniversalFUPort::setWidth";
    const string errorMsg = "Tried to set width of UniversalFUPort!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to modify UniversalFUPort. DO NOT
 * CALL THIS METHOD!
 *
 * @param triggers Never used.
 */
void
UniversalFUPort::setTriggering(bool) {
    const string procName = "UniversalFUPort::setTriggering";
    const string errorMsg =
        "Tried to set the triggering property of UniversalFUPort!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to set the operation code setting
 * property of UniversalFUPort. DO NOT CALL THIS METHOD!
 *
 * @param setsOpcode Never used.
 * @exception ComponentAlreadyExists Never thrown.
 */
void
UniversalFUPort::setOpcodeSetting(bool)
    throw (ComponentAlreadyExists) {

    const string procName = "UniversalFUPort::setOpcodeSetting";
    const string errorMsg = "Tried to set the operation code setting "
        "property of UniversalFUPort!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to load the state of UniversalFUPort
 * from an ObjectState tree. DO NOT CALL THIS METDOD!
 *
 * @param state Never used.
 * @exception ObjectStateLoadingException Never thrown.
 */
void
UniversalFUPort::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    const string procName = "UniversalFUPort::loadState";
    const string errorMsg =
        "Tried to load state of UniversalFUPort from an ObjectState tree!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}
