/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
