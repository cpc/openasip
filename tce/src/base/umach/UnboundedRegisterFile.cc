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
 * @file UnboundedRegisterFile.cc
 *
 * Implementation of UnboundedRegisterFile class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

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
