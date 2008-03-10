/**
 * @file MoveElement.cc
 *
 * Non-inline definitions of MoveElement class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cc.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @note reviewed 21 October 2003 by ml, jn, rm, pj
 *
 * @note rating: yellow
 */

#include "MoveElement.hh"

namespace TPEF {

/**
 * Constructor.
 *
 * Move attributes are initialized to default values.
 */
MoveElement::MoveElement() :
    InstructionElement(true), isEmpty_(false), bus_(0), sourceType_(MF_NULL),
    destinationType_(MF_NULL), sourceUnit_(0),
    destinationUnit_(0), sourceIndex_(0), destinationIndex_(0),
    isGuarded_(false), isGuardInverted_(false),
    guardType_(MF_NULL), guardUnit_(0),
    guardIndex_(0) {
}

/**
 * Destructor
 */
MoveElement::~MoveElement() {
}

}
