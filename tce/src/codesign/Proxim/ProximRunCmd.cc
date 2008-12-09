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
 * @file ProximRunCmd.cc
 *
 * Implementation of ProximRunCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximRunCmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
ProximRunCmd::ProximRunCmd():
    GUICommand(ProximConstants::COMMAND_NAME_RUN, NULL) {

    simulator_ = wxGetApp().simulation()->frontend();
}

/**
 * The Destructor.
 */
ProximRunCmd::~ProximRunCmd() {
}


/**
 * Executes the command.
 */
bool
ProximRunCmd::Do() {
    wxGetApp().simulation()->lineReader().input("run");
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximRunCmd::icon() const {
    return "run.png";
}


/**
 * Returns ID of this command.
 */
int
ProximRunCmd::id() const {
    return ProximConstants::COMMAND_RUN;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximRunCmd*
ProximRunCmd::create() const {
    return new ProximRunCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the simulation is initialized and ready to run,
 *         false otherwise.
 */
bool
ProximRunCmd::isEnabled() {

    if (simulator_ != NULL && simulator_->isSimulationInitialized()) {
	return true;
    } else {
	return false;
    }
}
