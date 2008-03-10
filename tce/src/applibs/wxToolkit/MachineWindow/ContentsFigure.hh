/**
 * @file ContentsFigure.hh
 *
 * Declaration of ContentsFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#ifndef TTA_CONTENTS_FIGURE_HH
#define TTA_CONTENTS_FIGURE_HH

#include <wx/wx.h>

#include "Figure.hh"

class wxDC;
class UnitContainerFigure;
class BusContainerFigure;
class SocketContainerFigure;

/**
 * Figure of a container EditPart for the whole machine.
 */
class ContentsFigure : public Figure {
public:
    ContentsFigure();
    virtual ~ContentsFigure();

    virtual void addChild(Figure* child);

protected:
    virtual void layoutSelf(wxDC*);
    virtual void layoutChildren(wxDC*);
    virtual void draw(wxDC* dc);
private:
    /// Assignment not allowed.
    ContentsFigure& operator=(ContentsFigure& old);
    /// Copying not allowed.
    ContentsFigure(ContentsFigure& old);

    /// Contains all the units.
    UnitContainerFigure* units_;
    /// Contains all the buses.
    BusContainerFigure* buses_;
    /// Contains all the sockets.
    SocketContainerFigure* sockets_;
};

#endif
