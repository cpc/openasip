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
