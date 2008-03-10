/**
 * @file SelectionFigure.hh
 *
 * Declaration of SelectionFigure class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SELECTION_FIGURE_HH
#define TTA_SELECTION_FIGURE_HH

#include <wx/wx.h>

#include "Figure.hh"

class wxDC;

/**
 * Figure of a component selection.
 */
class SelectionFigure : public Figure {
public:
    SelectionFigure(Figure* selection);
    virtual ~SelectionFigure();
    void setSelection(Figure* selection);
    virtual wxRect bounds() const;

protected:
    virtual void drawSelf(wxDC* dc);

private:
    /// Assignment not allowed.
    SelectionFigure& operator=(SelectionFigure& old);
    /// Copying not allowed.
    SelectionFigure(SelectionFigure& old);

    /// Selected part figure.
    Figure* selection_;

    /// Width of the margin between the part figure and selection box.
    static const int SELECT_BOX_MARGIN;
};

#endif
