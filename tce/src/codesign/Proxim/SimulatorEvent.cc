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
 * @file SimulatorEvent.cc
 *
 * Implementation of SimulatorEvent class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "SimulatorEvent.hh"

const wxEventType SimulatorEvent::EVT_SIMULATOR_TERMINATED = wxNewEventType();
const wxEventType SimulatorEvent::EVT_SIMULATOR_START = wxNewEventType();
const wxEventType SimulatorEvent::EVT_SIMULATOR_STOP = wxNewEventType();
const wxEventType SimulatorEvent::EVT_SIMULATOR_RUN = wxNewEventType();
const wxEventType SimulatorEvent::EVT_SIMULATOR_OUTPUT = wxNewEventType();
const wxEventType SimulatorEvent::EVT_SIMULATOR_ERROR = wxNewEventType();
const wxEventType SimulatorEvent::EVT_SIMULATOR_COMMAND = wxNewEventType();
const wxEventType SimulatorEvent::EVT_SIMULATOR_COMMAND_DONE =
    wxNewEventType();

const wxEventType SimulatorEvent::EVT_SIMULATOR_RUNTIME_ERROR =
    wxNewEventType();
const wxEventType SimulatorEvent::EVT_SIMULATOR_RUNTIME_WARNING =
    wxNewEventType();

const wxEventType SimulatorEvent::EVT_SIMULATOR_LOADING_MACHINE =
    wxNewEventType();
const wxEventType SimulatorEvent::EVT_SIMULATOR_MACHINE_LOADED =
    wxNewEventType();
const wxEventType SimulatorEvent::EVT_SIMULATOR_LOADING_PROGRAM =
    wxNewEventType();
const wxEventType SimulatorEvent::EVT_SIMULATOR_PROGRAM_LOADED =
    wxNewEventType();
const wxEventType SimulatorEvent::EVT_SIMULATOR_RESET = wxNewEventType();

/**
 * The Constructor.
 *
 * @param eventType Event type ID.
 * @param data String data associated with the event.
 */
SimulatorEvent::SimulatorEvent(WXTYPE eventType, std::string data) :
    wxEvent(0, eventType), data_(data) {

}


/**
 * The Destructor.
 */
SimulatorEvent::~SimulatorEvent() {
}


/**
 * Returns string data of the event.
 *
 * @return String data associated with the event.
 */
std::string
SimulatorEvent::data() const {
    return data_;
}
