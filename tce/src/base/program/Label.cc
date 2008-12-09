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
