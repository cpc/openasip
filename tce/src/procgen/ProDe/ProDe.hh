/**
 * @file ProDe.hh
 *
 * Declaration of ProDe class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_MAC_EDIT_HH
#define TTA_MAC_EDIT_HH

#include <wx/app.h>
#include <string>

class wxDocManager;
class wxDocMDIParentFrame;
class MainFrame;
class ProDeOptions;
class CommandRegistry;

/**
 * Represents the ProDe application.
 *
 * This class is responsible for parsing the command line and
 * initializing the editor accordingly. If the editor is executed in
 * interactive mode, a graphical user interface will be
 * initialized. The class is derived from wxApp class, which does the
 * low level initialization of wxWindows automatically.
 */

class ProDe : public wxApp {
public:
    ProDe();
    bool OnInit();
    int OnExit();
    MainFrame* mainFrame() const;
    ProDeOptions* options() const;
    void setOptions(ProDeOptions* options);
    CommandRegistry* commandRegistry() const;
    wxDocManager* docManager() const;
    static std::string bitmapsDirPath();

private:
    void createDefaultOptions();

    /// Manages multiple documents.
    wxDocManager* docManager_;
    /// Main frame of the application.
    MainFrame* mainFrame_;
    /// editor options
    ProDeOptions* options_;
    /// editor command registry
    CommandRegistry* commandRegistry_;
};

DECLARE_APP(ProDe)

#endif
