/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file Stoppoint.hh
 *
 * Declaration of StopPoint class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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

