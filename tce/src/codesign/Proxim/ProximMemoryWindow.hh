/**
 * @file ProximMemoryWindow.cc
 *
 * Declaration of ProximMemoryWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_PROXIM_MEMORY_WINDOW_HH
#define TTA_PROXIM_MEMORY_WINDOW_HH

#include "ProximSimulatorWindow.hh"
#include "SimulatorEvent.hh"
#include "AddressSpace.hh"

class MemorySystem;
class MemoryControl;
class SimulatorFrontend;

/**
 * Proxim subwindow which displays memory contents.
 *
 * This window listens to SimulatorEvents and updates the window
 * contents automatically.
 */
class ProximMemoryWindow : public ProximSimulatorWindow {
public:
    ProximMemoryWindow(ProximMainFrame* parent, int id);
    virtual ~ProximMemoryWindow();
    virtual void reset();
private:
    void onProgramLoaded(const SimulatorEvent& event);
    void onSimulationStop(const SimulatorEvent& event);
    void loadProgramMemory();
    void loadMemory(const TTAMachine::AddressSpace& as);
    void initialize();
    void onASChoice(wxCommandEvent& event);
    void onDisplayRangeChoice(wxCommandEvent& event);
    void createContents();

    /// Memory system to display in teh window.
    MemorySystem* memorySystem_;
    /// MemoryControl widget which displays the memory contents.
    MemoryControl* memoryControl_;
    /// Simulator instance which contains the memory system to display.
    SimulatorFrontend* simulator_;
    /// Toplevel sizer for the window widgets.
    wxBoxSizer* sizer_;
    /// Address space choicer widget.
    wxChoice* asChoice_;
    /// Sizer for address range widgets.
    wxStaticBoxSizer* addressRangeSizer_;
    /// Static text control displaying address space information.
    wxStaticText* asInfoText_;

    // Widget IDs.
    enum {
	ID_AS_CHOICE = 10000,
	ID_TEXT_AS,
        ID_TEXT_AS_INFO,
	ID_TEXT_START_ADDRESS,
	ID_START_ADDRESS,
	ID_TEXT_DISPLAY,
	ID_DISPLAY_SIZE_CHOICE,
	ID_LINE
    };
    DECLARE_EVENT_TABLE()
};

#endif
