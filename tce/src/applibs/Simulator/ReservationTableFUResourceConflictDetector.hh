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
 * @file ReservationTableFUResourceConflictDetector.hh
 *
 * Declaration of ReservationTableFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CRT_FU_RESOURCE_CONFLICT_DETECTOR_HH
#define TTA_CRT_FU_RESOURCE_CONFLICT_DETECTOR_HH

#include <map>
#include <string>

#include "Exception.hh"
#include "FUResourceConflictDetector.hh"
#include "FUReservationTableIndex.hh"
#include "ReservationTable.hh"

class Operation;
class TCEString;

namespace TTAMachine {
    class FunctionUnit;
}

/**
 * A Conventional Reservation Table implementation of a FU resource conflict 
 * detector.
 */
class ReservationTableFUResourceConflictDetector : 
    public FUResourceConflictDetector {
public:
    ReservationTableFUResourceConflictDetector(
        const TTAMachine::FunctionUnit& fu);
    virtual ~ReservationTableFUResourceConflictDetector();

    virtual bool issueOperation(OperationID id);
    virtual bool advanceCycle();
    virtual void reset();

    bool issueOperationInline(OperationID id);
    bool advanceCycleInline();

    virtual OperationID operationID(const TCEString& operationName) const;

private:
    /// The reservation tables of operations.
    FUReservationTableIndex reservationTables_;
    /// The global reservation table.
    ReservationTable globalReservationTable_;
    /// The modeled FU.
    const TTAMachine::FunctionUnit& fu_;
};

#include "ReservationTableFUResourceConflictDetector.icc"

#endif
