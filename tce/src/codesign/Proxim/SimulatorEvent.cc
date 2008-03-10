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
