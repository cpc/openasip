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
 * @file BusState.hh
 *
 * Declaration of BusState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUS_STATE_HH
#define TTA_BUS_STATE_HH

#include <string>

#include "RegisterState.hh"

//////////////////////////////////////////////////////////////////////////////
// BusState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models the state of the bus.
 */
class BusState : public RegisterState {
public:
    BusState(int width);
    virtual ~BusState();

    virtual void setValue(const SimValue& value);
    void setValueInlined(const SimValue& value);

    void clear();

    void setSquashed(bool isSquashed);
    bool isSquashed() const;    

private:
    /// Copying not allowed.
    BusState(const BusState&);
    /// Assignment not allowed.
    BusState& operator=(const BusState&);

    /// Name of the bus.
    std::string name_;
    /// Width of the bus.
    int width_;
    /// True in case this bus was squashed the last time a move was executed
    /// in this bus.
    bool squashed_;
};

//////////////////////////////////////////////////////////////////////////////
// NullBusState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models null BusState object.
 */
class NullBusState : public BusState {
public:
    static NullBusState& instance();
    
    virtual ~NullBusState();

    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;

private:
    NullBusState();
    /// Copying not allowed.
    NullBusState(const NullBusState&);
    /// Assignment not allowed.
    NullBusState& operator=(const NullBusState&);

    /// Unique instance of NullBusState.
    static NullBusState* instance_;
};

#include "BusState.icc"

#endif
