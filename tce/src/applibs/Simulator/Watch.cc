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
 * @file Watch.cc
 *
 * Definition of Watch class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Watch.hh"
#include "ConditionScript.hh"
#include "ExpressionScript.hh"
#include "SimulatorConstants.hh"
#include "BaseType.hh"
#include "Application.hh"
#include "SimulatorFrontend.hh"
#include "Conversion.hh"

/**
 * Constructor.
 *
 * @param frontend Used to fetch the simulation clock.
 * @param expression The expression watched.
 */
Watch::Watch(
    const SimulatorFrontend& frontend, 
    const ExpressionScript& expression) :
    StopPoint(), expression_(expression), frontend_(frontend),
    isTriggered_(false), lastCheckedCycle_(0) {
}

/**
 * Destructor.
 */
Watch::~Watch() {
}

/**
 * Copy method for dynamically bound copies.
 */
StopPoint*
Watch::copy() const {
    StopPoint* aCopy = new Watch(frontend_, expression_);
    if (conditional_) {
        assert(condition_ != NULL);
        ConditionScript* conditionCopy = condition_->copy();
        assert(conditionCopy != NULL);
        aCopy->setCondition(*conditionCopy);
    } else {
        aCopy->removeCondition();
    }
    aCopy->setEnabled(enabled_);
    aCopy->setDisabledAfterTriggered(disabledAfterTriggered_);
    aCopy->setDeletedAfterTriggered(deletedAfterTriggered_);
    aCopy->setIgnoreCount(ignoreCount_);
    return aCopy;
}

/**
 * Returns the expression.
 *
 * @return The expression.
 */
const ExpressionScript&
Watch::expression() const {
    return expression_;
}

/**
 * Sets the expression the watch is watching.
 *
 * @param expression The new expression.
 */
void
Watch::setExpression(const ExpressionScript& expression) {
    expression_ = expression;
}

/**
 * Returns true in case this watch is triggered.
 *
 * Returns true in case the expression has changed in current simulation cycle.
 *
 * @return The status of the watch.
 */
bool 
Watch::isTriggered() const {
    if (lastCheckedCycle_ != frontend_.cycleCount()) {
        // simulation clock has changed since the last expression check,
        // let's see if the watch expression value has changed
        try {
            isTriggered_ = expression_.resultChanged();
        } catch (const Exception&) {
            // for example simulation might not be initialized in every
            // check so the script throws, we'll assume that no triggering
            // should happen at that case
            isTriggered_ = false;
        }
        lastCheckedCycle_ = frontend_.cycleCount();
    }
    return isTriggered_;
}

/**
 * Prints the description string of the stop point.
 *
 * Each subclass overrides this method to construct a descripting string of
 * itself.
 */
std::string 
Watch::description() const {
    return std::string("watch for expression '") + expression_.script().at(0) +
        "' " + StopPoint::description();
}

