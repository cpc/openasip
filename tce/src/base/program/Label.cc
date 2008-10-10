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
 * @file Label.cc
 *
 * Implementation of Label class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "Label.hh"
#include "Scope.hh"
#include "NullAddressSpace.hh"

using std::string;
using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// Label
/////////////////////////////////////////////////////////////////////////////

Label::Label(): address_(Address(0, NullAddressSpace::instance())) {
}

/**
 * The constructor.
 *
 * Registers this label to the owning scope.
 *
 * @param name Name of the label. Must be unique within the owning scope.
 * @param address The address of the location corresponding to this label.
 * @param scope The innermost scope that contains this label.
 */
Label::Label(const std::string& name, Address address, const Scope& scope):
    name_(name), address_(address), scope_(&scope) {
}

/**
 * The destructor.
 */
Label::~Label() {
}

/**
 * Returns the name of this label.
 *
 * @return The name of this label.
 */
string
Label::name() const {
    return name_;
}

/**
 * Returns the address of the location corresponding to this label.
 *
 * @return The address of the location corresponding to this label.
 */
Address
Label::address() const {
    return address_;
}

/**
 * Returns the innermost scope that contains this label.
 *
 * @return The innermost scope that contains this label.
 */
const Scope&
Label::scope() const {
    return *scope_;
}

/**
 * Sets the name of this label.
 */
void
Label::setName(const string& name) {
    name_ = name;
}

/**
 * Sets the address of the location corresponding to this label.
 */
void
Label::setAddress(Address address) {
    address_ = address;
}

/**
 * Sets the innermost scope that contains this label.
 */
void
Label::setScope(const Scope& scope) {
    scope_ = &scope;
}

}
