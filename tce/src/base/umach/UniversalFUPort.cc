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
 * @file UniversalFUPort.cc
 *
 * Implementation of UniversalFUPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
    FUPort(name, width, parent, isTriggering, setsOpcode, true) {
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
