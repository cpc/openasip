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
 * @file GPRGuardEncoding.cc
 *
 * Implementation of GPRGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "GPRGuardEncoding.hh"
#include "GuardField.hh"
#include "ObjectState.hh"

using std::string;

const std::string GPRGuardEncoding::OSNAME_GPR_GUARD_ENCODING =
    "gpr_guard_enc";
const std::string GPRGuardEncoding::OSKEY_RF_NAME = "rf_name";
const std::string GPRGuardEncoding::OSKEY_REGISTER_INDEX = "reg_index";

/**
 * The constructor.
 *
 * Creates a guard encoding and Registers it into the given guard field
 * automatically. The guard expression is identified by the name of register
 * file, the index of the GPR and the "invert" flag, all given as parameters.
 *
 * @param regFile Name of the register file.
 * @param index The index the GPR.
 * @param inverted The "invert" flag.
 * @param encoding The control code of the guard expression.
 * @param parent The parent guard field.
 * @exception ObjectAlreadyExists If the guard expression is already encoded
 *                                in the parent field, or if the given control
 *                                code is already assigned to another guard
 *                                expression.
 */
GPRGuardEncoding::GPRGuardEncoding(
    const std::string& regFile,
    int index,
    bool inverted,
    unsigned int encoding,
    GuardField& parent)
    throw (ObjectAlreadyExists) :
    GuardEncoding(inverted, encoding), regFile_(regFile), index_(index) {

    parent.addGuardEncoding(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parent The parent guard field.
 * @exception ObjectStateLoadingException If an error occurs while loading the
 *                                        state.
 * @exception ObjectAlreadyExists If the guard expression is already encoded
 *                                in the parent field, or if the given control
 *                                code is already assigned to another guard
 *                                expression.
 */
GPRGuardEncoding::GPRGuardEncoding(
    const ObjectState* state,
    GuardField& parent)
    throw (ObjectStateLoadingException, ObjectAlreadyExists) :
    GuardEncoding(state), regFile_(""), index_(0) {

    const string procName = "GPRGuardEncoding::GPRGuardEncoding";

    if (state->name() != OSNAME_GPR_GUARD_ENCODING) {
	throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
	regFile_ = state->stringAttribute(OSKEY_RF_NAME);
	index_ = state->intAttribute(OSKEY_REGISTER_INDEX);
    } catch (const Exception& exception) {
	throw ObjectStateLoadingException(
	    __FILE__, __LINE__, procName, exception.errorMessage());
    }

    parent.addGuardEncoding(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
GPRGuardEncoding::~GPRGuardEncoding() {
    GuardField* parent = this->parent();
    setParent(NULL);
    parent->removeGuardEncoding(*this);
}


/**
 * Returns the name of the register file that contains the GPR of this guard
 * expression.
 *
 * @return The name of the register file.
 */
std::string
GPRGuardEncoding::registerFile() const {
    return regFile_;
}


/**
 * Returns the index of the GPR of this guard expression.
 *
 * @return The index of the GPR.
 */
int
GPRGuardEncoding::registerIndex() const {
    return index_;
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
GPRGuardEncoding::saveState() const {
    ObjectState* state = GuardEncoding::saveState();
    state->setName(OSNAME_GPR_GUARD_ENCODING);
    state->setAttribute(OSKEY_RF_NAME, registerFile());
    state->setAttribute(OSKEY_REGISTER_INDEX, registerIndex());
    return state;
}
