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
 * @file NullMachine.hh
 *
 * Implementation of NullMachine class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#include "NullMachine.hh"

namespace TTAMachine {

NullMachine NullMachine::machine_;


/**
 * The constructor.
 */
NullMachine::NullMachine() : Machine() {
}


/**
 * The destructor.
 */
NullMachine::~NullMachine() {
}


/**
 * Returns the only instance of NullMachine class.
 *
 * @return The only instance of NullMachine class.
 */
NullMachine&
NullMachine::instance() {
    return machine_;
}


/**
 * Aborts the program with an error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullMachine::addBus(Bus&)
    throw (ComponentAlreadyExists) {

    abortWithError("NullMachine::addBus");
}


/**
 * Aborts the program with an error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullMachine::addSocket(Socket&)
    throw (ComponentAlreadyExists) {

    abortWithError("NullMachine::addSocket");
}


/**
 * Aborts the program with an error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 * @exception IllegalParameters Never thrown.
 */
void
NullMachine::addUnit(Unit&)
    throw (ComponentAlreadyExists, IllegalParameters) {

    abortWithError("NullMachine::addUnit");
}


/**
 * Aborts the program with an error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 * @exception IllegalParameters Never thrown.
 */
void
NullMachine::addFunctionUnit(FunctionUnit&)
    throw (ComponentAlreadyExists, IllegalParameters) {

    abortWithError("NullMachine::addFunctionUnit");
}


/**
 * Aborts the program with an error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullMachine::addImmediateUnit(ImmediateUnit&)
    throw (ComponentAlreadyExists) {

    abortWithError("NullMachine::addImmediateUnit");
}


/**
 * Aborts the program with an error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullMachine::addRegisterFile(RegisterFile&)
    throw (ComponentAlreadyExists) {

    abortWithError("NullMachine::addRegisterFile");
}


/**
 * Aborts the program with an error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullMachine::addAddressSpace(AddressSpace&)
    throw (ComponentAlreadyExists) {

    abortWithError("NullMachine::addAddressSpace");
}


/**
 * Aborts the program with an error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullMachine::addBridge(Bridge&)
    throw (ComponentAlreadyExists) {

    abortWithError("NullMachine::addBridge");
}


/**
 * Aborts the program with an error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullMachine::addInstructionTemplate(InstructionTemplate&)
    throw (ComponentAlreadyExists) {

    abortWithError("NullMachine::addInstructionTemplate");
}


/**
 * Aborts the program with an error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullMachine::setGlobalControl(ControlUnit&)
    throw (ComponentAlreadyExists) {

    abortWithError("NullMachine::setGlobalControl");
}


/**
 * Aborts the program with an error message.
 */
void
NullMachine::unsetGlobalControl() {
    abortWithError("NullMachine::unsetGlobalControl");
}


/**
 * Aborts the program with an error message.
 *
 * @return Never returns.
 */
ControlUnit*
NullMachine::controlUnit() const {
    abortWithError("NullMachine::controlUnit");
    return NULL;
}


/**
 * Aborts the program with an error message.
 *
 * @exception InstanceNotFound Never thrown.
 */
void
NullMachine::removeBus(Bus&)
    throw (InstanceNotFound) {

    abortWithError("NullMachine::removeBus");
}


/**
 * Aborts the program with an error message.
 *
 * @exception InstanceNotFound Never thrown.
 */
void
NullMachine::removeSocket(Socket&)
    throw (InstanceNotFound) {

    abortWithError("NullMachine::removeSocket");
}


/**
 * Aborts the program with an error message.
 *
 * @exception InstanceNotFound Never thrown.
 * @exception IllegalParameters Never thrown.
 */
void
NullMachine::removeUnit(Unit&)
    throw (InstanceNotFound, IllegalParameters) {

    abortWithError("NullMachine::removeUnit");
}


/**
 * Aborts the program with an error message.
 *
 * @exception InstanceNotFound Never thrown.
 */
void
NullMachine::removeFunctionUnit(FunctionUnit&)
    throw (InstanceNotFound) {

    abortWithError("NullMachine::removeFunctionUnit");
}


/**
 * Aborts the program with an error message.
 *
 * @exception InstanceNotFound Never thrown.
 */
void
NullMachine::removeImmediateUnit(ImmediateUnit&)
    throw (InstanceNotFound) {

    abortWithError("NullMachine::removeImmediateUnit");
}


/**
 * Aborts the program with an error message.
 *
 * @exception InstanceNotFound Never thrown.
 */
void
NullMachine::removeRegisterFile(RegisterFile&)
    throw (InstanceNotFound) {

    abortWithError("NullMachine::removeRegisterFile");
}


/**
 * Aborts the program with an error message.
 *
 * @exception InstanceNotFound Never thrown.
 */
void
NullMachine::deleteBridge(Bridge&)
    throw (InstanceNotFound) {

    abortWithError("NullMachine::deleteBridge");
}


/**
 * Aborts the program with an error message.
 *
 * @exception InstanceNotFound Never thrown.
 */
void
NullMachine::deleteInstructionTemplate(InstructionTemplate&)
    throw (InstanceNotFound) {

    abortWithError("NullMachine::deleteInstructionTemplate");
}


/**
 * Aborts the program with an error message.
 *
 * @exception InstanceNotFound Never thrown.
 */
void
NullMachine::deleteAddressSpace(AddressSpace&)
    throw (InstanceNotFound) {

    abortWithError("NullMachine::deleteAddressSpace");
}


/**
 * Aborts the program with an error message.
 *
 * @return Never returns.
 */
Machine::BusNavigator
NullMachine::busNavigator() const {
    abortWithError("NullMachine::busNavigator");
    return Machine::busNavigator();
}


/**
 * Aborts the program with an error message.
 *
 * @return Never returns.
 */
Machine::SocketNavigator
NullMachine::socketNavigator() const {
    abortWithError("NullMachine::socketNavigator");
    return Machine::socketNavigator();
}


/**
 * Aborts the program with an error message.
 *
 * @return Never returns.
 */
Machine::FunctionUnitNavigator
NullMachine::functionUnitNavigator() const {
    abortWithError("NullMachine::functionUnitNavigator");
    return Machine::functionUnitNavigator();
}


/**
 * Aborts the program with an error message.
 *
 * @return Never returns.
 */
Machine::AddressSpaceNavigator
NullMachine::addressSpaceNavigator() const {
    abortWithError("NullMachine::addressSpaceNavigator");
    return Machine::addressSpaceNavigator();
}


/**
 * Aborts the program with an error message.
 *
 * @return Never returns.
 */
Machine::BridgeNavigator
NullMachine::bridgeNavigator() const {
    abortWithError("NullMachine::bridgeNavigator");
    return Machine::bridgeNavigator();
}


/**
 * Aborts the program with an error message.
 *
 * @return Never returns.
 */
Machine::ImmediateUnitNavigator
NullMachine::immediateUnitNavigator() const {
    abortWithError("NullMachine::immediateUnitNavigator");
    return Machine::immediateUnitNavigator();
}


/**
 * Aborts the program with an error message.
 *
 * @return Never returns.
 */
Machine::InstructionTemplateNavigator
NullMachine::instructionTemplateNavigator() const {
    abortWithError("NullMachine::instructionTemplateNavigator");
    return Machine::instructionTemplateNavigator();
}


/**
 * Aborts the program with an error message.
 *
 * @return Never returns.
 */
Machine::RegisterFileNavigator
NullMachine::registerFileNavigator() const {
    abortWithError("NullMachine::registerFileNavigator");
    return Machine::registerFileNavigator();
}


/**
 * Aborts the program with an error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullMachine::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    abortWithError("NullMachine::loadState");
}


/**
 * Aborts the program with an error message.
 *
 * @return Never returns.
 */
ObjectState*
NullMachine::saveState() const {
    abortWithError("NullMachine::saveState");
    return NULL;
}

}
