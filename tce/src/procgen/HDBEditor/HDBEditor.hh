/**
 * @file HDBEditor.hh
 *
 * Declaration of HDBEditor class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_EDITOR_HH
#define TTA_HDB_EDITOR_HH

#include <wx/app.h>

#include "HDBEditorMainFrame.hh"


class HDBBrowserWindow;

/**
 * Main class for HDBEditor.
 *
 * Initializes the application.
 */
class HDBEditor : public wxApp {
public:
    HDBEditor();
    virtual ~HDBEditor();

    virtual bool OnInit();
    virtual int OnExit();

    HDBEditorMainFrame& mainFrame() const;
    HDBBrowserWindow* browser() const;
private:
    /// Copying not allowed.
    HDBEditor(const HDBEditor&);
    /// Assignment not allowed.
    HDBEditor& operator=(const HDBEditor&);
    
    void createDefaultOptions();

    /// Main window of the application.
    HDBEditorMainFrame* mainFrame_;

};

DECLARE_APP(HDBEditor)

#endif
