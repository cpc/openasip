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
 * @file MoveNodeSelector.cc
 *
 * Implementation of MoveNodeSelector interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include "MoveNodeSelector.hh"
#include "Application.hh"

/**
 * Constructor.
 */
MoveNodeSelector::MoveNodeSelector() {
}

/**
 * Destructor.
 */
MoveNodeSelector::~MoveNodeSelector() {
}

/**
 * Returns a group of move nodes which should be scheduled next.
 *
 * By default returns an empty group. A non-empty group is guaranteed to
 * contain at least one unscheduled move, but not all of them necessarily
 * are.
 *
 * @return Move node group.
 */
MoveNodeGroup
MoveNodeSelector::candidates() {
    return MoveNodeGroup();
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * This should be called by the client as soon as a MoveNode is scheduled
 * in order to update the internal state of the selector.
 *
 * @param node The scheduled MoveNode.
 */
void
MoveNodeSelector::notifyScheduled(MoveNode& /*node*/) {
    assert(false && "Should never be called. Here just to avoid warning.");
}


/** 
 * This should be called by the client when some incoming dependence to
 * a moveNode is removed.
 *
 * @param node Node which has lost some incoming dependence
 */
void
MoveNodeSelector::mightBeReady(MoveNode& /*node*/) {
    assert(false && "Should never be called. Here just to avoid warning.");
}
