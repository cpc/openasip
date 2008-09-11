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
 * @file OperationState.hh
 *
 * Declaration of OperationState class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#ifndef TTA_OPERATION_STATE_HH
#define TTA_OPERATION_STATE_HH

class OperationContext;
class SimValue;

/**
 * OperationState is the base class of all operation state classes.
 *
 * Operation state classes store state information specific to a family of
 * operations. 
 */
class OperationState {
public:
    OperationState();
    virtual ~OperationState();

    virtual const char* name() = 0;
    virtual bool isAvailable(const OperationContext& context) const;
    virtual void advanceClock(OperationContext& context);
};

//////////////////////////////////////////////////////////////////////////////
// NullOperationState
//////////////////////////////////////////////////////////////////////////////

/**
 * Singleton class that is used to represent a null value for operation state.
 */
class NullOperationState : public OperationState {
public:
    static NullOperationState* instance();

    virtual const char* name();
    virtual bool isAvailable(const OperationContext& context) const;
    virtual void advanceClock(OperationContext& context);

private:
    static NullOperationState* instance_;
};

#include "OperationState.icc"

#endif
