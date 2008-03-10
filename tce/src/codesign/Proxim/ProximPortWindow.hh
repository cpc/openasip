/**
 * @file ProximPortWindow.cc
 *
 * Declaration of ProximPortWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_PROXIM_PORT_WINDOW_HH
#define TTA_PROXIM_PORT_WINDOW_HH

#include "ProximUnitWindow.hh"

class ProximMainFrame;

/**
 * Proxim subwindow which displays function unit port values.
 *
 * This window listens to SimulatorEvents and updates the window
 * contents automatically.
 */
class ProximPortWindow : public ProximUnitWindow {
public:
    ProximPortWindow(ProximMainFrame* parent, int id);
    virtual ~ProximPortWindow();
    void showFunctionUnit(const std::string& name);

private:
    virtual void reinitialize();
    virtual void update();
};

#endif
