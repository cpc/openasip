/*
    Copyright (C) 2026 Tampere University.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301 USA
 */
/**
 * @file ExplorationTools.hh
 *
 * Declaration of ExplorationTools class that includes some helper functions
 * used in automated design space exploration.
 *
 * @author Alex Hirvonen (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */

#ifndef TTA_EXPLORATION_TOOLS_HH
#define TTA_EXPLORATION_TOOLS_HH

#include "Machine.hh"
#include "Operation.hh"

using namespace TTAMachine;

class ExplorationTools {
public:
    static FunctionUnit*
    createOperationFU(Machine* mach, const Operation& osalOp);

    static void
    connectPortToBus(Port* port, Bus* bus, Socket::Direction direction);

    static std::vector<Operation> getArchOperations(Machine* mach);
    static int opCycleStart(TCEString opName, bool isReadCycle);
    static std::vector<TCEString> sortRFsByWidth(Machine* mach);
};

#endif
