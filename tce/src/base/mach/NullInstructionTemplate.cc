/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file NullInstructionTemplate.cc
 *
 * Implementation of NullInstructionTemplate class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
NullInstructionTemplate::setName(const std::string&) {
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
NullInstructionTemplate::addSlot(const std::string&, int, ImmediateUnit&) {
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
NullInstructionTemplate::destinationOfSlot(const std::string&) const {
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
NullInstructionTemplate::slotOfDestination(const ImmediateUnit&, int) const {
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
NullInstructionTemplate::setMachine(Machine&) {
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
NullInstructionTemplate::ensureRegistration(const Component&) const {
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
NullInstructionTemplate::loadState(const ObjectState*) {
    abortWithError("loadState()");
}
}
