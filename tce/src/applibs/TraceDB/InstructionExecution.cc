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
 * @file InstructionExecution.cc
 *
 * Definition of InstructionExecution class.
 *
 * @author Pekka Jääskeläinen 2004 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "InstructionExecution.hh"
#include "ExecutionTrace.hh" 
#include "DataObject.hh"

/**
 * Constructor.
 *
 * Object is initialized to point to the first record in the record set,
 * if any.
 *
 * @param result The query result to traverse.
 */
InstructionExecution::InstructionExecution(RelationalDBQueryResult* result) :
    result_(result), addressColumnIndex_(-1), cycleColumnIndex_(-1) {
    cycleColumnIndex_ = result_->column("cycle");
    addressColumnIndex_ = result_->column("address");

    assert(addressColumnIndex_ > -1);
    assert(cycleColumnIndex_ > -1);
    assert(result != NULL);
    
    if (hasNext()) {
        next();
    }
}

/**
 * Destructor.
 */
InstructionExecution::~InstructionExecution() {
    delete result_;
    result_ = NULL;
}

/**
 * Returns the cycle count value of the current record.
 *
 * @return The cycle count.
 * @exception NotAvailable If the current record is empty.
 */
ClockCycleCount 
InstructionExecution::cycle() const throw (NotAvailable) {
    if (&result_->data(cycleColumnIndex_) == &NullDataObject::instance()) {
        const std::string errorMsg =
            "Tried to fetch data from an empty result set.";
        throw NotAvailable(__FILE__, __LINE__, __func__, errorMsg);
    }
    return result_->data(cycleColumnIndex_).integerValue();
}

/**
 * Returns the instruction address value of the current record.
 *
 * @return The instruction address value.
 * @exception NotAvailable If the current record is empty.
 */
InstructionAddress
InstructionExecution::address() const throw (NotAvailable) {
    if (&result_->data(addressColumnIndex_) == &NullDataObject::instance()) {
        const std::string errorMsg =
            "Tried to fetch data from an empty result set.";
        throw NotAvailable(__FILE__, __LINE__, __func__, errorMsg);
    }
    return result_->data(addressColumnIndex_).integerValue();
}

/**
 * Advances the navigator to the next record in the query result set.
 *
 * @exception NotAvailable if there is no more results in the set.
 */
void
InstructionExecution::next() throw (NotAvailable) {
    if (!result_->hasNext()) {
        throw NotAvailable(__FILE__, __LINE__, __func__, "No more results.");
    }
    result_->next();
    
}

/**
 * Returns true if there are more results available.
 *
 * @return True if more results available.
 */
bool
InstructionExecution::hasNext() const {
    return result_->hasNext();
}
