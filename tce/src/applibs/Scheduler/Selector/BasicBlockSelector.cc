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
 * @file BasicBlockSelector.hh
 *
 * Dummy implementation of BasicBlockSelector interface.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include "BasicBlockSelector.hh"
#include "Application.hh"

/**
 * Constructor for BasicBlockSelector class.
 */
BasicBlockSelector::BasicBlockSelector() {
}

/**
 * Destructor for BasicBlockSelector class.
 */
BasicBlockSelector::~BasicBlockSelector() {
}

/**
 * Builds the scopes. 
 *
 * This function has to be called before getScope can be called.
 *
 * @note Why? What's wrong with constructors?
 */
void 
BasicBlockSelector::buildScopes() {
    abortWithError("Not yet implemented.");
}

/**
 * Returns a basic block to schedule next.
 *
 * The client is responsible for deallocating the returned basic block 
 * after using. This returns all basic blocks in the program one-by-one, one
 * procedure at a time, until all basic blocks have been returned.
 *
 * @return SchedulingScope, or NULL if all scopes already given.
 */
SchedulingScope*
BasicBlockSelector::getScope() {
    abortWithError("Not yet implemented.");
    return NULL;
}
