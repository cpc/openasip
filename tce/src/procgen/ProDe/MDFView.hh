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
 * @file MDFView.hh
 *
 * Declaration of MDFView class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2003 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 * @note reviewed Jun 23 2004 by ml, jn, jm, vpj
 */

#ifndef TTA_MDF_VIEW_HH
#define TTA_MDF_VIEW_HH

#include <wx/docview.h>
#include "EditPart.hh"
#include "Figure.hh"

#include "RootEditPart.hh"
#include "EditPartFactory.hh"
#include "ChildFrame.hh"
#include "ProDe.hh"


class MachineCanvas;

/**
 * Viewing and editing component of a MDFDocument.
 *
 * Part of the wxWidgets document/view framework. View of a Machine Object
 * Model consists of EditParts defining the editing behaviour of the
 * model components. For each EditPart in the view exists a figure defining
 * the visualization of the component. The EditParts form a tree hierarchy
 * starting from the top level contents EditPart, which is a child of the
 * RootEditPart. This class also keeps track of the selected EditParts.
 */
class MDFView : public wxView {
public:
    MDFView();
    virtual ~MDFView();
    virtual bool OnCreate(wxDocument* doc, long);
    virtual bool OnClose(bool deleteWindow);
    virtual void OnUpdate(wxView* sender, wxObject* hint);
    virtual void OnDraw(wxDC* dc);
    void clearSelection();
    EditPart* selection();
    virtual void OnActivateView(
        bool activate, wxView* activateView, wxView* deactivateView);

    virtual void OnChangeFilename();
    virtual wxPrintout* OnCreatePrintout();

    MachineCanvas* canvas() const;

private:

    /// Copying forbidden.
    MDFView(const MDFView&);
    /// Assignment forbidden.
    MDFView& operator=(const MDFView&);

    /// The window where the figures are drawn.
    MachineCanvas* canvas_;
    /// ChildFrame for displaying the view.
    ChildFrame* frame_;

    DECLARE_DYNAMIC_CLASS(MDFView)
};

#include "MDFView.icc"

#endif
