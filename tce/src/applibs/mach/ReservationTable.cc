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
