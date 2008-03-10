/**
 * @file OSEd.hh
 *
 * Declaration of OSEd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_HH
#define TTA_OSED_HH

#include <wx/app.h>

#include "OSEdMainFrame.hh"
#include "OSEdOptions.hh"

/**
 * Main class for OSEd (Operation Set Editor).
 *
 * Initializes the application.
 */
class OSEd : public wxApp {
public:
    OSEd();
    virtual ~OSEd();
    
    virtual bool OnInit();
    virtual int OnExit();
    
    OSEdMainFrame* mainFrame() const;
    OSEdOptions* options() const;
    
private:
    /// Copying not allowed.
    OSEd(const OSEd&);
    /// Assignment not allowed.
    OSEd& operator=(const OSEd&);
    
    void createDefaultOptions();

    /// Main window of the application.
    OSEdMainFrame* mainFrame_;
    /// Options of the application.
    OSEdOptions* options_;
};

DECLARE_APP(OSEd)

#endif
