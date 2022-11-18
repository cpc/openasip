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
 * @file EditPart.hh
 *
 * Declaration of EditPart class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#ifndef TTA_EDIT_PART_HH
#define TTA_EDIT_PART_HH

#include <vector>
#include <set>
#include <stddef.h>

class Figure;
class wxPoint;
class wxRect;
class EditPolicy;
class Request;
class ComponentCommand;

namespace TTAMachine {
    class MachinePart;
}

/**
 * Connects a model object to a graphical object on the canvas.
 *
 * An EditPart receives Requests generated by user actions and
 * converts them to possible changes in the model (Commands) using its
 * EditPolicies. EditParts can contain other EditParts as children.
 * Every EditPart has a Figure.
 */
class EditPart {
public:
    EditPart();
    virtual ~EditPart();

    void putGarbage(std::set<EditPart*>& trashbag);
    EditPart* parent() const;
    void setParent(EditPart* parent);
    TTAMachine::MachinePart* model() const;
    void setModel(TTAMachine::MachinePart* model);
    Figure* figure() const;
    void setFigure(Figure* figure);
    EditPart* find(wxPoint point);
    EditPart* find(const TTAMachine::MachinePart* model);
    EditPart* findNearest(wxPoint point, const EditPart* exclude = NULL);
    int findInRange(wxPoint point, float radius,
                    std::vector<EditPart*>& found);
    bool hasEditPartRecursive(const EditPart* part) const;
    bool selectable() const;
    bool selected() const;
    void setSelectable(bool selectable);
    void setSelected(bool select);
    void installEditPolicy(EditPolicy* editpolicy);
    void addChild(EditPart* child);
    int childCount() const ;
    EditPart* child(unsigned int index) const;
    ComponentCommand* performRequest(Request* request) const;
    bool canHandle(Request* request) const;

protected:
    /// Parent of this EditPart.
    EditPart* parent_;
    /// Figure of this EditPart.
    Figure* figure_;
    /// Machine component corresponding to this EditPart.
    TTAMachine::MachinePart* model_;
    /// Tells whether the EditPart is selectable or not.
    bool selectable_;
    /// Tells whether the EditPart is selected or not.
    bool selected_;

    /// Helper member to prevent improper deletion.
    bool garbageCollected_;

    /// List of supported EditPolicies.
    std::vector<EditPolicy*> editPolicies_;
    /// List of children EditParts.
    std::vector<EditPart*> children_;

private:
    /// Assignment not allowed.
    EditPart& operator=(EditPart& old);
    /// Copying not allowed.
    EditPart(EditPart& old);

    static float distance(wxPoint p, wxRect r);
};

#include "EditPart.icc"

#endif