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
 * @file NullInstructionTemplate.cc
 *
 * Implementation of NullInstructionTemplate class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#include "NullInstructionTemplate.hh"
#include "Machine.hh"

namespace TTAMachine {

Machine NullInstructionTemplate::machine_;
NullInstructionTemplate NullInstructionTemplate::instance_;

/**
 * The constructor.
 */
NullInstructionTemplate::NullInstructionTemplate() :
    InstructionTemplate("NULL", machine_) {
}


/**
 * The destructor.
 */
NullInstructionTemplate::~NullInstructionTemplate() {
}


/**
 * Returns the only instance of NullInstructionTemplate.
 *
 * @return The only instance of NullInstructionTemplate.
 */
NullInstructionTemplate&
NullInstructionTemplate::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullInstructionTemplate::name() const {
    abortWithError("name()");
    return "";
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 * @exception InvalidName Never thrown.
 */
void
NullInstructionTemplate::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    abortWithError("setName()");
}


/**
 * Aborts the program with error message.
 *
 * @exception InstanceNotFound Never thrown.
 * @exception ComponentAlreadyExists Never thrown.
 * @exception OutOfRange Never thrown.
 */
void
NullInstructionTemplate::addSlot(const std::string&, int, ImmediateUnit&)
    throw (InstanceNotFound, ComponentAlreadyExists, OutOfRange) {

    abortWithError("addSlot()");
}


/**
 * Aborts the program with error message.
 */
void
NullInstructionTemplate::removeSlot(const std::string&) {
    abortWithError("removeSlot()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
void
NullInstructionTemplate::removeSlots(const ImmediateUnit&) {
    abortWithError("removeSlots()");
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullInstructionTemplate::slotCount() const {
    abortWithError("slotCount()");
    return -1;
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
TemplateSlot*
NullInstructionTemplate::slot(int) const {
    abortWithError("slot()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullInstructionTemplate::usesSlot(const std::string&) const {
    abortWithError("usesSlot()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullInstructionTemplate::destinationUsesSlot(
    const std::string&,
    const ImmediateUnit&) const {

    abortWithError("destinationUsesSlot()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullInstructionTemplate::numberOfDestinations() const {
    abortWithError("numberOfDestinations()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullInstructionTemplate::isOneOfDestinations(const ImmediateUnit&) const {
    abortWithError("isOneOfDestinations()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ImmediateUnit*
NullInstructionTemplate::destinationOfSlot(const std::string&) const
    throw (InstanceNotFound) {

    abortWithError("destinationOfSlot()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullInstructionTemplate::numberOfSlots(const ImmediateUnit&) const {
    abortWithError("numberOfSlots()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullInstructionTemplate::slotOfDestination(const ImmediateUnit&, int) const
    throw (OutOfRange) {

    abortWithError("slotOfDestination()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullInstructionTemplate::supportedWidth() const {
    abortWithError("supportedWidth()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullInstructionTemplate::supportedWidth(const ImmediateUnit&) const {
    abortWithError("supportedWidth()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullInstructionTemplate::supportedWidth(const std::string&) const {
    abortWithError("supportedWidth()");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullInstructionTemplate::isEmpty() const {
    abortWithError("isEmpty()");
    return false;
}


/**
 * Aborts the program with error message.
 *
 * @exception ComponentAlreadyExists Never thrown.
 */
void
NullInstructionTemplate::setMachine(Machine&)
    throw (ComponentAlreadyExists) {

    abortWithError("setMachine()");
}


/**
 * Aborts the program with error message.
 */
void
NullInstructionTemplate::unsetMachine() {
    abortWithError("unsetMachine()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
Machine*
NullInstructionTemplate::machine() const {
    abortWithError("machine()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception IllegalRegistration Never thrown.
 */
void
NullInstructionTemplate::ensureRegistration(const Component&) const
    throw (IllegalRegistration) {

    abortWithError("ensureRegistration()");
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
bool
NullInstructionTemplate::isRegistered() const {
    abortWithError("isRegistered()");
    return false;
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
ObjectState*
NullInstructionTemplate::saveState() const {
    abortWithError("saveState()");
    return NULL;
}


/**
 * Aborts the program with error message.
 *
 * @exception ObjectStateLoadingException Never thrown.
 */
void
NullInstructionTemplate::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    abortWithError("loadState()");
}

}
