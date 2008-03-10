/**
 * @file OSEdMainFrame.hh
 *
 * Declaration of class OSEdMainFrame.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_MAIN_FRAME_HH
#define TTA_OSED_MAIN_FRAME_HH

#include <wx/wx.h>
#include <wx/splitter.h>

class CommandRegistry;
class OSEdTreeView;
class OSEdInformer;

/**
 * Main window of the application.
 *
 * Contains a splitted window for showing data and a menu bar for executing
 * actions.
 */
class OSEdMainFrame : public wxFrame {
public:
    OSEdMainFrame(
        const wxString& title,
        const wxPoint& pos,
        const wxSize& size);
    
    virtual ~OSEdMainFrame();
    
    OSEdTreeView* treeView() const;
    CommandRegistry* registry() const;
    OSEdInformer* informer() const;
    wxStatusBar* statusBar() const;

    void updateMenuBar();
    void onCommandEvent(wxCommandEvent& event);

private:
    /// Copying not allowed.
    OSEdMainFrame(const OSEdMainFrame&);
    /// Assignment not allowed.
    OSEdMainFrame& operator=(const OSEdMainFrame&);

    /// Command registry.
    CommandRegistry* registry_;
    /// Tree view.
    OSEdTreeView* treeView_;
    /// Informs listener classes for events that occurs.
    OSEdInformer* informer_;
    /// Status bar of the main window.
    wxStatusBar* statusBar_;
    
    DECLARE_EVENT_TABLE()
};

#endif
