/**
 * @file ProximDisasmAttrProvider.cc
 *
 * Implementation ProximDisasmAttrProvider class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
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
