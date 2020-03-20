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
 * @file OSEdInfoView.hh
 *
 * Declaration of OSEdInfoView class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_INFO_VIEW_HH
#define TTA_OSED_INFO_VIEW_HH

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <string>

class Operation;

/**
 * Models an info screen.
 *
 * Info screen shows info about search paths, modules, and operations.
 */
class OSEdInfoView : public wxListCtrl {
public:
    OSEdInfoView(wxWindow* parent);
    virtual ~OSEdInfoView();
    
    void clear();
    void pathView();
    void moduleView(const std::string& name);
    void operationView(const std::string& path, const std::string& mod);
    void operationPropertyView(
        const std::string& opName,
        const std::string& modName,
        const std::string& pathName);

    std::string selectedPath();
    std::string selectedModule();
    std::string selectedOperation();

private:

    void onSelection(wxListEvent& event);
    void onDropDownMenu(wxMouseEvent& event);
    void insertOperationPropertyColumns();
    int  writeStaticPropertiesOfOperation(Operation* op);

    /**
     * Possible modes of info view.
     */
    enum InfoMode {
        MODE_NOMODE,    ///< Initial state.
        MODE_PATH,      ///< Path view.
        MODE_MODULE,    ///< Module view.
        MODE_OPERATION, ///< Operation View.
        MODE_PROPERTY   ///< Operation property view.
    };
    
    /// Mode of the info view.
    InfoMode mode_;

    DECLARE_EVENT_TABLE()
};

#endif
