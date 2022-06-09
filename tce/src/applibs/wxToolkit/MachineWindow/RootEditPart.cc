/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
