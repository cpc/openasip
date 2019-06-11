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
 * @file Watch.hh
 *
 * Declaration of Watch class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_WATCH_HH
#define TTA_WATCH_HH

#include "SimulatorConstants.hh"
#include "BaseType.hh"
#include "StopPoint.hh"
#include "ExpressionScript.hh"

class SimulatorFrontend;

/**
 * Represents a simulation watch point.
 *
 * Watch stops simulation when user-given expression changes its value.
 */
class Watch : public StopPoint {
public:
    Watch(
        const SimulatorFrontend& frontend, 
        const ExpressionScript& expression);
    virtual ~Watch();

    virtual bool isTriggered() const;
    virtual std::string description() const;
    virtual StopPoint* copy() const;

    virtual const ExpressionScript& expression() const;
    virtual void setExpression(const ExpressionScript& expression);

private:
    /// Static copying not allowed (should use copy()).
    Watch(const Watch& source);
    /// The expression that is watched.
    mutable ExpressionScript expression_;
    /// The simulator frontend which is used to fetch the current PC.
    const SimulatorFrontend& frontend_;
    /// Flag which tells whether the watch was triggered in current simulation
    /// cycle.
    mutable bool isTriggered_;
    /// The simulation clock cycle in which the expression was checked the
    /// last time.
    mutable ClockCycleCount lastCheckedCycle_;
};

#endif
