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
