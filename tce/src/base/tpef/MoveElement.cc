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
 * @file MoveElement.cc
 *
 * Non-inline definitions of MoveElement class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cc.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 21 October 2003 by ml, jn, rm, pj
 *
 * @note rating: yellow
 */

#include "MoveElement.hh"

namespace TPEF {

/**
 * Constructor.
 *
 * Move attributes are initialized to default values.
 */
MoveElement::MoveElement() :
    InstructionElement(true), isEmpty_(false), bus_(0), sourceType_(MF_NULL),
    destinationType_(MF_NULL), sourceUnit_(0),
    destinationUnit_(0), sourceIndex_(0), destinationIndex_(0),
    isGuarded_(false), isGuardInverted_(false),
    guardType_(MF_NULL), guardUnit_(0),
    guardIndex_(0) {
}

/**
 * Destructor
 */
MoveElement::~MoveElement() {
}

}
