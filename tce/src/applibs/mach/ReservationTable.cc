/**
 * @file ReservationTable.cc
 *
 * Implementation of ReservationTable class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include <cmath>
#include <sstream>

#include "ReservationTable.hh"
#include "FunctionUnit.hh"
#include "BaseType.hh"
#include "HWOperation.hh"
#include "PipelineElement.hh"

/**
 * Builds an empty reservation table of given width for the given function
 * unit.
 *
 * @param fu The function unit of which resources to build the reservation
 *           table of.
 * @param cycleCount The count of cycle columns.
 */
ReservationTable::ReservationTable(const TTAMachine::FunctionUnit& fu) :
    BitMatrix(
        fu.maxLatency(), 
        fu.operationPortCount() + fu.pipelineElementCount(),
        false) {
}

/**
 * Builds a reservation table for the given operation.
 *
 * Also includes empty resource rows so the reservation table is compatible
 * with the global reservation table for the whole function unit.
 *
 * @param operation The operation of which resources to use.
 */
ReservationTable::ReservationTable(const TTAMachine::HWOperation& operation) :
    BitMatrix(
        operation.parentUnit()->maxLatency(),
        operation.parentUnit()->operationPortCount() + 
        operation.parentUnit()->pipelineElementCount(), false) {

    const TTAMachine::FunctionUnit& fu = *operation.parentUnit();

    // First resource rows represent the input and output ports of the FU.
    for (int p = 0; p < fu.operationPortCount(); ++p) {
        for (int cycle = 0; cycle < columnCount(); ++cycle) {
            setBit(
                cycle, p, operation.pipeline()->isPortUsed(
                    *fu.operationPort(p), cycle));
        }
    }
    // Rest of the rows are pipeline resources.
    for (int r = 0; r < fu.pipelineElementCount(); ++r) {
        for (int cycle = 0; cycle < columnCount(); ++cycle) {
            setBit(
                cycle, fu.operationPortCount() + r, 
                operation.pipeline()->isResourceUsed(
                    fu.pipelineElement(r)->name(), cycle));
        }
    }
}

/**
 * Destructor.
 */
ReservationTable::~ReservationTable() {
}
