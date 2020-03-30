/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file Reversible.cc
 * Implementation of Reversible class; Framework for operations that contain
 * undo infromation and can be recursively reverted.
 */

#include <cassert>
#include "Reversible.hh"

/** Delete the undo information. cannot revert after this */
Reversible::~Reversible() {
    deleteChildren(postChildren_);
    deleteChildren(preChildren_);
}

/** Delete children without reverting them.
    They cannot be reverted after this */
void Reversible::deleteChildren(std::stack<Reversible*>& children) {
    while (!children.empty()) {
        Reversible* child = children.top();
        assert(child != nullptr);
        children.pop();
        delete child;
    }
}

/**
 * Undoes one stack of children.
 */
void
Reversible::undoAndRemoveChildren(std::stack<Reversible*>& children) {
    while (!children.empty()) {
        Reversible* child = children.top();
        assert(child != nullptr);
        children.pop();
        child->undo();
        delete child;
    }
}

/**
 * Undoes this and all children.
 */
void
Reversible::undo() {
    undoAndRemovePostChildren();
    undoOnlyMe();
    undoAndRemovePreChildren();
}

/**
 * Recursively calls undo for all children that were performed before this.
 * So these are undoed after this is undoed
*/
void
Reversible::undoAndRemovePreChildren() {
    undoAndRemoveChildren(preChildren_);
}

/**
 * Recursively calls undo for all children that were performed after this.
 * So these are undoed before this is undoed
*/
void
Reversible::undoAndRemovePostChildren() {
    undoAndRemoveChildren(postChildren_);
}

/**
 * Undoes the operations done by this class but not children.
 * This method should be overloaded by most derived classes.
 */
void
Reversible::undoOnlyMe() {}

/**
 * Tries to run another Reversible and takes ownership of it.
 *
 * If the running succeeds, makes it a child.
 * If the running fails, deletes it.
 *
 * @return true if running child succeeded, false if failed.
 */
bool
Reversible::runChild(std::stack<Reversible*>& children, Reversible* child) {
    if ((*child)()) {
        children.push(child);
        return true;
    } else {
        delete child;
        return false;
    }
}

/**
 * Tries to run another Reversible (before running this).
 *
 * If the running succeeds, makes it a pre-child.
 * If the running fails, deletes it.
 *
 * @return true if running child succeeded, false if failed.
 */
bool Reversible::runPreChild(Reversible* preChild) {
    return runChild(preChildren_, preChild);
}

/**
 * Tries to run another Reversible (after running this).
 *
 * If the running succeeds, makes it a post-child.
 * If the running fails, deletes it.
 *
 * @return true if running child succeeded, false if failed.
 */
bool Reversible::runPostChild(Reversible* postChild) {
    return runChild(postChildren_, postChild);
}

/**
 * Tries to run another Reversible and takes ownership of it.
 *
 * @param if pre is true, make it a pre-child,
 * if pre is false, make it a post-child.
 *
 * @return true if running child succeeded, false if failed.
 */
bool Reversible::runChild(Reversible* child, bool pre) {
    if (pre) {
        return runPreChild(child);
    } else {
        return runPostChild(child);
    }
}

int Reversible::idCounter_ = 0;
