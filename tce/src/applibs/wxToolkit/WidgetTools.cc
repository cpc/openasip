/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file WidgetTools.cc
 *
 * Implementation of the WidgetTools class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
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
WidgetTools::setWidgetLabel(wxWindow* widget, string text) {
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
