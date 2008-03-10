/**
 * @file Proxim.hh
 *
 * Declaration of Proxim class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef PROXIM_HH
#define PROXIM_HH

#include <wx/app.h>
#include "CommandRegistry.hh"

class MainFrame;

class ProximMainFrame;
class wxPanel;
class ProximSimulationThread;
class GUIOptions;

/**
 * Represents the Proxim application.
 *
 * This is the top level class of the application. Derived from the wxApp
 * class, which does the low level initialization of wxWidgets. This class
 * is responsible for the application initialization and termination.
 */
class Proxim : public wxApp {
public:
    Proxim();
    virtual ~Proxim();
    virtual bool OnInit();
    virtual int OnExit();
    ProximSimulationThread* simulation();
    CommandRegistry& commandRegistry();
    GUIOptions& options();

private:
    void loadOptions();
    void createDefaultOptions();

    /// The application main frame.
    ProximMainFrame* mainFrame_;
    /// The application command registry.
    CommandRegistry* commandRegistry_;
    /// The simulation thread running the simulator backend.
    ProximSimulationThread* simulation_;
    /// Options defining toolbar, keyboard shortcuts etc.
    GUIOptions* options_;

    /// Name of the config file.
    static const std::string CONFIG_FILE_NAME;
    /// Name of the xml-schema file.
    static const std::string SCHEMA_FILE_NAME;
};

DECLARE_APP(Proxim)

#endif
