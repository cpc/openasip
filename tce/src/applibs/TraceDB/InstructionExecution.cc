/**
 * @file InstructionExecution.cc
 *
 * Definition of InstructionExecution class.
 *
 * @author Pekka Jääskeläinen 2004 (pjaaskel@cs.tut.fi)
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
