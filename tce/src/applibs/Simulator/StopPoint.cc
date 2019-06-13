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
 * @file StopPoint.cc
 *
 * Definition of StopPoint class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "StopPoint.hh"
#include "ConditionScript.hh"
#include "SimulatorConstants.hh"
#include "BaseType.hh"
#include "Application.hh"
#include "Conversion.hh"

/**
 * Constructor.
 */
StopPoint::StopPoint() :
    enabled_(false), disabledAfterTriggered_(false), 
    deletedAfterTriggered_(false), conditional_(false), condition_(NULL),
    ignoreCount_(0) {
}

/**
 * Destructor.
 */
StopPoint::~StopPoint() {
    delete condition_;
    condition_ = NULL;
}

/**
 * Sets the enabled status of the breakpoint.
 *
 * @param flag The status.
 */
void
StopPoint::setEnabled(bool flag) {
    enabled_ = flag;
}

/**
 * Tells wheter the stop point is enabled or not.
 *
 * @return 'True' if the breakpoint is enabled, 'false' if it is disabled.
 */
bool
StopPoint::isEnabled() const {
    return enabled_;
}


/**
 * Sets the stop point to be disabled after it is triggered the next 
 * time.
 *
 * @param flag The new value for this property.
 */
void
StopPoint::setDisabledAfterTriggered(bool flag) {
    disabledAfterTriggered_ = flag;
}

/**
 * Tells if the stop point is disabled after it is triggered (fired) the
 * next time.
 *
 * @return 'True' if the stop oint is disabled after it is triggered,
 *         'false' otherwise.
 */
bool
StopPoint::isDisabledAfterTriggered() const {
    return disabledAfterTriggered_;
}

/**
 * Makes the stop point to be deleted after it is triggered the next time.
 *
 * @param flag The new value for this property.
 */
void
StopPoint::setDeletedAfterTriggered(bool flag) {
    deletedAfterTriggered_ = flag;
}

/**
 * Tells whether the stop point is deleted after it is triggered (fired)
 * the next time.
 *
 * @return 'True' if the stop point is deleted after it is triggered,
 *         'false' if it is not.
 */
bool
StopPoint::isDeletedAfterTriggered() const {
    return deletedAfterTriggered_;
}

/**
 * Sets the condition of the StopPoint. 
 *
 * The condition is used to determine if the stop point should be fired.
 *
 * @param condition The condition to be used as stop point's condition.
 */
void
StopPoint::setCondition(const ConditionScript& condition) {
    conditional_ = true;
    condition_ = condition.copy();
}

/**
 * Remove the condition of the StopPoint. 
 *
 * The condition is used to determine if the stop point should be fired.
 */
void
StopPoint::removeCondition() {
    conditional_ = false;
    delete condition_;
    condition_ = NULL;
}

/**
 * Returns the condition of the stop point.
 *
 * If breakpoint is unconditional, an empty ConditionScript is returned.
 *
 * @return The condition of the stop point firing.
 */
const ConditionScript&
StopPoint::condition() const {

    static ConditionScript emptyCondition(NULL, "");

    if (conditional_) {
        return *condition_;
    } else {
        return emptyCondition;
    }
}

/**
 * Tells whether the stop point is conditional or not.
 *
 * @return 'True' if the stop point is conditional, 'false' if it is not.
 */
bool
StopPoint::isConditional() const {
    return conditional_;
}

/**
 * Sets the number of times the condition for firing the stop point is
 * to be disabled before enabling it.
 */
void
StopPoint::setIgnoreCount(unsigned int count) {
    ignoreCount_ = count;
}

/**
 * Tells the current ignore count, i.e. the number of times the stop point
 * condition is ignored before being fired.
 *
 * @return The ignore count.
 */
unsigned int
StopPoint::ignoreCount() const {
    return ignoreCount_;
}

/**
 * Decreases the ignore count.
 */
void 
StopPoint::decreaseIgnoreCount() {
    if (ignoreCount() > 0) {
        setIgnoreCount(ignoreCount() - 1);
    }
}

/**
 * Tells wheter the condition of the stop point evalutes to true.
 *
 * If the stop point is not conditional, returns always true.
 *
 * @return 'True' if condition is OK, 'false' otherwise.
 */
bool
StopPoint::isConditionOK() {

    if (conditional_) {
        assert(condition_ != NULL);
        try {
            return condition_->conditionOk();
        } catch (const Exception& e) {
            Application::logStream() 
                << "Condition script threw exception: " << e.errorMessage()
                << std::endl;
            return false;
        }
    } else {
        return true;
    }
}

/**
 * Prints the description string of the stop point.
 *
 * Each subclass overrides this method to construct a descripting string of
 * itself.
 */
std::string 
StopPoint::description() const {
    std::string description = "";
    if (!enabled_)
        description += "is disabled";

    if (disabledAfterTriggered_) {
        if (description != "")
            description += ", ";
        description = "will be disabled after triggered";
    }

    if (deletedAfterTriggered_) {
        if (description != "")
            description += ", ";
        description += "will be deleted after triggered";
    }

    if (ignoreCount_) {
        if (description != "")
            description += ", ";
        description += "will be ignored " + 
            Conversion::toString(ignoreCount_) + " time(s)";
    }

    if (conditional_) {
        if (description != "")
            description += ", ";
        description += "has condition '" + condition_->script().at(0) + "'";
    }

    if (description != "")
        description += " ";

    return description;
}

