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
 * @file FUReservationTableIndex.cc
 *
 * Definition of FUReservationTableIndex class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
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

