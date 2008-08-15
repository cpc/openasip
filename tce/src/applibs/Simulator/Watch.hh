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
 * @file Watch.hh
 *
 * Declaration of Watch class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
