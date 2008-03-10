/**
 * @file MachineCanvasOptions.cc
 *
 * Implementation of MachineCanvasOptions class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "MachineCanvasOptions.hh"

const std::string MachineCanvasOptions::SHOW_UNIT_INFO_STRING =
    "SHOT_UNIT_INFO_STRING";

/**
 * The Constructor.
 */
MachineCanvasOptions::MachineCanvasOptions() : Options() {
    addOptionValue(SHOW_UNIT_INFO_STRING, new BoolOptionValue(true));
}

/**
 * The Destructor.
 */
MachineCanvasOptions::~MachineCanvasOptions() {
}
