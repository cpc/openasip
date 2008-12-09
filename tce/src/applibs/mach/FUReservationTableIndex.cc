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

