/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file EditPartFactory.cc
 *
 * Definition of EditPartFactory class.
 *
 * @author Ari Mets‰halme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2010
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
