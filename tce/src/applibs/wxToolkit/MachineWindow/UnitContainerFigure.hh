/**
 * @file UnitContainerFigure.hh
 *
 * Declaration of UnitContainerFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#ifndef TTA_UNIT_CONTAINER_FIGURE_HH
#define TTA_UNIT_CONTAINER_FIGURE_HH

#include <wx/wx.h>

#include "Figure.hh"

class wxDC;

/**
 * Figure of a container EditPart for units.
 */
class UnitContainerFigure : public Figure {
public:
    UnitContainerFigure();
    virtual ~UnitContainerFigure();

protected:
    virtual void layoutSelf(wxDC*);
    virtual void layoutChildren(wxDC*);

private:
    /// Assignment not allowed.
    UnitContainerFigure& operator=(UnitContainerFigure& old);
    /// Copying not allowed.
    UnitContainerFigure(UnitContainerFigure& old);
};

#endif
