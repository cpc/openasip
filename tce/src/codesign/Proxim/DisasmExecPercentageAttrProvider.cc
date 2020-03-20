/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file DisasmExecPercentageAttrProvider.cc
 *
 * Implementation of DisasmExecPercentageAttrProvider class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/grid.h>
#include <math.h>
#include "DisasmExecPercentageAttrProvider.hh"
#include "TracedSimulatorFrontend.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "ExecutableInstruction.hh"
#include "Move.hh"
#include "Machine.hh"

/**
 * The Constructor.
 *
 * @param simulator Simulator frontend for accessing instructions and execution
 *                  counts.
 */
DisasmExecPercentageAttrProvider::DisasmExecPercentageAttrProvider(
    const TracedSimulatorFrontend& simulator):
    ProximDisasmAttrProvider(), simulator_(simulator) {
}


/**
 * The Destructor.
 */
DisasmExecPercentageAttrProvider::~DisasmExecPercentageAttrProvider() {
}


/**
 * Returns grid cell attributes for cell with given move.
 *
 * @param address Address of the cell's instruction.
 */
wxGridCellAttr*
DisasmExecPercentageAttrProvider::moveCellAttr(
    InstructionAddress address, int move) {
    
    assert(simulator_.isProgramLoaded());

    wxGridCellAttr* attr = new wxGridCellAttr();

    const ExecutableInstruction& ins =
        simulator_.executableInstructionAt(address);

    const TTAProgram::Program& program = simulator_.program();

    const TTAMachine::Machine::BusNavigator& nav =
        program.targetProcessor().busNavigator();

    assert(move >= 0);

    if (move >= nav.count()) {
        // No highlight for immediate slots.
        return attr;
    }

    const TTAMachine::Bus* moveBus = nav.item(move);
    const TTAProgram::Instruction& instruction =
        program.instructionAt(address);

    // Search for the correct move index in the instruction.
    // TTAProgram::Instruction doesn't contain nops and the move indexing
    // differs from the busNavigator and disassembly grid indexing.
    int index = 0;
    for (; index < instruction.moveCount(); index++) {
        if (moveBus == &instruction.move(index).bus()) {
            break;
        }
    }

    ClockCycleCount executions = 0;
    if (index < instruction.moveCount()) {
        executions = ins.moveExecutionCount(index);
    }

    if (executions > 0) {
        ClockCycleCount cycles = simulator_.cycleCount();
        int colour = static_cast<int>(
            5 * 255 * sin((executions / cycles) * 0.5 * 3.1415926));
        if (colour > 255) colour = 255;
        attr->SetBackgroundColour(
            wxColour(255, 255 - colour, 255 - colour));
    } else {
        // Gray background colour for moves that are not executed at all.
        attr->SetBackgroundColour(wxColour(220, 220, 220));
    }
    return attr;
}
