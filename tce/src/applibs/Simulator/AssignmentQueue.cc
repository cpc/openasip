/**
 * @file AssignmentQueue.cc
 *
 * Definition of AssignmentQueue class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */


#include "AssignmentQueue.hh"
#include <iostream>

/**
 * The constructor
 */
AssignmentQueue::AssignmentQueue(int maxLatency) :
    position_(0), maxLatency_(maxLatency) {
    assignmentQueue_.resize(maxLatency_);
}

/**
 * The destructor
 */
AssignmentQueue::~AssignmentQueue() {
}

