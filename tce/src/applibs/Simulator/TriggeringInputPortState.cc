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
 * @file TriggeringInputPortState.cc
 *
 * Definition of TriggeringInputPortState class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "TriggeringInputPortState.hh"
#include "SimValue.hh"
#include "FUState.hh"

using std::string;

/**
 * Constructor.
 *
 * @param parent Parent of the port.
 * @param width Width of the port.
 */
TriggeringInputPortState::TriggeringInputPortState(
    FUState& parent, 
    int width) : 
    InputPortState(parent, width) {
}

/**
 * Constructor for port which uses a shared register.
 *
 * @param parent Parent of the port.
 * @param name Name of the port.
 * @param registerToUse The shared register storage.
 */
TriggeringInputPortState::TriggeringInputPortState(
    FUState& parent, 
    SimValue& registerToUse) :
    InputPortState(parent, registerToUse) {
}

/**
 * Destructor.
 */
TriggeringInputPortState::~TriggeringInputPortState() {
}

/**
 * Sets the value of the port and triggers an operation.
 *
 * @param value Value to be set.
 */
void
TriggeringInputPortState::setValue(const SimValue& value) {
    parent_->setTriggered();
    RegisterState::setValue(value);
}

