/**
 * @file HDBEditorMainFrame.hh
 *
 * Declaration of the HDBEditorMainFrame class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_EDITOR_MAIN_FRAME_HH
#define TTA_HDB_EDITOR_MAIN_FRAME_HH

class CommandRegistry;
class HDBBrowserWindow;

#include <wx/frame.h>
#include <string>

namespace HDB {
    class HDBManager;
    class CachedHDBManager;
}

/**
 * Main window of the editor. 
 */
class HDBEditorMainFrame : public wxFrame {
public:
    HDBEditorMainFrame(
        const wxString& title, const wxPoint& position, const wxSize& size);
    virtual ~HDBEditorMainFrame();

    bool setHDB(const std::string& hdbFile);
    bool createHDB(const std::string& filePath);

    HDB::HDBManager* hdbManager();
    HDBBrowserWindow* browser() const;

    void update();

private:
    void onCommandEvent(wxCommandEvent& event);
    void onUpdateUI(wxUpdateUIEvent& event);

    /// Command registry.
    CommandRegistry* commandRegistry_;

    HDB::CachedHDBManager* hdb_;
    HDBBrowserWindow* browser_;

    /// Event table of the MainFrame.
    DECLARE_EVENT_TABLE()
};
#endif
