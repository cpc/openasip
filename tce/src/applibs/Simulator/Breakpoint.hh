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
 * @file Breakpoint.hh
 *
 * Declaration of Breakpoint class.
 *
 * @author Atte Oksman 2005 (oksman@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
