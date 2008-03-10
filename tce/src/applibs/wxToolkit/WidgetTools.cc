/**
 * @file WidgetTools.cc
 *
 * Implementation of the WidgetTools class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <boost/format.hpp>
#include <string>

#include "WidgetTools.hh"
#include "WxConversion.hh"
#include "TextGenerator.hh"

using std::string;
using boost::format;

/**
 * Sets a wxControl label text.
 *
 * @param widget Widget to set the label to.
 * @param textId Enumerated ID which identifies the text label.
 * @exception WrongSubclass If the widget was not a wxControl.
 */
void
WidgetTools::setWidgetLabel(
    wxWindow* widget, string text) throw(WrongSubclass) {

    // wxControls
    if (widget->IsKindOf(CLASSINFO(wxControl))) {
        wxControl* control = dynamic_cast<wxControl*>(widget);
        assert(widget != NULL);
        wxString label = WxConversion::toWxString(text);
        control->SetLabel(label);
        return;
    }


    string method = "GUITextGenerator::setWidgetLabel()";
    string message = "Widget is not a wxControl, ";
    message += "unable to set the widget label.";
    throw WrongSubclass(__FILE__, __LINE__, method, message);
}


/**
 * Sets a boxsizer label text.
 *
 * @param sizer wxStaticBoxSizer to set the label to.
 * @param textId Enumerated ID which identifies the text label.
 */
void
WidgetTools::setWidgetLabel(
    wxStaticBoxSizer* sizer, string text) {
    wxControl* box = sizer->GetStaticBox();
    wxString label = WxConversion::toWxString(text);
    box->SetLabel(label);
}


/**
 * Sets a widget label text.
 *
 * @param generator TextGenerator which provides the label text.
 * @param widget Widget to set the label to.
 * @param textID Text generator string ID for the label text.
 */
void
WidgetTools::setLabel(Texts::TextGenerator* generator, wxWindow* widget,
                      int textID) {

    format fmt = generator->text(textID);
    setWidgetLabel(widget, fmt.str());
}


/**
 * Returns wxListCtrl's first selected item as a string.
 *
 * @param list List to search for selected item.
 * @param column Column of the string to return.
 * @return Selected list item string, or empty string if no item is selected.
 */
std::string
WidgetTools::lcStringSelection(wxListCtrl* list, int column) {

    long item = -1;
    item = list->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        return "";
    }

    wxListItem info;
    info.SetId(item);
    info.SetColumn(column);
    list->GetItem(info);

    return WxConversion::toString(info.GetText());
}
