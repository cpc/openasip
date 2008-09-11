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
 * @file EditPart.cc
 *
 * Definition of EditPart class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#include <wx/wx.h>
#include <wx/cmdproc.h>

#include "Application.hh"
#include "ContainerTools.hh"
#include "MachinePart.hh"
#include "Port.hh"
#include "Segment.hh"
#include "EditPart.hh"
#include "Figure.hh"
#include "EditPolicy.hh"
#include "Request.hh"
#include "ComponentCommand.hh"

using std::vector;
using std::set;

/**
 * The Constructor.
 */
EditPart::EditPart():
    parent_(NULL), figure_(NULL), model_(NULL), selectable_(false),
    selected_(false), garbageCollected_(false) {
}

/**
 * The Destructor.
 *
 * @note Do not destruct this object elsewhere than in the RootEditPart
 *       destructor! Children "garbage" should be collected before 
 *       and EditPart is deleted.
 */
EditPart::~EditPart() {
    assert(garbageCollected_);
    for (unsigned int i = 0; i < editPolicies_.size(); i++) {
	delete editPolicies_[i];
    }
    editPolicies_.clear();
    delete figure_;
}

/**
 * Puts all children and this EditPart to the trashbag.
 *
 * @param trashbag The trashbag.
 */
void
EditPart::putGarbage(std::set<EditPart*>& trashbag) {
    for (unsigned i = 0; i < children_.size(); i++) {
	children_[i]->putGarbage(trashbag);
    }
    trashbag.insert(this);
    garbageCollected_ = true;
}

/**
 * Finds an EditPart whose Figure is located in the given point.
 *
 * @param point Point in which to find an EditPart.
 * @return EditPart whose Figure is located in point or NULL if none found.
 */
EditPart*
EditPart::find(wxPoint point) {

    EditPart* found = NULL;

    // first check if a child is in point
    for (unsigned int i = 0; i < children_.size(); i++) {
        found = children_[i]->find(point);
        if (found != NULL) {
            return found;
        }
    }
    
    // if no children were in located in the point, check self
#if wxCHECK_VERSION(2, 8, 0)
    if (selectable_ && figure_->virtualBounds().Contains(point)) {
#else
    if (selectable_ && figure_->virtualBounds().Inside(point)) {
#endif     
	return this;
    } else {
	return NULL;
    }
}

/**
 * Finds an EditPart which corresponds given machine component.
 *
 * @param model Machine component to find.
 * @return EditPart of the machine component, or NULL if the component was
 *         not found.
 */
EditPart*
EditPart::find(const TTAMachine::MachinePart* model) {

    if (model == model_) {
	return this;
    }

    EditPart* found = NULL;

    // Check if the component is child of this edit part.
    for (unsigned int i = 0; i < children_.size(); i++) {
	found = children_[i]->find(model);
	if (found != NULL) {
	    return found;
	}
    }

    return NULL;
}

/**
 * Installs a new EditPolicy.
 *
 * @param editpolicy The EditPolicy to be installed.
 * @note Only one EditPolicy per Request type should be installed.
 *       If two or more EditPolicies are able to handle one type of
 *       Request, it is not defined, which EditPolicy will be chosen.
 */
void
EditPart::installEditPolicy(EditPolicy* editpolicy) {
    if (editpolicy->host() == NULL) {
	editpolicy->setHost(this);
    }
    editPolicies_.push_back(editpolicy);
}

/**
 * Adds an EditPart as a child.
 *
 * @param child The new child to be added.
 */
void
EditPart::addChild(EditPart* child) {
    if (child != NULL && !ContainerTools::containsValue(children_, child)) {
	assert(figure_ != NULL);
	figure_->addChild(child->figure());
	children_.push_back(child);
    }
}

/**
 * If an EditPolicy supporting the given Request is installed to this
 * EditPart, a corresponding Command will be returned or NULL if no
 * EditPolicies supporting the Request is found.
 *
 * @param request The Request to be performed.
 * @return A command corresponding to the given Request, if an EditPolicy
 *         supporting it is installed, NULL otherwise.
 */
ComponentCommand*
EditPart::performRequest(Request* request) const {
    vector<EditPolicy*>::const_iterator i = editPolicies_.begin();
    for (; i != editPolicies_.end(); i++) {
	ComponentCommand* command = (*i)->getCommand(request);
	if (command != NULL) {
	    return command;
	}
    }
    return NULL;
}

/**
 * Tells whether this EditPart can handle a certain type of Request.
 * 
 * @param request The Request to be asked if it can be handled.
 * @return true if an EditPolicy supporting the given Request is
 *              installed to this EditPart, false otherwise.
 */
bool
EditPart::canHandle(Request* request) const {
    vector<EditPolicy*>::const_iterator i = editPolicies_.begin();
    for (; i != editPolicies_.end(); i++) {
	if ((*i)->canHandle(request)) {
	    return true;
	}
    }
    return false;
}
