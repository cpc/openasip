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
 * @file InputPortState.cc
 *
 * Definition of InputPortState class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "InputPortState.hh"
#include "SimValue.hh"
#include "FUState.hh"

using std::string;

/**
 * Constructor.
 *
 * @param parent Parent of the port.
 * @param width The bit width of the port.
 */
InputPortState::InputPortState(
    FUState& parent, int width) : 
    PortState(parent, width) {
}

/**
 * Constructor for port which uses a shared register.
 *
 * @param parent Parent of the port.
 * @param registerToUse The shared register storage.
 */
InputPortState::InputPortState(
    FUState& parent, 
    SimValue& registerToUse) :
    PortState(parent, registerToUse) {
}

/**
 * Destructor.
 */
InputPortState::~InputPortState() {
}

