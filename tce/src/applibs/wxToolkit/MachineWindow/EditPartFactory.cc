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
