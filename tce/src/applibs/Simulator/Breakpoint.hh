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
 * @file Breakpoint.hh
 *
 * Declaration of Breakpoint class.
 *
 * @author Atte Oksman 2005 (oksman-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BREAKPOINT_HH
#define TTA_BREAKPOINT_HH

#include "ConditionScript.hh"
#include "SimulatorConstants.hh"
#include "BaseType.hh"
#include "StopPoint.hh"

class SimulatorFrontend;
class SimulationEventHandler;

/**
 * Represents a breakpoint in simulation. 
 *
 * Breakpoint stops simulation when simulation has reached the user set
 * instruction address and general condition for stopping is true.
 */
class Breakpoint : public StopPoint {
public:
    Breakpoint(SimulatorFrontend& frontend, InstructionAddress address);
    virtual ~Breakpoint();

    virtual bool isTriggered() const;
    virtual std::string description() const;
    virtual StopPoint* copy() const;

    virtual InstructionAddress address() const;
    virtual void setAddress(InstructionAddress newAddress);

private:
    /// Static copying not allowed (should use copy()).
    Breakpoint(const Breakpoint& source);
    /// The address of the breakpoint. A breakpoint is fired when PC equals
    /// this address.
    InstructionAddress address_;
    /// The simulator frontend which is used to fetch the current PC.
    SimulatorFrontend& frontend_;
};

#endif
