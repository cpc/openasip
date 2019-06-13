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
 * @file ProximDisasmAttrProvider.cc
 *
 * Implementation ProximDisasmAttrProvider class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/grid.h>
#include "ProximDisasmAttrProvider.hh"

/**
 * Constructor.
 */
ProximDisasmAttrProvider::ProximDisasmAttrProvider() {
}

/**
 * Destructor.
 */
ProximDisasmAttrProvider::~ProximDisasmAttrProvider() {
}

/**
 * Returns style attribute for the disassembly grid cell with the given move.
 *
 * @param address Address of the cell's instruciton.
 * @param move Index of the cell's move in the instruction.
 * @return Style attribute for the cell with given move.
 */
wxGridCellAttr*
ProximDisasmAttrProvider::moveCellAttr(InstructionAddress, int) {

    return new wxGridCellAttr();
}

/**
 * This function is called when the simulation stops.
 *
 * Implement in the derived class to update the attr provider state if 
 * necessary.
 */
void
ProximDisasmAttrProvider::update() {
}
