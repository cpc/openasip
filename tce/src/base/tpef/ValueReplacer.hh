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
 * @file ValueReplacer.hh
 *
 * Declaration of ValueReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_REFERENCE_REPLACER_HH
#define TTA_REFERENCE_REPLACER_HH

#include <set>

#include "SafePointable.hh"
#include "BinaryStream.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Abstract base class of value replacer classes.
 *
 * A value replacer class handles writing of object references into an
 * output binary stream.  An object reference is represented with a value in
 * the stream.  The order in which reference values are computed may depend
 * on the data already written into the stream, which may include other
 * references whose value is not yet available.  When an object reference
 * must be written out and its corresponding value is not yet available,
 * value replacers are used.
 *
 * E.g., If file offset to the body of the section should is written 
 *       while writing section header, FileOffsetReplacer is used
 *       to write FileOffset of the first element of the section.
 *       When file offset of the first element is known, the 
 *       FileOffsetReplacer object knows where to replace write that offset.
 *
 * Value replacers defer the writing of the actual reference values.  A
 * dummy reference value is written out in stream sequential order and later
 * replaced witht he actual value whent his becomes available.
 *
 * The ValueReplacer base class is not a pure interface: it handles all
 * bookkeeping of deferred replacements.
 *
 * Each concrete value replacer handles the writing of reference to a
 * specific object type in a specific data format.
 */
class ValueReplacer {
public:
    static void finalize()
        throw (MissingKeys, UnreachableStream, WritePastEOF);

    static void initialize(BinaryStream& stream);

    void resolve()
        throw (UnreachableStream, WritePastEOF);

protected:
    ValueReplacer(const SafePointable* obj);

    ValueReplacer(const ValueReplacer& replacer);

    virtual ~ValueReplacer();

    /// Does replacement if can. If can't returns false.
    virtual bool tryToReplace()
        throw (UnreachableStream, WritePastEOF) = 0;

    /// Creates dynamically allocated clone of object.
    virtual ValueReplacer* clone() = 0;

    static BinaryStream& stream();
    const SafePointable* reference() const;
    unsigned int streamPosition() const;

private:
    static void addReplacement(ValueReplacer* obj);

    /// File offset where replacement is done.
    unsigned int streamPosition_;
    /// Reference which to be written.
    const SafePointable* reference_;
    /// Stream for writing replacements.
    static BinaryStream* stream_;

    /// Replacements which should be done afterwards.
    static std::set<ValueReplacer*> replacements_;
};
}

#endif
