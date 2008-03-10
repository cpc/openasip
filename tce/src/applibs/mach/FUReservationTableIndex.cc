/**
 * @file FUReservationTableIndex.cc
 *
 * Definition of FUReservationTableIndex class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include "FUReservationTableIndex.hh"
#include "ReservationTable.hh"
#include "FunctionUnit.hh"
#include "SequenceTools.hh"
#include "HWOperation.hh"

/**
 * Builds reservation tables for all operations in the FU.
 *
 * Tables are stored in the order returned by FunctionUnit::operation(),
 * the NOP matrix is at index FunctionUnit::operationCount().
 *
 * @param functionUnit The FU to build the matrices for.
 */
FUReservationTableIndex::FUReservationTableIndex(
    const TTAMachine::FunctionUnit& functionUnit) {

    for (int i = 0; i < functionUnit.operationCount(); ++i) {
        const TTAMachine::HWOperation& operation = 
            *functionUnit.operation(i);
        tables_.push_back(new ReservationTable(operation));
    }
}

/**
 * Destructor.
 */
FUReservationTableIndex::~FUReservationTableIndex() {
    SequenceTools::deleteAllItems(tables_);
}

/**
 * Returns the reservation table at the given index.
 *
 * @param operation The index of the reservation table.
 */
ReservationTable&
FUReservationTableIndex::at(std::size_t index) {
    return *tables_.at(index);
}

/**
 * Returns the count of reservation tables in the index.
 *
 * @return The count of reservation tables in the index.
 */
std::size_t
FUReservationTableIndex::size() const {
    return tables_.size();
}

