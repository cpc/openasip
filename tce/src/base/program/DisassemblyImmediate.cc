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
 * @file DisassemblyImmediate.cc
 *
 * Implementation of DisassemblyImmediate class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyImmediate.hh"
#include "Conversion.hh"

/**
 * The constructor.
 *
 * @param value Inline immediate value.
 */
DisassemblyImmediate::DisassemblyImmediate(SimValue value, bool signExtend) :
    DisassemblyElement(),
    value_(value), signExtend_(signExtend) {
}


/**
 * The destructor.
 */
DisassemblyImmediate::~DisassemblyImmediate() {
}


/**
 * Returns disassembly of the inline immediate value.
 *
 * @return Disassembly of the inline immediate value as a string.
 */
std::string
DisassemblyImmediate::toString() const {
    if (value_.width() == 0) {
        return "Invalid Immediate";
    }
    // Select the extension mode in the printout depending on the
    // move slots's (bus) extension mode. Wrong extension mode can
    // produce wrong output that cannot be assembled in case
    // of a negative-looking value on a zero extending slot.
    if (signExtend_)
        return Conversion::toString(value_.intValue());
    else
        return Conversion::toString(value_.unsignedValue());
}
