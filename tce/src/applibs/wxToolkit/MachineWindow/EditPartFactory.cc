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
 * @file EditPartFactory.cc
 *
 * Definition of EditPartFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#include <vector>

#include "EditPart.hh"
#include "EditPartFactory.hh"
#include "SequenceTools.hh"

using std::vector;
using namespace TTAMachine;

vector<EditPart*> EditPartFactory::created_;

/**
 * The Constructor.
 */
EditPartFactory::EditPartFactory(EditPolicyFactory& editPolicyFactory):
    Factory(),
    editPolicyFactory_(editPolicyFactory) {
}

/**
 * The Destructor.
 */
EditPartFactory::~EditPartFactory() {
    // erase data of already created EditParts
    created_.clear();
    SequenceTools::deleteAllItems(factories_);
}

/**
 * Checks if an EditPart of the given component has already been created
 * and returns it.
 *
 * @param component A machine component to check.
 * @return An EditPart of the given machine component if such has already
 *         been created, NULL otherwise.
 */
EditPart*
EditPartFactory::checkCache(const MachinePart* component) const {
    // check if an EditPart has already been created of the component
    vector<EditPart*>::const_iterator i = created_.begin();
    for (; i != created_.end(); i++) {
	if ((*i)->model() == component) {
	    return *i;
	}
    }
    return NULL;
}

/**
 * Inserts an EditPart to the EditPart cache.
 *
 * @param editPart The EditPart to insert.
 */
void
EditPartFactory::writeToCache(EditPart* editPart) {
    created_.push_back(editPart);
}
