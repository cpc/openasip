/**
 * @file HDBBrowserInfoPanel.cc
 *
 * Implementation of HDBBrowserInfoPanel class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <string>
#include <sstream>
#include "HDBBrowserInfoPanel.hh"
#include "HDBEditor.hh"
#include "HDBBrowserWindow.hh"
#include "WxConversion.hh"
#include "HDBToHtml.hh"
#include "Application.hh"

using namespace HDB;

/**
 * The Constructor.
 *
 * @param parent Parent window of the panel.
 * @param id Window ientifier for the panel.
 */
HDBBrowserInfoPanel::HDBBrowserInfoPanel(wxWindow* parent, wxWindowID id) :
    wxHtmlWindow(parent, id),
    htmlGen_(NULL) {
}

/**
 * The Destructor.
 */
HDBBrowserInfoPanel::~HDBBrowserInfoPanel() {
    if (htmlGen_ != NULL) {
        delete htmlGen_;
        htmlGen_ = NULL;
    }
}

void
HDBBrowserInfoPanel::OnLinkClicked(const wxHtmlLinkInfo& link) {
    wxString href = link.GetHref();
    wxGetApp().browser()->openLink(href);
}

/**
 * Sets the HDB.
 *
 * @param hdb HDB to generate html from.
 */
void
HDBBrowserInfoPanel::setHDB(const HDB::HDBManager& hdb) {
    if (htmlGen_ != NULL) {
        delete htmlGen_;
    }
    htmlGen_ = new HDBToHtml(hdb);
}

/**
 * Clears the panel contents.
 */
void
HDBBrowserInfoPanel::clear() {
    SetPage(_T("<html><body></body></html>"));
}

/**
 * Displays details of an FU entry in the panel.
 *
 * @param id ID of the FU entry.
 */
void
HDBBrowserInfoPanel::displayFUEntry(RowID id) {
    assert(htmlGen_ != NULL);
    std::stringstream stream;
    htmlGen_->fuEntryToHtml(id, stream);
    std::string page = stream.str();
    SetPage(WxConversion::toWxString(page));
}


/**
 * Displays details of a RF entry in the panel.
 *
 * @param id ID of the register file entry.
 */
void
HDBBrowserInfoPanel::displayRFEntry(RowID id) {
    assert(htmlGen_ != NULL);
    std::stringstream stream;
    htmlGen_->rfEntryToHtml(id, stream);
    std::string page = stream.str();
    SetPage(WxConversion::toWxString(page));
}

/**
 * Displays details of a bus entry in the panel.
 *
 * @param id ID of the bus entry.
 */
void
HDBBrowserInfoPanel::displayBusEntry(RowID id) {
    assert(htmlGen_ != NULL);
    std::stringstream stream;
    htmlGen_->busEntryToHtml(id, stream);
    std::string page = stream.str();
    SetPage(WxConversion::toWxString(page));
}

/**
 * Displays details of a socket entry in the panel.
 *
 * @param id ID of the socket entry.
 */
void
HDBBrowserInfoPanel::displaySocketEntry(RowID id) {
    assert(htmlGen_ != NULL);
    std::stringstream stream;
    htmlGen_->socketEntryToHtml(id, stream);
    std::string page = stream.str();
    SetPage(WxConversion::toWxString(page));
}


/**
 * Displays details of an FU architecture in the panel.
 *
 * @param id ID of the FU architecture.
 */
void
HDBBrowserInfoPanel::displayFUArchitecture(RowID id) {
    assert(htmlGen_ != NULL);
    std::stringstream stream;
    htmlGen_->fuArchToHtml(id, stream);
    std::string page = stream.str();
    SetPage(WxConversion::toWxString(page));
}

/**
 * Displays details of a RF architecture in the panel.
 *
 * @param id ID of the register file architecture.
 */
void
HDBBrowserInfoPanel::displayRFArchitecture(RowID id) {
    assert(htmlGen_ != NULL);
    std::stringstream stream;
    htmlGen_->rfArchToHtml(id, stream);
    std::string page = stream.str();
    SetPage(WxConversion::toWxString(page));
}


/**
 * Displays details of an FU implementation in the panel.
 *
 * @param id ID of the FU implementation.
 */
void
HDBBrowserInfoPanel::displayFUImplementation(RowID id){
    assert(htmlGen_ != NULL);
    std::stringstream stream;
    htmlGen_->fuImplToHtml(id, stream);
    std::string page = stream.str();
    SetPage(WxConversion::toWxString(page));
}


/**
 * Displays details of a RF implementation in the panel.
 *
 * @param id ID of the RF implementation.
 */
void
HDBBrowserInfoPanel::displayRFImplementation(RowID id) {
    assert(htmlGen_ != NULL);
    std::stringstream stream;
    htmlGen_->rfImplToHtml(id, stream);
    std::string page = stream.str();
    SetPage(WxConversion::toWxString(page));
}

/**
 * Displays details of a cost function plugin in the panel.
 *
 * @param id ID of the cost function plugin.
 */
void
HDBBrowserInfoPanel::displayCostFunctionPlugin(RowID id) {
    assert(htmlGen_ != NULL);
    std::stringstream stream;
    htmlGen_->costFunctionPluginToHtml(id, stream);
    std::string page = stream.str();
    SetPage(WxConversion::toWxString(page));
}
