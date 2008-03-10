/**
 * @file MDFView.hh
 *
 * Declaration of MDFView class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2003 (vjaaskel@cs.tut.fi)
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
