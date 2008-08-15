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
 * @file ProDeClipboard.cc
 *
 * Implementation of the ProDeClipboard class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
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
