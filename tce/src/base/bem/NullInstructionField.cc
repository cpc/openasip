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
 * @file NullInstructionField.cc
 *
 * Implementation of NullInstructionField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "NullInstructionField.hh"
#include "Application.hh"

using std::string;

NullInstructionField NullInstructionField::instance_;

/**
 * The constructor.
 */
NullInstructionField::NullInstructionField() : InstructionField(NULL) {
}


/**
 * The destructor.
 */
NullInstructionField::~NullInstructionField() {
}


/**
 * Returns the only instance of NullInstructionField.
 *
 * @return The only instance of NullInstructionField.
 */
NullInstructionField&
NullInstructionField::instance() {
    return instance_;
}


/**
 * Always returns 0.
 *
 * @return 0.
 */
int
NullInstructionField::childFieldCount() const {
    return 0;
}


/**
 * Always throws OutOfRange exception.
 *
 * @return Never returns.
 * @exception OutOfRange Always thrown.
 */
InstructionField&
NullInstructionField::childField(int) const {
    const string procName = "NullInstructionField::childField";
    throw OutOfRange(__FILE__, __LINE__, procName);
}

/**
 * Always returns 0.
 *
 * @return 0.
 */
int
NullInstructionField::width() const {
    return 0;
}

