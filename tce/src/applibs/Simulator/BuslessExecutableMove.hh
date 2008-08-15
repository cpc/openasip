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
 * @file BuslessExecutableMove.hh
 *
 * Declaration of BuslessExecutableMove class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUSLESS_EXECUTABLE_MOVE_HH
#define TTA_BUSLESS_EXECUTABLE_MOVE_HH

#include "ExecutableMove.hh"
#include "InlineImmediateValue.hh"

class ReadableState;
class BusState;
class WritableState;
class SimValue;


/**
 * Represents an interpreted move which does not utilize a transport bus.
 *
 * Moves that utilize this class include control unit internal moves such
 * as control flow operations (jump, call) with immediate or RA source. 
 * This class can be used also for simulating data transports of sequential 
 * code, as writing to (the universal) bus is not necessary in that case.
 */
class BuslessExecutableMove : public ExecutableMove {
public:
    BuslessExecutableMove(
        const ReadableState& src, 
        WritableState& dst);

    BuslessExecutableMove(
        const ReadableState& src,
        WritableState& dst,
        const ReadableState& guardReg,
        bool negated);

    BuslessExecutableMove(
        InlineImmediateValue* immediateSource,
        WritableState& dst,
        const ReadableState& guardReg,
        bool negated);

    BuslessExecutableMove(
        InlineImmediateValue* immediateSource,
        WritableState& dst);
    
    virtual ~BuslessExecutableMove();

    virtual void executeRead();
    virtual void executeWrite();

private:
    /// Copying not allowed.
    BuslessExecutableMove(const BuslessExecutableMove&);
    /// Assignment not allowed.
    BuslessExecutableMove& operator=(const BuslessExecutableMove&);
};

#endif
