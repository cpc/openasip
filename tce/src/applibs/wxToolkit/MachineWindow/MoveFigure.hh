/**
 * @file MoveFigure.hh
 *
 * Declaration of MoveFigure class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MOVE_FIGURE_HH
#define TTA_MOVE_FIGURE_HH

#include "Figure.hh"

/**
 * Figure of a move.
 */
class MoveFigure : public Figure {
public:
    MoveFigure(Figure* bus, Figure* source, Figure* target);
    virtual ~MoveFigure();

protected:
    virtual void drawSelf(wxDC* dc);

private:
    /// Default colour of the figure.
    static const wxColour DEFAULT_COLOUR;
    /// Figure of the bus where the move is done.
    Figure* bus_;
    /// Figure of the source port.
    Figure* source_;
    /// Figure of the TargetPort.
    Figure* target_;

};

#endif
