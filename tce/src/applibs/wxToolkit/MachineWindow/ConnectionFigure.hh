/**
 * @file ConnectionFigure.hh
 *
 * Declaration of ConnectionFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#ifndef TTA_CONNECTION_FIGURE_HH
#define TTA_CONNECTION_FIGURE_HH

#include "Figure.hh"

class wxDC;

/**
 * Figure of a connection.
 */
class ConnectionFigure : public Figure {
public:
    virtual ~ConnectionFigure();

    void setSource(Figure* figure);
    void setTarget(Figure* figure);
    Figure* source() const;
    Figure* target() const;

protected:
    ConnectionFigure();
    /// Source of connection.
    Figure* source_;
    /// Target of connection.
    Figure* target_;

private:
    /// Assignment not allowed.
    ConnectionFigure& operator=(ConnectionFigure& old);
    /// Copying not allowed.
    ConnectionFigure(ConnectionFigure& old);

};

#include "ConnectionFigure.icc"

#endif
