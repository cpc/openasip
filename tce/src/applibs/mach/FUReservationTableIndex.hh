/**
 * @file FUReservationTableIndex.hh
 *
 * Declaration of FUReservationTableIndex class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_RESERVATION_TABLE_INDEX_HH
#define TTA_FU_RESERVATION_TABLE_INDEX_HH

#include <vector>

class ReservationTable;
namespace TTAMachine {
    class FunctionUnit;
}

/**
 * Represents an index of reservation tables for modeling resource usage of
 * operations in an FU.
 */
class FUReservationTableIndex {
public:
    FUReservationTableIndex(const TTAMachine::FunctionUnit& functionUnit);
    virtual ~FUReservationTableIndex();

    ReservationTable& at(std::size_t operation);
    std::size_t size() const;
private:
    /// Stores all reservation tables.
    std::vector<ReservationTable*> tables_;
};
#endif
