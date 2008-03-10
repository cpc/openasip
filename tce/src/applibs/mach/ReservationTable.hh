/**
 * @file ReservationTable.hh
 *
 * Declaration of ReservationTable class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESERVATION_TABLE_HH
#define TTA_RESERVATION_TABLE_HH

#include "ExecutionPipeline.hh"
#include "BitMatrix.hh"

namespace TTAMachine {
    class FunctionUnit;
    class HWOperation;
}

/**
 * Models pipeline resources using a Conventional Reservation Table (CRT).
 *
 * An efficient implementation to avoid all kinds of function overheads etc.
 * All simulation operations can be done by calling inline methods.
 */
class ReservationTable : public BitMatrix {
public:

    typedef int ResourceID;

    ReservationTable(const TTAMachine::FunctionUnit& fu);
    ReservationTable(const TTAMachine::HWOperation& operation);
    virtual ~ReservationTable();

    void init(const TTAMachine::FunctionUnit& fu);

    bool conflictsWith(
        const ReservationTable& anotherReservationTable, 
        int cycle) const;

    using BitMatrix::conflictsWith;

    void issueOperation(ReservationTable& anotherReservationTable);
    void advanceCycle();

    bool isReserved(ResourceID resource, int cycle) const;
};

#include "ReservationTable.icc"

#endif
