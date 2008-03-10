/**
 * @file AssignmentQueue.hh
 *
 * Declaration of AssignmentQueue class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
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
