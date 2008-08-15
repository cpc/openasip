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
 * @file IDFValidator.cc
 *
 * Implementation of IDFValidator class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include "IDFValidator.hh"
#include "Machine.hh"
#include "MachineImplementation.hh"

using namespace TTAMachine;
using namespace IDF;
using boost::format;

/**
 * The constructor.
 */
IDFValidator::IDFValidator(
    const IDF::MachineImplementation& idf,
    const TTAMachine::Machine& machine) :
    machine_(machine), idf_(idf) {
}


/**
 * The destructor.
 */
IDFValidator::~IDFValidator() {
}


/**
 * Validates the IDF by checking that the IDF has an implementation
 * defined for all the units of the machine.
 *
 * @return True if the IDF is valid, otherwise false.
 */
bool
IDFValidator::validate() {
    errorMessages_.clear();
    checkFUImplementations();
    checkRFImplementations();
    checkIUImplementations();
    return errorCount() == 0;
}


/**
 * Returns the number of errors found in the last validation.
 *
 * @return The error count.
 */
int
IDFValidator::errorCount() const {
    return errorMessages_.size();
}


/**
 * Returns an error message by the given index.
 *
 * @param index The index.
 * @exception OutOfRange If the index is negative or not smaller than the 
 *                       number of errors.
 */
std::string
IDFValidator::errorMessage(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= errorCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return errorMessages_[index];
}


/**
 * Checks that the IDF defines an implementation for each FU in the machine.
 *
 * If errors are found, error messages are inserted to the vector of error 
 * messages.
 */
void
IDFValidator::checkFUImplementations() {

    const Machine::FunctionUnitNavigator fuNav =
        machine_.functionUnitNavigator();

    for (int i = 0; i < fuNav.count(); i++) {
        const FunctionUnit* fu = fuNav.item(i);
        if (!idf_.hasFUImplementation(fu->name())) {
            format errorMsg(
                "IDF does not define an implementation for "
                " function unit %1%.");

            errorMsg % fu->name();
            errorMessages_.push_back(errorMsg.str());
        }
    }

    for (int i = 0; i < idf_.fuImplementationCount(); i++) {
        const FUImplementationLocation& fui = idf_.fuImplementation(i);
        if (!fuNav.hasItem(fui.unitName())) {
            format errorMsg(
                "IDF defines implementation for unknown "
                "function unit %1%.");

            errorMsg % fui.unitName();
            errorMessages_.push_back(errorMsg.str());
        }
    }
}


/**
 * Checks that the IDF defines an implementation for each RF in the machine.
 *
 * If errors are found, error messages are inserted to the vector of error 
 * messages.
 */
void
IDFValidator::checkRFImplementations() {

    const Machine::RegisterFileNavigator rfNav =
        machine_.registerFileNavigator();

    for (int i = 0; i < rfNav.count(); i++) {
        RegisterFile* rf = rfNav.item(i);
        if (!idf_.hasRFImplementation(rf->name())) {
            format errorMsg(
                "IDF does not define an implementation for "
                "register file %1%.");

            errorMsg % rf->name();
            errorMessages_.push_back(errorMsg.str());
        }
    }

    for (int i = 0; i < idf_.rfImplementationCount(); i++) {
        const RFImplementationLocation& rfi = idf_.rfImplementation(i);
        if (!rfNav.hasItem(rfi.unitName())) {
            format errorMsg(
                "IDF defines implementation for unknown register file %1%.");

            errorMsg % rfi.unitName();
            errorMessages_.push_back(errorMsg.str());
        }
    }
}


/**
 * Checks that the IDF defines an implementation for each IU in the machine.
 *
 * If errors are found, error messages are inserted to the vector of error 
 * messages.
 */
void
IDFValidator::checkIUImplementations() {

    const Machine::ImmediateUnitNavigator iuNav = 
        machine_.immediateUnitNavigator();

    for (int i = 0; i < iuNav.count(); i++) {
        ImmediateUnit* iu = iuNav.item(i);
        if (!idf_.hasIUImplementation(iu->name())) {
            format errorMsg(
                "IDF does not define an implementation for "
                "immediate unit %1%.");

            errorMsg % iu->name();
            errorMessages_.push_back(errorMsg.str());
        }
    }

    for (int i = 0; i < idf_.iuImplementationCount(); i++) {
        const RFImplementationLocation& iui = idf_.iuImplementation(i);
        if (!iuNav.hasItem(iui.unitName())) {
            format errorMsg(
                "IDF defines implementation for unknown immediate unit %1%.");

            errorMsg % iui.unitName();
            errorMessages_.push_back(errorMsg.str());
        }
    }
}


/**
 * Static helper function for removing implementations to units that don't
 * exist in the machine.
 *
 * @param idf IDF to remove extraneous implementations from.
 * @param machine Machine to check the IDF against.
 */
void
IDFValidator::removeUnknownImplementations(
    IDF::MachineImplementation& idf,
    const TTAMachine::Machine& machine) {

    const Machine::ImmediateUnitNavigator iuNav =
        machine.immediateUnitNavigator();

    const Machine::RegisterFileNavigator rfNav =
        machine.registerFileNavigator();

    const Machine::FunctionUnitNavigator fuNav =
        machine.functionUnitNavigator();

    bool ok = false;
    while (!ok) {
        for (int i = 0; i < idf.iuImplementationCount(); i++) {
            const RFImplementationLocation& iui = idf.iuImplementation(i);
            if (!iuNav.hasItem(iui.unitName())) {
                idf.removeIUImplementation(iui.unitName());
                break;
            }
        }
        ok = true;
    }

    ok = false;
    while (!ok) {
        for (int i = 0; i < idf.rfImplementationCount(); i++) {
            const RFImplementationLocation& rfi = idf.rfImplementation(i);
            if (!rfNav.hasItem(rfi.unitName())) {
                idf.removeRFImplementation(rfi.unitName());
                break;
            }
        }
        ok = true;
    }

    ok = false;
    while (!ok) {
        for (int i = 0; i < idf.fuImplementationCount(); i++) {
            const FUImplementationLocation& fui = idf.fuImplementation(i);
            if (!fuNav.hasItem(fui.unitName())) {
                idf.removeFUImplementation(fui.unitName());
                break;
            }
        }
        ok = true;
    }
}
