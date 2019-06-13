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
 * @file MachineCheck.cc
 *
 * Implementation of MachineCheck class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "MachineCheck.hh"


/**
 * The Constructor.
 *
 * @param shortDesc The short description of the check.
 */
MachineCheck::MachineCheck(const std::string& shortDesc):
    shortDesc_(shortDesc) {
}

/**
 * The Destructor.
 */
MachineCheck::~MachineCheck() {
}


/**
 * Returns short description of the checker.
 */
std::string
MachineCheck::shortDescription() const {
    return shortDesc_;
}


/**
 * Returns description of the checker.
 *
 * Base class implementation returns the checker short description
 * as the long description.
 */
std::string
MachineCheck::description() const {
    return shortDescription();
}

/**
 * Returns true if the checker can automatically fix the machine to pass
 * the check.
 *
 * @return Base class implementation returns always false.
 */
bool
MachineCheck::canFix(const TTAMachine::Machine&) const {
    return false;
}

/**
 * Automatically fixes the machine to pass the test.
 *
 * The base class implementation always throws InvalidObject.
 *
 * @return Short description of the modifications to the machine.
 * @throw InvalidData If the machine can't be fixed automatically.
 */
std::string
MachineCheck::fix(TTAMachine::Machine&) const {
    throw InvalidData(__FILE__, __LINE__, __func__);
}
