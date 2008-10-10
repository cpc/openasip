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
 * @file ValueReplacer.cc
 *
 * Definition of ValueReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <set>
#include <iterator>

#include "ValueReplacer.hh"
#include "BinaryStream.hh"
#include "SafePointer.hh"
#include "SafePointable.hh"
#include "Exception.hh"

namespace TPEF {

std::set<ValueReplacer*> ValueReplacer::replacements_;
BinaryStream* ValueReplacer::stream_ = NULL;

/**
 * Constructor
 *
 * @param obj Object whole key is used for replacing bytes in stream.
 */
ValueReplacer::ValueReplacer(const SafePointable* obj) :
    reference_(obj) {

    assert(stream_ != NULL);
    assert(obj != NULL);

    try {
        streamPosition_ = stream_->writePosition();
    } catch (UnreachableStream e) {
        bool replacerConstructorBadStream = false;
        assert(replacerConstructorBadStream);
    }
}

/**
 * Copy constructor
 *
 * @para replacer Replacer which is copied.
 */
ValueReplacer::ValueReplacer(const ValueReplacer& replacer) :
    streamPosition_(replacer.streamPosition_),
    reference_(replacer.reference_) {
}

/**
 * Destructor
 */
ValueReplacer::~ValueReplacer() {
}

/**
 * Does replacement or set it to be done later.
 *
 * @exception UnreachableStream Writing can't be done, because of bad stream.
 * @exception WritePastEOF If replacer tried to write past end of file.
 */
void
ValueReplacer::resolve()
    throw (UnreachableStream, WritePastEOF) {

    if(!tryToReplace()) {
        addReplacement(clone());
    }
}

/**
 * Initializes ValueReplacer for writing.
 *
 * Sets stream which we want write to and clears up key table of reference
 * manager so there won't be any keys with wrong values.
 *
 * @param stream Stream where replacements are done.
 */
void
ValueReplacer::initialize(BinaryStream& stream) {
    stream_ = &stream;
    assert(stream_ != NULL);
    replacements_.clear();
}

/**
 * Tries to do those replacement which couldn't do while writing.
 *
 * Deletes all remaining replacer instances after replacements were done.
 * When method is runned and no exceptions are thrown method cleans up
 * key tables of reference manager and unset's BinaryStream where
 * replacements were written. So initilize() method must be called again
 * before starting new writing. If MissingKeys exception is thrown,
 * then those replacements are not freed, which could not be done before.
 * Keys can be added and finalize may be called again.
 *
 * @exception MissingKeys If ReferenceManager doesn't contain needed keys.
 * @exception UnreachableStream Writing can't be done, because of bad stream.
 * @exception WritePastEOF If replacer tried to write past end of file.
 */
void
ValueReplacer::finalize()
    throw (MissingKeys, UnreachableStream, WritePastEOF) {

    while (!replacements_.empty()) {
        ValueReplacer* replacer = *(replacements_.begin());

        if(!replacer->tryToReplace()) {
            throw MissingKeys(
                __FILE__, __LINE__, __func__,
                "Can't replace object because of missing key.");
        }

        delete replacer;
        replacements_.erase(replacer);
    }

    stream_ = NULL;
}

/**
 * Adds a replacement object for later writing.
 *
 * @param replacer Replacement that should be done later.
 */
void
ValueReplacer::addReplacement(ValueReplacer* replacer) {
  replacements_.insert(replacements_.end(),replacer);
}

/**
 * Returns stream where to write.
 *
 * @return Stream where to write reference.
 */
BinaryStream&
ValueReplacer::stream() {
    return *stream_;
}

/**
 * Returns referenced object.
 *
 * @return Referenced object.
 */
const SafePointable*
ValueReplacer::reference() const {
    return reference_;
}

/**
 * Stream position where to write reference.
 *
 * @return Position where to write reference.
 */
unsigned int
ValueReplacer::streamPosition() const {
    return streamPosition_;
}

}
