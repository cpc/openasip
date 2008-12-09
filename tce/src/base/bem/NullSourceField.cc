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
 * @file NullSourceField.cc
 *
 * Implementation of NullSourceField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "NullSourceField.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"

BinaryEncoding NullSourceField::bem_;
MoveSlot NullSourceField::moveSlot_("NULL", bem_);
NullSourceField NullSourceField::instance_;

/**
 * The constructor.
 */
NullSourceField::NullSourceField() :
    SourceField(BinaryEncoding::LEFT ,moveSlot_) {
}


/**
 * The destructor.
 */
NullSourceField::~NullSourceField() {
}


/**
 * Returns the only instance of NullSourceField.
 *
 * @return The instance.
 */
NullSourceField&
NullSourceField::instance() {
    return instance_;
}
