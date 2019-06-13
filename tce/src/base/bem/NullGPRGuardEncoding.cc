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
 * @file NullGPRGuardEncoding.cc
 *
 * Implementation of NullGPRGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "NullGPRGuardEncoding.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "GuardField.hh"

BinaryEncoding NullGPRGuardEncoding::bem_;
MoveSlot NullGPRGuardEncoding::moveSlot_("NULL", bem_);
GuardField NullGPRGuardEncoding::guardField_(moveSlot_);
NullGPRGuardEncoding NullGPRGuardEncoding::instance_;

/**
 * The constructor.
 */
NullGPRGuardEncoding::NullGPRGuardEncoding() :
    GPRGuardEncoding("NULL", 0, false, 0, guardField_) {
}


/**
 * The destructor.
 */
NullGPRGuardEncoding::~NullGPRGuardEncoding() {
}


/**
 * Returns the instance of NullGPRGuardEncoding.
 *
 * @return The instance.
 */
NullGPRGuardEncoding&
NullGPRGuardEncoding::instance() {
    return instance_;
}
