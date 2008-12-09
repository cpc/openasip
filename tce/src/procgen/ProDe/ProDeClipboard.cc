/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file ProDeClipboard.cc
 *
 * Implementation of the ProDeClipboard class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include "ProDeClipboard.hh"
#include "ObjectState.hh"


ProDeClipboard* ProDeClipboard::instance_ = NULL;

/**
 * The Constructor.
 */
ProDeClipboard::ProDeClipboard() : contents_(NULL) {
}


/**
 * The Destructor.
 */
ProDeClipboard::~ProDeClipboard() {
    if (contents_ != NULL) {
        delete contents_;
        contents_ = NULL;
    }
}


/**
 * Returns the singleton instance of the clipboard.
 *
 * @return Singleton instance of the clipboard.
 */
ProDeClipboard*
ProDeClipboard::instance() {
    if (instance_ == NULL) {
        instance_ = new ProDeClipboard();
    }
    return instance_;
}


/**
 * Deletes the singleton instance of the clipboard.
 */
void
ProDeClipboard::destroy() {
    if (instance_ != NULL) {
        delete instance_;
        instance_ = NULL;
    }
}


/**
 * Sets the clipboard contents.
 *
 *†@param component Copied/cut component.
 */
void
ProDeClipboard::setContents(ObjectState* component) {
    if (contents_ != NULL) {
        delete contents_;
    }
    contents_ = component;
}


/**
 * Returns a copy of the clipboard contents.
 *
 * @return Copy of the cliboard contents.
 */
ObjectState*
ProDeClipboard::copyContents() {

    if (contents_ == NULL) {
        return NULL;
    }

    ObjectState* component = new ObjectState(*contents_);
    return component;
}


/**
 * Returns true if the clipboard is empty.
 *
 * @return true, if the clipboard is empty.
 */
bool
ProDeClipboard::isEmpty() {
    if (contents_ == NULL) {
	return true;
    }
    return false;
}
