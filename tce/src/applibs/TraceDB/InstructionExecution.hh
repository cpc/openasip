/**
 * @file InstructionExecution.hh
 *
 * Declaration of InstructionExecution class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pjaaskel@cs.tut.fi)
 */

#ifndef TTA_INSTRUCTION_EXECUTION_HH
#define TTA_INSTRUCTION_EXECUTION_HH
 
#include "Exception.hh"
#include "ExecutionTrace.hh"
#include "RelationalDBQueryResult.hh"
#include "SimulatorConstants.hh"

/**
 * Class used to navigate through the list of all execution cycles.
 *
 * This class is also used to access the data of the pointed record.
 */
class InstructionExecution {
public:
    InstructionExecution(RelationalDBQueryResult* result);
    virtual ~InstructionExecution();

    ClockCycleCount cycle() const throw (NotAvailable);
    InstructionAddress address() const throw (NotAvailable);

    void next() throw (NotAvailable);
    bool hasNext() const;

private:
    RelationalDBQueryResult* result_;
    int addressColumnIndex_;
    int cycleColumnIndex_;
};

#endif
