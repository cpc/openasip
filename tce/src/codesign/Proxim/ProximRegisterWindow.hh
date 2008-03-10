/**
 * @file ProximRegisterWindow.hh
 *
 * Declaration of ProximRegisterWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_PROXIM_REGISTER_WINDOW_HH
#define TTA_PROXIM_REGISTER_WINDOW_HH

#include "SimulatorEvent.hh"
#include "ProximUnitWindow.hh"

namespace TTAMachine {
    class RegisterFile;
    class ImmediateUnit;
}

class ProximMainFrame;

/**
 * Proxim subwindow which displays register values.
 *
 * This window listens to SimulatorEvents and updates the window
 * contents automatically.
 */
class ProximRegisterWindow : public ProximUnitWindow {
public:
    ProximRegisterWindow(ProximMainFrame* parent, int id);
    virtual ~ProximRegisterWindow();
    void showRegisterFile(const std::string& name);
    void showImmediateUnit(const std::string& name);

private:
    void loadRegisterFile(const TTAMachine::RegisterFile& rf);
    void loadImmediateUnit(const TTAMachine::ImmediateUnit& iu);
    void loadUniversalIntegerRF();
    void loadUniversalFloatRF();
    void loadUniversalBoolRF();
    virtual void reinitialize();
    virtual void update();

    /// Unit choicer string for universal machine integer register file.
    static const wxString UNIVERSAL_INT_RF;
    /// Unit choicer string for universal machine double register file.
    static const wxString UNIVERSAL_DOUBLE_RF;
    /// Unit choicer string for universal machine boolean register file.
    static const wxString UNIVERSAL_BOOL_RF;

    /// Prefix for register files in the unit choicer.
    static const std::string RF_PREFIX;
    /// Prefix for immediate units in the unit choicer.
    static const std::string IMM_PREFIX;
};

#endif
