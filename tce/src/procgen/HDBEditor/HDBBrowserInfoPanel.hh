/**
 * @file HDBBrowserInfoPanel.hh
 *
 * Declaration of HDBBrowserInfoPanel class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_BROWSER_INFO_PANEL_HH
#define TTA_HDB_BROWSER_INFO_PANEL_HH

#include <wx/wx.h>
#include <wx/html/htmlwin.h>

#include "DBTypes.hh"

class HDBToHtml;
namespace HDB {
    class HDBManager;
}

/**
 * Window for displaying HDB element details.
 */
class HDBBrowserInfoPanel : public wxHtmlWindow {
public:
    HDBBrowserInfoPanel(wxWindow* parent, wxWindowID id);
    virtual ~HDBBrowserInfoPanel();

    void setHDB(const HDB::HDBManager& hdb);

    void clear();
    void displayFUEntry(RowID id);
    void displayRFEntry(RowID id);
    void displayBusEntry(RowID id);
    void displaySocketEntry(RowID id);
    void displayFUArchitecture(RowID id);
    void displayRFArchitecture(RowID id);
    void displayFUImplementation(RowID id);
    void displayRFImplementation(RowID id);
    void displayCostFunctionPlugin(RowID id);

private:
    virtual void OnLinkClicked(const wxHtmlLinkInfo& link);

    /// HDB to HTML generator.
    HDBToHtml* htmlGen_;
};

#endif
