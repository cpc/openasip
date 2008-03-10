/**
 * @file RootEditPart.cc
 *
 * Definition of RootEditPart class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
