/**
 * @file BusContainerFigure.hh
 *
 * Declaration of BusContainerFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#ifndef TTA_BUS_CONTAINER_FIGURE_HH
#define TTA_BUS_CONTAINER_FIGURE_HH

#include <wx/wx.h>
#include <vector>

#include "Figure.hh"

class wxDC;

/**
 * Figure of a container EditPart for buses.
 */
class BusContainerFigure : public Figure {
public:
    BusContainerFigure();
    virtual ~BusContainerFigure();

protected:
    virtual void layoutSelf(wxDC*);
    virtual void layoutChildren(wxDC*);

private:
    /// Assignment not allowed.
    BusContainerFigure& operator=(BusContainerFigure& old);
    /// Copying not allowed.
    BusContainerFigure(BusContainerFigure& old);
};

#endif
