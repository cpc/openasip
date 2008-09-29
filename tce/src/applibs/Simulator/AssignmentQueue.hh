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
 * @file AssignmentQueue.hh
 *
 * Declaration of AssignmentQueue class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef ASSIGNMENT_QUEUE_HH
#define ASSIGNMENT_QUEUE_HH

#include <string>
#include <deque>
#include <boost/shared_ptr.hpp>
#include "SimValue.hh"

/**
 * This class is used for delayed assignments of SimValues to given targets.
 * 
 * The implementation uses std::vector for a simple ring buffer
 * Buffer size is directly the maximum possible latency
 * 
 */
class AssignmentQueue {
public:
    AssignmentQueue(int maxLatency);
    ~AssignmentQueue();
    
    void inline addAssignment(const SimValue& assignValue, 
        SimValue* assignTarget,
        int latency);
    
    void inline advanceClock();
    
private:
    /// Copying not allowed.
    AssignmentQueue(const AssignmentQueue&);
    /// Assignment not allowed.
    AssignmentQueue& operator=(const AssignmentQueue&);
    
    /// Type for the assignment: source value -> destination pointer
    typedef std::pair< SimValue, SimValue*> Assignment;
        
    /// Type for the stored assignments
    typedef std::deque< std::deque< Assignment > > Assignments;
    
    /// Our assignment queue
    Assignments assignmentQueue_;
    
    /// Position in the ring buffer
    int position_;
    
    /// Maximum possible latency
    int maxLatency_;
};

#include "AssignmentQueue.icc"

#endif
