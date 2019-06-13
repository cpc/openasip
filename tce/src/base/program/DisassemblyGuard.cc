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
 * @file DisassemblyGuard.cc
 *
 * Implementation of DisassemblyGuard class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyGuard.hh"

/**
 * The constructor.
 *
 * @param guardedElement The element which the guard refers to.
 * @param inverted Defines if the guard is inverted or not.
 */
DisassemblyGuard::DisassemblyGuard(
    DisassemblyElement* guardedElement, bool inverted):
    inverted_(inverted),
    guardedElement_(guardedElement) {
}


/**
 * The destructor.
 */
DisassemblyGuard::~DisassemblyGuard() {
    delete guardedElement_;
}


/**
 * True, if the guard is inverted.
 *
 * @return True if the guard is inverted, false if not.
 */
bool
DisassemblyGuard::inverted() const {
    return inverted_;
}


/**
 * Returns the element which the guard refers to.
 *
 * @return Guarded element.
 */
const DisassemblyElement&
DisassemblyGuard::guardedElement() const {
    return *guardedElement_;
}


/**
 * Returns disassembly of the guard.
 *
 * @return Disassembly of the guard as a string.
 */
std::string
DisassemblyGuard::toString() const {
    std::string disassembly;
    if (inverted_) {
	disassembly = "!";
    } else {
	disassembly = "?";
    }

    disassembly = disassembly + guardedElement_->toString() + " ";
    return disassembly;
}
