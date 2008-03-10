/**
 * @file ConnectionEditPart.cc
 *
 * Definition of ConnectionEditPart class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#include "Application.hh"
#include "ConnectionEditPart.hh"
#include "ConnectionFigure.hh"

/**
 * The Constructor.
 */
ConnectionEditPart::ConnectionEditPart(): source_(NULL), target_(NULL) {
}

/**
 * The Destructor.
 */
ConnectionEditPart::~ConnectionEditPart() {
}

/**
 * Sets the source of this connection.
 *
 * @param source New source.
 */
void
ConnectionEditPart::setSource(EditPart* source) {
    assert(figure_ != NULL);
    ConnectionFigure* fig = dynamic_cast<ConnectionFigure*>(figure_);
    assert(fig != NULL);
    fig->setSource(source->figure());
    figure_ = fig;
    source_ = source;
}

/**
 * Sets the target of this connection.
 *
 * @param target New target.
 */
void
ConnectionEditPart::setTarget(EditPart* target) {
    assert(figure_ != NULL);
    ConnectionFigure* fig = dynamic_cast<ConnectionFigure*>(figure_);
    assert(fig != NULL);
    fig->setTarget(target->figure());
    figure_ = fig;
    target_ = target;
}
