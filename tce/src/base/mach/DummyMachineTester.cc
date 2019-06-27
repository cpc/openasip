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
 * @file DummyMachineTester.cc
 *
 * Implementation of DummyMachineTester class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "DummyMachineTester.hh"

/**
 * The constructor.
 *
 * @param Machine The machine to be tested.
 */
DummyMachineTester::DummyMachineTester(const TTAMachine::Machine& machine) :
    MachineTester(machine) {
}


/**
 * The destructor.
 */
DummyMachineTester::~DummyMachineTester() {
}


/**
 * Always returns true.
 *
 * @return True.
 */
bool
DummyMachineTester::canConnect(
    const TTAMachine::Socket&,
    const TTAMachine::Segment&) {

    return true;
}


/**
 * Always returns true.
 *
 * @return True.
 */
bool
DummyMachineTester::canConnect(
    const TTAMachine::Socket&,
    const TTAMachine::Port&) {

    return true;
}


/**
 * Always returns true.
 *
 * @return True.
 */
bool
DummyMachineTester::canBridge(
    const TTAMachine::Bus&,
    const TTAMachine::Bus&) {

    return true;
}


/**
 * Always returns true.
 *
 * @return True.
 */
bool
DummyMachineTester::canSetDirection(
    const TTAMachine::Socket&,
    TTAMachine::Socket::Direction) {

    return true;
}
