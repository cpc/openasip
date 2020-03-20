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
 * @file ProximNextICmd.cc
 *
 * Implementation of ProximNextICmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximNextICmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
ProximNextICmd::ProximNextICmd():
    GUICommand(ProximConstants::COMMAND_NAME_NEXTI, NULL) {

    simulator_ = wxGetApp().simulation()->frontend();
}

/**
 * The Destructor.
 */
ProximNextICmd::~ProximNextICmd() {
}


/**
 * Executes the command.
 */
bool
ProximNextICmd::Do() {
    wxGetApp().simulation()->lineReader().input(
        ProximConstants::SCL_NEXT_INSTRUCTION);
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximNextICmd::icon() const {
    return "nexti.png";
}


/**
 * Returns ID of this command.
 */
int
ProximNextICmd::id() const {
    return ProximConstants::COMMAND_NEXTI;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximNextICmd*
ProximNextICmd::create() const {
    return new ProximNextICmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the simulation is initialized or stopped.
 */
bool
ProximNextICmd::isEnabled() {

    if (simulator_ != NULL &&
        (simulator_->isSimulationInitialized() ||
         simulator_->isSimulationStopped())) {

	return true;
    } else {
	return false;
    }
}
