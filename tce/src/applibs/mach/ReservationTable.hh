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
 * @file ReservationTable.hh
 *
 * Declaration of ReservationTable class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESERVATION_TABLE_HH
#define TTA_RESERVATION_TABLE_HH

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
