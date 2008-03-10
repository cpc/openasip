/**
 * @file SocketContainerFigure.hh
 *
 * Declaration of SocketContainerFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#ifndef TTA_SOCKET_CONTAINER_FIGURE_HH
#define TTA_SOCKET_CONTAINER_FIGURE_HH

#include <wx/wx.h>

#include "Figure.hh"

class wxDC;

/**
 * Figure of a container EditPart for sockets.
 */
class SocketContainerFigure : public Figure {
public:
    SocketContainerFigure();
    virtual ~SocketContainerFigure();
    
protected:
    virtual void layoutChildren(wxDC*);
    virtual void layoutSelf(wxDC*);

private:
    /// Assignment not allowed.
    SocketContainerFigure& operator=(SocketContainerFigure& old);
    /// Copying not allowed.
    SocketContainerFigure(SocketContainerFigure& old);
};

#endif
