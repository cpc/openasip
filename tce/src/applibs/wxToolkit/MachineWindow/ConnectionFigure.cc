/**
 * @file ConnectionFigure.cc
 *
 * Definition of ConnectionFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 27 2004 by ml, pj, am
 */

#include <vector>

#include "ConnectionFigure.hh"

/**
 * The Constructor.
 */
ConnectionFigure::ConnectionFigure():
    Figure(), source_(NULL), target_(NULL){
}

/**
 * The Destructor.
 */
ConnectionFigure::~ConnectionFigure() {
}
