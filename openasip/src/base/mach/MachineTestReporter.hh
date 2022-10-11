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
 * @file MachineTestReporter.hh
 *
 * Declaration of MachineTestReporter class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef TTA_MACHINE_TEST_REPORTER_HH
#define TTA_MACHINE_TEST_REPORTER_HH

#include <string>

#include "Socket.hh"

class MachineTester;

/**
 * Creates string error messages according to the error stated by
 * MachineTester.
 */
class MachineTestReporter {
public:
    static std::string socketSegmentConnectionError(
        const TTAMachine::Socket& socket,
        const TTAMachine::Segment& segment,
        const MachineTester& tester);
    static std::string socketPortConnectionError(
        const TTAMachine::Socket& socket,
        const TTAMachine::Port& port,
        const MachineTester& tester);
    static std::string bridgingError(
        const TTAMachine::Bus& sourceBus,
        const TTAMachine::Bus& destinationBus,
        const MachineTester& tester);
    static std::string socketDirectionSettingError(
        const TTAMachine::Socket& socket,
        TTAMachine::Socket::Direction,
        const MachineTester& tester);
};

#endif
