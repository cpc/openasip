/*
 Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file EditPart.cc
 *
 * Definition of EditPart class.
 *
 * @author Ari Mets‰halme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2010
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#include <wx/wx.h>
#include <wx/cmdproc.h>
#include <stdlib.h>
#include <limits>
#include <math.h>

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
using std::min;
using std::max;

/**
 * The Constructor.
 */
EditPart::EditPart() :
                parent_(NULL), figure_(NULL), model_(NULL), selectable_(
                    false), selected_(false), garbageCollected_(false) {
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
 * Finds an EditPart whose Figure is nearest to the given point or first
 * found EditPart whose figure contains the given point.
 *
 * @param point Point in which to find the nearest EditPart.
 * @param exclude An EditPart which is not included in the search.
 * @return EditPart if the search found one. Otherwise returns NULL.
 */
EditPart*
EditPart::findNearest(wxPoint point, const EditPart* exclude) {
    EditPart* found = NULL;
    EditPart* lastFound = NULL;
    int lastDist = std::numeric_limits<int>::max();

    for (unsigned int i = 0; i < children_.size(); i++) {
        found = children_[i]->findNearest(point, exclude);
        if (found != NULL) {
            wxRect foundRect = found->figure()->virtualBounds();

#if wxCHECK_VERSION(2, 8, 0)
            if (foundRect.Contains(point)) {
#else
            if (foundRect.Inside(point)) {
#endif 
                assert(found->selectable());
                return found;
            }

            if (distance(point, foundRect) < lastDist) {
                lastFound = found;
                lastDist = distance(point, foundRect);
            }
        }
    }

    if (!selectable_ || this == exclude) {
        return lastFound;
    }

    // Check if this part's figure is closer to given point or
    // contains the point.
#if wxCHECK_VERSION(2, 8, 0)
    if(figure_->virtualBounds().Contains(point)) {
#else
    if (figure_->virtualBounds().Inside(point)) {
#endif  
        return this;
    } else {
        if (distance(point, figure_->virtualBounds()) < lastDist) {
            return this;
        } else {
            return lastFound;
        }
    }
}

/**
 * Recursively looks for selectable EditParts including self that are in
 * range around given coordinates.
 *
 * @param point Position for the search.
 * @param radius Search range.
 * @param found Found EditParts are collected to this.
 * @return Number of found EditParts.
 */
int
EditPart::findInRange(wxPoint point, float radius,
    std::vector<EditPart*>& found) {
    int totalFound = 0;

    for (unsigned int i = 0; i < children_.size(); i++) {
        totalFound += children_[i]->findInRange(point, radius, found);
    }

    if (!selectable_) {
        return totalFound;
    }

    if (distance(point, figure_->virtualBounds()) < radius) {
        found.push_back(this);
        return totalFound + 1;
    }
    return totalFound;
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
 * Looks for given EditPart recursively.
 * @param part EditPart to look up.
 * @return True if tree has given EditPart.
 */
bool
EditPart::hasEditPartRecursive(const EditPart* part) const {
    if (part == this) {
        return true;
    }

    for (unsigned int i = 0; i < children_.size(); i++) {
        if (children_.at(i)->hasEditPartRecursive(part)) {
            return true;
        }
    }
    return false;
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

/**
 * Calculates distance between a point and a rectangle.
 *
 * @param p Position.
 * @param r Rectangle.
 * @return Distance between point and rectangle. Zero if the point is in
 * the rectangle.
 */
float
EditPart::distance(wxPoint p, wxRect r) {
    float xr = 0;
    float yr = 0;
    float xp = static_cast<float>(p.x);
    float yp = static_cast<float>(p.y);

    xr = static_cast<float>(max(min(p.x, r.GetRight()), r.GetLeft()));
    yr = static_cast<float>(max(min(p.y, r.GetBottom()), r.GetTop()));

    float dist = sqrt((xr - xp) * (xr - xp) + (yr - yp) * (yr - yp));

    return dist;
}
