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
 * @file BusTracker.hh
 *
 * Declaration of BusTracker class.
 *
 * @author Pekka J��skel�inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @author Henry Linjamäki 2017 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUS_TRACKER_HH
#define TTA_BUS_TRACKER_HH

#include "Listener.hh"
#include "Exception.hh"

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

class SimulationController;
class SimulatorFrontend;

/**
 * Tracks the bus activity.
 *
 * Stores bus data as hexadecimal numbers in a bus trace file in CSV format.
 */
class BusTracker : public Listener {
public:
    BusTracker(
        SimulatorFrontend& frontend,
        std::ostream* traceStream);
    BusTracker(
        SimulatorFrontend& frontend,
        std::ostream& traceStream);
    virtual ~BusTracker();

    virtual void handleEvent();

private:
    static const int COLUMN_WIDTH;
    static const std::string COLUMN_SEPARATOR;
    /// the simulator frontend used to access simulation data
    SimulatorFrontend& frontend_;
    bool ownsTraceStream_;
    std::ostream* traceStream_;
};

#endif
