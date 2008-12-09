/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file StopPointManager.cc
 *
 * Definition of StopPointManager class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>
#include <map>

#include "Exception.hh"
#include "StopPointManager.hh"
#include "SimulationEventHandler.hh"
#include "SimulationController.hh"
#include "MapTools.hh"
#include "Application.hh"

using std::string;
using std::vector;
using std::map;
using std::make_pair;


/**
 * Constructor.
 *
 * @param controller The simulation controller the stop points should use to
 *                   stop the simulation.
 */
StopPointManager::StopPointManager(
    TTASimulationController& controller, 
    SimulationEventHandler& eventHandler) :
    handleCount_(0), lastStopCycle_(0), controller_(controller),
    eventHandler_(eventHandler) {
}


/**
 * Destructor.
 */
StopPointManager::~StopPointManager() {
    MapTools::deleteAllValues(stopPoints_);
}


/**
 * Function that adds a new stop point.
 *
 * Copies the given stop point.
 *
 * @param stopPoint The stop point to be added.
 * @return Handle for the added stop point.
 */
unsigned int
StopPointManager::add(const StopPoint& stopPoint) {

    StopPoint* toAdd = stopPoint.copy();

    ++handleCount_;

    stopPoints_.insert(make_pair(handleCount_, toAdd));
    handles_.push_back(handleCount_);

    toAdd->setEnabled(true);

    if (stopPoints_.size() == 1) {
        // this is the first added stop point, enable event
        // listening
        eventHandler_.registerListener(
            SimulationEventHandler::SE_NEW_INSTRUCTION, this);
    }
    return handleCount_;
}


/**
 * Remove a stop point by the given handle.
 *
 * @param handle The handle of the stop point to be removed.
 */
void
StopPointManager::deleteStopPoint(unsigned int handle)
    throw (InstanceNotFound) {

    StopPoint* stopPoint = findStopPoint(handle);

    StopPointIndex::iterator findResult = stopPoints_.find(handle);
    stopPoints_.erase(findResult);

    for (HandleContainer::iterator i = handles_.begin(); i != handles_.end(); 
         i++) {
        if ((*i) == handle) {
            handles_.erase(i);
            break;
        }
    }

    delete stopPoint;
    stopPoint = NULL;

    if (stopPoints_.size() == 0) {
        // this was the last stop point, disable event listening
        eventHandler_.unregisterListener(
            SimulationEventHandler::SE_NEW_INSTRUCTION, this);
    }
}


/**
 * Removes all stop points.
 */
void
StopPointManager::deleteAll() {
    while (handles_.size() > 0) {
        deleteStopPoint(handles_.at(0));
    }
}


/**
 * Enables the stop point by the given handle.
 *
 * @param handle The handle to identify the stop point.
 */
void
StopPointManager::enable(unsigned int handle)
    throw (InstanceNotFound) {
    findStopPoint(handle)->setEnabled(true);
}


/**
 * Enables all stop points.
 */
void
StopPointManager::enableAll() {
    for (unsigned int i = 0; i < handles_.size(); i++) {
        enable(handles_.at(i));
    }
}


/**
 * Enables the stop point by the given handle and sets it to be deleted
 * after being triggered.
 *
 * @param handle The handle for the stop point.
 * @throw InstanceNotFound if the given handle cannot be found.
 */
void
StopPointManager::enableOnceAndDelete(unsigned int handle)
    throw (InstanceNotFound) {

    StopPoint* stopPoint = findStopPoint(handle);

    stopPoint->setEnabled(true);
    stopPoint->setDeletedAfterTriggered(true);
}


/**
 * Enables the stop point by the given handle and sets it to be disabled
 * after being triggered.
 *
 * @param handle The handle for the stop point.
 * @throw InstanceNotFound if the given handle cannot be found.
 */
void
StopPointManager::enableOnceAndDisable(unsigned int handle)
    throw (InstanceNotFound) {

    StopPoint* stopPoint = findStopPoint(handle);

    stopPoint->setEnabled(true);
    stopPoint->setDisabledAfterTriggered(true);
}


/**
 * Disables the stop point by the given handle.
 *
 * @param handle The handle for the stop point.
 */
void
StopPointManager::disable(unsigned int handle) 
    throw (InstanceNotFound) {
    findStopPoint(handle)->setEnabled(false);
}


/**
 * Disables all stop oints.
 */
void
StopPointManager::disableAll() {
    for (unsigned int i = 0; i < handles_.size(); i++) {
        disable(handles_.at(i));
    }
}


/**
 * Returns the handle of the stop point by the given index.
 *
 * The index is not a direct index to the container used to store stop points,
 * but more just a way to go through all of the stop points in the manager.
 * A stop point may not have the same index every call.
 *
 * All of the stop points can be accessed by going through the stop points
 * with the indices between 0 and stopPointCount - 1, inclusive. The
 * stop points are in no particular order.
 *
 * @param index The index.
 * @return A Copy of the stop point by the given index.
 * @throw OutOfRange If there is no stop point by the given index.
 */
unsigned int
StopPointManager::stopPointHandle(unsigned int index)
    throw (OutOfRange) {

    if (index < stopPointCount()) {
        return handles_.at(index);
    } else {
        /// @todo textgenerator, to be displayed in the UI.
        string msg = "No stop point found by the given index.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
}
/**
 * Returns the stop point with the given handle.
 *
 * @param handle The handle.
 * @return Stop point with the given handle,
 * @throw InstanceNotFound If there is no stop point with the given handle.
 */
const StopPoint&
StopPointManager::stopPointWithHandleConst(unsigned int handle) const
    throw (InstanceNotFound) {

    StopPointIndex::const_iterator containerEnd = stopPoints_.end();
    StopPointIndex::const_iterator findResult = stopPoints_.find(handle);

    if (findResult == containerEnd) {
        /// @todo textgenerator, to be displayed in the UI.
        std::string msg = "No stop point found by the given handle.";
        throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);        
    } else {
        return *findResult->second;
    }
}


