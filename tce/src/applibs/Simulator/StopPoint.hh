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
 * @file Stoppoint.hh
 *
 * Declaration of StopPoint class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_STOP_POINT_HH
#define TTA_STOP_POINT_HH

#include "ConditionScript.hh"
#include "SimulatorConstants.hh"
#include "BaseType.hh"

class SimulationController;
class SimulationEventHandler;

/**
 * Represents a stop point in simulation. 
 *
 * StopPointManager uses this class to store information of each stop point of
 * the simulation phase. StopPoint is a parent class for different types of
 * user-set simulation stop conditions (currently breakpoints and watches).
 *
 * Each StopPoint listens to a simulation event (usually PC advance) and
 * evaluates its stop condition.
 */
class StopPoint {
public:
    StopPoint();
    virtual ~StopPoint();

    virtual void setEnabled(bool flag);
    virtual bool isEnabled() const;

    virtual void setDisabledAfterTriggered(bool flag);
    virtual bool isDisabledAfterTriggered() const;

    virtual void setDeletedAfterTriggered(bool flag);
    virtual bool isDeletedAfterTriggered() const;
    
    virtual void setCondition(const ConditionScript& condition);
    virtual void removeCondition();
    virtual const ConditionScript& condition() const;
    virtual bool isConditional() const;

    virtual void setIgnoreCount(unsigned int count);
    virtual unsigned int ignoreCount() const;

    virtual bool isConditionOK();
    virtual bool isTriggered() const = 0;
    virtual std::string description() const = 0;
    virtual StopPoint* copy() const = 0;

    virtual void decreaseIgnoreCount();

protected:
    /// Tells whether the breakpoint is enabled or disabled.
    bool enabled_;
    /// Tells if the breakpoint is disabled after it is triggered
    /// the next time.
    bool disabledAfterTriggered_;
    /// Tells if the breakpoint is deleted after it is triggered the next time.
    bool deletedAfterTriggered_;
    /// Tells whether the breakpoint is conditional or not.
    bool conditional_;
    /// The condition which is used to determine whether the breakpoint
    /// should be fired or not.
    ConditionScript* condition_;
    /// The number of times the condition is to be ignored before enabling
    /// the breakpoint.
    unsigned int ignoreCount_;
};

#endif

