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
 * @file LiveRange.cc
 *
 * LiveRange is used to mark one liverange for register renaming purposes.
 *
 * @author Heikki Kultala 2011 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "LiveRange.hh"

bool
LiveRange::noneScheduled() const {
    for (DataDependenceGraph::NodeSet::iterator i = writes.begin();
         i != writes.end(); i++) {
        if ((*i)->isScheduled()) {
            return false;
        }
    }
    for (DataDependenceGraph::NodeSet::iterator i = reads.begin();
         i != reads.end(); i++) {
        if ((*i)->isScheduled()) {
            return false;
        }
    }
    return true;
}

TCEString 
LiveRange::toString() const {
    TCEString rv;
    for (DataDependenceGraph::NodeSet::iterator i = writes.begin();
         i != writes.end(); i++) {
        rv << (*i)->toString() << " ";
    }
    for (DataDependenceGraph::NodeSet::iterator i = reads.begin();
         i != reads.end(); i++) {
        rv << (*i)->toString() << " ";
    }
    return rv;
}    

int
LiveRange::firstCycle() const {
    int fc = INT_MAX;
    for (DataDependenceGraph::NodeSet::iterator i = writes.begin();
         i != writes.end(); i++) {
        if ((*i)->isScheduled()) {
            fc = std::min(fc, (*i)->cycle());
        }
    }
    for (DataDependenceGraph::NodeSet::iterator i = reads.begin();
         i != reads.end(); i++) {
        if ((*i)->isScheduled()) {
            fc = std::min(fc, (*i)->cycle());

        }
    }
    return fc;
}

int
LiveRange::lastCycle() const {
    int fc = -1;
    for (DataDependenceGraph::NodeSet::iterator i = writes.begin();
         i != writes.end(); i++) {
        if ((*i)->isScheduled()) {
            fc = std::max(fc, (*i)->cycle());
        }
    }
    for (DataDependenceGraph::NodeSet::iterator i = reads.begin();
         i != reads.end(); i++) {
        if ((*i)->isScheduled()) {
            fc = std::max(fc, (*i)->cycle());

        }
    }
    return fc;
}

void
LiveRange::clear() {
    writes.clear();
    reads.clear();
    guards.clear();
}