/**
 * Returns the current number of stop points in the manager.
 *
 * Can be used to access all stop points in the manager.
 *
 * @return The number of stop points in the manager.
 */
unsigned int
StopPointManager::stopPointCount() {
    return handles_.size();
}


/**
 * Sets the number of times the stop point by the given handle should not
 * be triggered when the condition is met.
 *
 * @param handle The handle for the stop point.
 * @param count The number of times the stop point should be ignored.
 * @exception InstanceNotFound If no stop point with given handle is found.
 */
void
StopPointManager::setIgnore(unsigned int handle, unsigned int count)
    throw (InstanceNotFound) {
    findStopPoint(handle)->setIgnoreCount(count);
}


/**
 * Sets the condition of triggering for the stop point by the given handle.
 *
 * @param handle The handle for the stop point.
 * @param condition The condition to be used to determine if the stop point
 *                  should be fired.
 * @exception InstanceNotFound If no stop point with given handle is found.
 */
void
StopPointManager::setCondition(
    unsigned int handle,
    const ConditionScript& condition)
    throw (InstanceNotFound) {
    findStopPoint(handle)->setCondition(condition);
}

/**
 * Removes the condition of triggering of the stop point by the given handle.
 *
 * @param handle The handle for the stop point.
 * @exception InstanceNotFound If no stop point with given handle is found.
 */
void
StopPointManager::removeCondition(unsigned int handle)
    throw (InstanceNotFound) {
    findStopPoint(handle)->removeCondition();
}

/**
 * Tries to find a stop point by the given handle. If no such stop point is
 * found, throws an exception.
 *
 * @param handle The handle for the stop point.
 * @exception InstanceNotFound If no stop point with given handle is found.
 */
StopPoint*
StopPointManager::findStopPoint(unsigned int handle)
    throw (InstanceNotFound) {

    StopPointIndex::iterator containerEnd = stopPoints_.end();
    StopPointIndex::iterator findResult = stopPoints_.find(handle);

    if (findResult == containerEnd) {
        /// @todo textgenerator, to be displayed in the UI.
        string msg = "No stop point found by the given handle.";
        throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);        
    } else {
        return findResult->second;
    }
}

/**
 * Returns the handle of a stop causing stop point with given index in the
 * container of stop causing stop points.
 *
 * Stop causing stop point is a stop point that caused the latest stop of
 * simulator.
 *
 * @param index Index of the stop point in the container.
 * @return the handle of the stop point.
 * @exception OutOfRange
 */
unsigned int 
StopPointManager::stopCausingStopPoint(unsigned int index) const 
    throw (OutOfRange) {

    unsigned int count = 0;
    for (StopPointIndex::const_iterator i = stopPoints_.begin(); 
         i != stopPoints_.end(); ++i) {
        if ((*i).second->isTriggered()) {
            if (count == index)
                return (*i).first;
            ++count;
        }
    }
    throw OutOfRange(
        __FILE__, __LINE__, __func__, "Stop point index out of range.");
}

/**
 * Returns the count of stop causing stop points.
 *
 * Stop causing stop point is a stop point that caused the latest stop of
 * simulator.
 *
 * @return the count of stop points.
 */
unsigned int 
StopPointManager::stopCausingStopPointCount() const {
    if (lastStopCycle_ != controller_.clockCount()) {
        return 0;
    }
    std::size_t count = 0;
    for (StopPointIndex::const_iterator i = stopPoints_.begin(); 
         i != stopPoints_.end(); ++i) {
        if ((*i).second->isTriggered())
            ++count;
    }
    return count;        
}


/**
 * Stops simulation if there is at least one stop point requesting it.
 *
 * Receives SE_NEW_INSTRUCTION events.
 */
void
StopPointManager::handleEvent() {

    // find all the stop points watching the new instruction address
    StopPointIndex::iterator i = stopPoints_.begin();
    HandleContainer toBeDeletedStopPoints;
    while (i != stopPoints_.end()) {
        
        StopPoint& stopPoint = *(*i).second;
        const int handle = (*i).first;
        if (stopPoint.isEnabled() && stopPoint.isTriggered()) {
            // we found a stop point that is triggered at this clock cycle

            if (stopPoint.ignoreCount() == 0 && stopPoint.isConditionOK()) {

                controller_.prepareToStop(SRE_BREAKPOINT);
                lastStopCycle_ = controller_.clockCount();
                if (stopPoint.isDisabledAfterTriggered()) {
                    stopPoint.setEnabled(false);
                }

                if (stopPoint.isDeletedAfterTriggered()) {
                    toBeDeletedStopPoints.push_back(handle);
                }
                
            } else if (stopPoint.isConditionOK()) {
                // decrease the ignore count only if the (possible) condition
                // of the breakpoint is also true
                stopPoint.decreaseIgnoreCount();
            }
        }
        ++i;
    }

    // delete stop points that wanted to be deleted after triggered
    for (size_t i = 0; i < toBeDeletedStopPoints.size(); ++i) {
        deleteStopPoint(toBeDeletedStopPoints[i]);
    }
}

