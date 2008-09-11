/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file MoveElement.cc
 *
 * Non-inline definitions of MoveElement class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cc.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
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
