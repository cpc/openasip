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
 * @file RootEditPart.cc
 *
 * Definition of RootEditPart class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#include <set>

#include "Application.hh"
#include "RootEditPart.hh"
#include "Figure.hh"

using std::set;

/**
 * The Constructor.
 */
RootEditPart::RootEditPart(): EditPart(), contents_(NULL) {
}

/**
 * The Destructor.
 *
 * Collects garbage from the contents EditPart and deletes it all.
 */
RootEditPart::~RootEditPart() {
    collectTrash();
}

/**
 * Sets the contents EditPart of the whole view.
 *
 * @param contents The new contents EditPart.
 */
void
RootEditPart::setContents(EditPart* contents) {
    collectTrash();
    contents_ = contents;
    garbageCollected_ = false;
}

/**
 * Collects trash from the contents EditPart and deletes it.
 */
void
RootEditPart::collectTrash() {

    if (contents_ != NULL) {

	set<EditPart*> trashbag;
	contents_->putGarbage(trashbag);

	for (set<EditPart*>::iterator i = trashbag.begin();
	     i != trashbag.end(); i++) {
	    delete *i;
	}

    }
    garbageCollected_ = true;
}
