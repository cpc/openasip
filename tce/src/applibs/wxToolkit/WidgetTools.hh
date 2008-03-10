/**
 * @file WidgetTools.hh
 *
 * Declaration of the WidgetTools class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_WIDGET_TOOLS_HH
#define TTA_WIDGET_TOOLS_HH

#include <string>

#include "Exception.hh"
#include "TextGenerator.hh"

class wxWindow;
class wxListCtrl;

/**
 * Helper functions for handling wxWidgets controls.
 */
class WidgetTools {
public:
    static void setWidgetLabel(wxWindow* widget, std::string text)
        throw(WrongSubclass);

    static void setWidgetLabel(wxStaticBoxSizer* widget, std::string text);

    static void setLabel(Texts::TextGenerator* generator, wxWindow* widget,
                         int textID);

    static std::string lcStringSelection(wxListCtrl* list, int column);
};

#endif
